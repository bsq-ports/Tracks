#include "beatsaber-hook/shared/hooking.hpp"

#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnController.hpp"
#include "GlobalNamespace/BpmController.hpp"
#include "UnityEngine/Time.hpp"

#include "UnityEngine/Resources.hpp"

#include "beatsaber-hook/shared/hooking.hpp"
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

namespace {
// Keep the new queue in the JSON wrapper's associated data so its lifetime follows
// the beatmap, without changing the public BeatmapAssociatedData layout.
constexpr char AnimateTrackQueueKey = 't';

std::shared_ptr<CoroutineManagerW> GetAnimateTrackQueue(CustomJSONData::JSONWrapper* customData) {
  auto& queue = customData->associatedData[AnimateTrackQueueKey];
  if (!queue.has_value()) queue = std::make_shared<CoroutineManagerW>();
  return std::any_cast<std::shared_ptr<CoroutineManagerW>>(queue);
}
} // namespace

MAKE_HOOK_MATCH(BeatmapObjectSpawnController_Start, &BeatmapObjectSpawnController::Start, void,
                BeatmapObjectSpawnController* self) {
  spawnController = self;
  BeatmapObjectSpawnController_Start(self);
}

void Events::UpdateCoroutines(BeatmapCallbacksController* callbackController) {
  auto songTime = callbackController->_songTime;
  auto customBeatmapData = i2c::cast<CustomJSONData::CustomBeatmapData*>(callbackController->_beatmapData);

  auto& beatmapAD = getBeatmapAD(customBeatmapData->customData);

  auto coroutine = beatmapAD.GetCoroutineManager();
  auto baseManager = beatmapAD.GetBaseProviderContext();
  auto tracksHolder = beatmapAD.GetTracksHolder();
  baseManager->Update(UnityEngine::Time::get_deltaTime());
  coroutine->PollCoroutines(songTime, *baseManager, *tracksHolder);
}

void Events::UpdateAnimateTrackCoroutines(BeatmapCallbacksController* callbackController) {
  auto customBeatmapData = i2c::cast<CustomJSONData::CustomBeatmapData*>(callbackController->_beatmapData);
  auto customData = customBeatmapData->customData;
  auto queue = customData->associatedData.find(AnimateTrackQueueKey);
  if (queue == customData->associatedData.end()) return;

  auto& beatmapAD = getBeatmapAD(customData);
  auto const& coroutine = std::any_cast<std::shared_ptr<CoroutineManagerW> const&>(queue->second);
  // Base providers retain their existing once-per-frame update in UpdateCoroutines.
  coroutine->PollCoroutines(callbackController->_songTime, *beatmapAD.GetBaseProviderContext(),
                            *beatmapAD.GetTracksHolder());
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

  auto* customBeatmapData = i2c::cast<CustomJSONData::CustomBeatmapData*>(callbackController->_beatmapData);
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

  auto* bpmController = TracksStatic::GetBpmController(callbackController);
  if (!bpmController) {
    CJDLogger::Logger.fmtLog<Paper::LogLevel::ERR>("BPM CONTROLLER NOT INITIALIZED");
    return;
  }

  auto bpm = bpmController->currentBpm;

  // PC advances AnimateTrack at end of frame; AssignPathAnimation yields null.
  // Starting an event still evaluates its first (or zero-duration final) value now.
  auto coroutineManager = eventAD.type == EventType::animateTrack
                              ? GetAnimateTrackQueue(customBeatmapData->customData)
                              : beatmapAD.GetCoroutineManager();
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
