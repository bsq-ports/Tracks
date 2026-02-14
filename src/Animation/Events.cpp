#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"
#include "GlobalNamespace/BpmController.hpp"

#include "UnityEngine/Resources.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "bindings.h"
#include "custom-json-data/shared/CustomEventData.h"
#include "custom-json-data/shared/CustomBeatmapData.h"
#include "custom-types/shared/register.hpp"

#include "Animation/Easings.h"
#include "Animation/Events.h"
#include "Animation/Easings.h"
#include "Animation/Track.h"
#include "Animation/Animation.h"
#include "TimeSourceHelper.h"
#include "AssociatedData.h"
#include "TLogger.h"
#include "Vector.h"
#include "StaticHolders.hpp"

using namespace Events;
using namespace GlobalNamespace;
using namespace NEVector;
using namespace TracksAD;

BeatmapObjectSpawnController* spawnController;
// BeatmapObjectSpawnController.cpp

MAKE_HOOK_MATCH(BeatmapObjectSpawnController_Start, &BeatmapObjectSpawnController::Start, void,
                BeatmapObjectSpawnController* self) {
  spawnController = self;
  BeatmapObjectSpawnController_Start(self);
}

void Events::UpdateCoroutines(BeatmapCallbacksController* callbackController) {
  auto songTime = callbackController->_songTime;
  auto customBeatmapData = il2cpp_utils::cast<CustomJSONData::CustomBeatmapData>(callbackController->_beatmapData);

  auto& beatmapAD = getBeatmapAD(customBeatmapData->customData);

  auto coroutine = beatmapAD.GetCoroutineManager();
  auto baseManager = beatmapAD.GetBaseProviderContext();
  auto tracksHolder = beatmapAD.GetTracksHolder();
  coroutine->PollCoroutines(songTime, *baseManager, *tracksHolder);
}

void CustomEventCallback(BeatmapCallbacksController* callbackController,
                         CustomJSONData::CustomEventData* customEventData) {

  bool isType = false;

  auto typeHash = customEventData->typeHash;

#define TYPE_GET(jsonName, varName)                                                                                    \
  static auto jsonNameHash_##varName = std::hash<std::string_view>()(jsonName);                                        \
  if (!isType && typeHash == (jsonNameHash_##varName)) isType = true

  TYPE_GET("AnimateTrack", AnimateTrack);
  TYPE_GET("AssignPathAnimation", AssignPathAnimation);

  if (!isType) {
    return;
  }

  CustomEventAssociatedData const& eventAD = getEventAD(customEventData);

  auto* customBeatmapData = il2cpp_utils::cast<CustomJSONData::CustomBeatmapData>(callbackController->_beatmapData);
  TracksAD::BeatmapAssociatedData& beatmapAD = TracksAD::getBeatmapAD(customBeatmapData->customData);

  // fail safe, idek why this needs to be done smh
  // CJD you bugger
  if (!eventAD.parsed) {
    TLogger::Logger.debug("callbackController {}", fmt::ptr(callbackController));
    TLogger::Logger.debug("_beatmapData {}", fmt::ptr(callbackController->_beatmapData));
    TLogger::Logger.debug("customBeatmapData {}", fmt::ptr(customBeatmapData));

    if (!beatmapAD.valid) {
      TLogger::Logger.debug("Beatmap wasn't parsed when event is invoked, what?");
      TracksAD::readBeatmapDataAD(customBeatmapData);
    }

    LoadTrackEvent(customEventData, beatmapAD, customBeatmapData->v2orEarlier);
  }

  if (!TracksStatic::bpmController) {
    CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>("BPM CONTROLLER NOT INITIALIZED");
  }

  auto bpm = TracksStatic::bpmController->currentBpm; // spawnController->get_currentBpm()

  auto coroutineManager = beatmapAD.GetCoroutineManager();
  auto baseManager = beatmapAD.GetBaseProviderContext();
  auto tracksHolder = beatmapAD.GetTracksHolder();

  auto songTime = callbackController->_songTime;

  for (auto const& event : eventAD.rustEventData) {
    coroutineManager->StartCoroutine(bpm, songTime, *baseManager, *tracksHolder, *event);
  }
}

void Events::AddEventCallbacks() {
  auto logger = Paper::ConstLoggerContext("Tracks | AddEventCallbacks");
  CustomJSONData::CustomEventCallbacks::AddCustomEventCallback(&CustomEventCallback);

  INSTALL_HOOK(logger, BeatmapObjectSpawnController_Start);
}
