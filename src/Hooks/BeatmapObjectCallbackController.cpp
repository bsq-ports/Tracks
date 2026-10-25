#include "THooks.h"
#include "beatsaber-hook/shared/hooking.hpp"
#include "Animation/CoroutineRunner.hpp"

#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/BpmController.hpp"

using namespace GlobalNamespace;

MAKE_HOOK_MATCH(BeatmapObjectCallbackController_Start, &BeatmapCallbacksController::ManualUpdate, void,
                BeatmapCallbacksController* self, float songTime) {
  BeatmapObjectCallbackController_Start(self, songTime);
  Tracks::CoroutineRunner::EnsureStarted(self);
}

MAKE_HOOK_MATCH(BeatmapCallbacksController_Dispose, &BeatmapCallbacksController::Dispose, void,
                BeatmapCallbacksController* self) {
  Tracks::CoroutineRunner::StopFor(self);
  BeatmapCallbacksController_Dispose(self);
}

MAKE_HOOK(BpmController_ctor, (i2c::class_of<BpmController*>(), ".ctor", true), void, BpmController* self,
                        BpmController::InitData* initData, BeatmapCallbacksController* beatmapCallbacksController) {
  BpmController_ctor(self, initData, beatmapCallbacksController);
  Tracks::CoroutineRunner::BindBpm(beatmapCallbacksController, self);
}

void InstallBeatmapObjectCallbackControllerHooks() {
  auto logger = Paper::ConstLoggerContext("Tracks | InstallBeatmapObjectCallbackControllerHooks");
  INSTALL_HOOK(logger, BeatmapObjectCallbackController_Start);
  INSTALL_HOOK(logger, BeatmapCallbacksController_Dispose);
  INSTALL_HOOK(logger, BpmController_ctor);
}
TInstallHooks(InstallBeatmapObjectCallbackControllerHooks)
