#include "beatsaber-hook/shared/safeptr.hpp"
#include "AssociatedData.h"
#include "THooks.h"
#include "beatsaber-hook/shared/hooking.hpp"
#include "Animation/Events.h"

#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/BeatmapCallbacksUpdater.hpp"
#include "GlobalNamespace/BpmController.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "UnityEngine/Resources.hpp"
#include "StaticHolders.hpp"

#include "Animation/GameObjectTrackController.hpp"

using namespace GlobalNamespace;

// MAKE_HOOK_MATCH(BeatmapObjectCallbackController_LateUpdate, &BeatmapObjectCallbackController::LateUpdate, void,
// BeatmapObjectCallbackController *self) {
//     Events::UpdateCoroutines(self);
//     BeatmapObjectCallbackController_LateUpdate(self);
// }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
custom_types::Helpers::Coroutine updateCoroutines(BeatmapCallbacksController* self) {
  PAPER_IL2CPP_CATCH_HANDLER(while (true) {
    Events::UpdateCoroutines(self);
    co_yield nullptr;
  })
}
#pragma clang diagnostic pop

BeatmapCallbacksController* controller;
safe_ptr<BpmController*> TracksStatic::bpmController;

MAKE_HOOK_MATCH(BeatmapObjectCallbackController_Start, &BeatmapCallbacksController::ManualUpdate, void,
                BeatmapCallbacksController* self, float songTime) {
  BeatmapObjectCallbackController_Start(self, songTime);
  if (controller != self) {
    controller = self;

    if (auto customBeatmap = i2c::try_cast<CustomJSONData::CustomBeatmapData*>(self->_beatmapData)) {
      if (customBeatmap->customData) {
        auto& tracksBeatmapAD = TracksAD::getBeatmapAD(customBeatmap->customData);
        Tracks::GameObjectTrackController::LeftHanded = tracksBeatmapAD.leftHanded;
      }

      UnityEngine::Resources::FindObjectsOfTypeAll<BeatmapCallbacksUpdater*>()[0]->StartCoroutine(
          custom_types::Helpers::CoroutineHelper::New(updateCoroutines(self)));
    }


  }
}

MAKE_HOOK(BpmController_ctor, (i2c::class_of<BpmController*>(), ".ctor", true), void, BpmController* self,
                        BpmController::InitData* initData, BeatmapCallbacksController* beatmapCallbacksController) {
  BpmController_ctor(self, initData, beatmapCallbacksController);
  TracksStatic::bpmController = self;
}

void InstallBeatmapObjectCallbackControllerHooks() {
  auto logger = Paper::ConstLoggerContext("Tracks | InstallBeatmapObjectCallbackControllerHooks");
  INSTALL_HOOK(logger, BeatmapObjectCallbackController_Start);
  INSTALL_HOOK(logger, BpmController_ctor);
}
TInstallHooks(InstallBeatmapObjectCallbackControllerHooks)
