#include "Animation/CoroutineRunner.hpp"

#include "Animation/Events.h"
#include "Animation/GameObjectTrackController.hpp"
#include "AssociatedData.h"
#include "StaticHolders.hpp"
#include "TLogger.h"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"
#include "beatsaber-hook/shared/safeptr.hpp"
#include "custom-types/shared/coroutine.hpp"

#include <stdexcept>
#include <unordered_map>

DEFINE_TYPE(Tracks, CoroutineRunner)

namespace {
using GlobalNamespace::BeatmapCallbacksController;
using Tracks::CoroutineRunner;

// Unity callbacks, creation and disposal all run on the main thread. Each key
// remains alive through the runner's managed controller field until removal.
std::unordered_map<BeatmapCallbacksController*, safe_ptr<CoroutineRunner*>> runners;

custom_types::Helpers::Coroutine UpdateCoroutines(safe_ptr<CoroutineRunner*> runner) {
  try {
    while (runner && runner->controller) {
      Events::UpdateCoroutines(runner->controller);
      co_yield nullptr;
    }
  } catch (const Il2CppExceptionWrapper& e) {
    TLogger::Logger.error("UpdateCoroutines stopped: {}", i2c::exception_to_string(e.ex));
  } catch (const std::exception& e) {
    TLogger::Logger.error("UpdateCoroutines stopped: {}", e.what());
  } catch (...) {
    TLogger::Logger.error("UpdateCoroutines stopped: unknown exception");
  }
}

custom_types::Helpers::Coroutine UpdateAnimateTrackCoroutines(safe_ptr<CoroutineRunner*> runner) {
  try {
    safe_ptr<UnityEngine::WaitForEndOfFrame*> endOfFrame(UnityEngine::WaitForEndOfFrame::New_ctor());
    while (runner && runner->controller) {
      co_yield reinterpret_cast<custom_types::Helpers::enumeratorT>(endOfFrame.ptr());
      if (!runner || !runner->controller) co_return;
      Events::UpdateAnimateTrackCoroutines(runner->controller);
    }
  } catch (const Il2CppExceptionWrapper& e) {
    TLogger::Logger.error("UpdateAnimateTrackCoroutines stopped: {}", i2c::exception_to_string(e.ex));
  } catch (const std::exception& e) {
    TLogger::Logger.error("UpdateAnimateTrackCoroutines stopped: {}", e.what());
  } catch (...) {
    TLogger::Logger.error("UpdateAnimateTrackCoroutines stopped: unknown exception");
  }
}
} // namespace

void Tracks::CoroutineRunner::BindBpm(BeatmapCallbacksController* controller,
                                     GlobalNamespace::BpmController* bpmController) {
  auto customBeatmap = i2c::try_cast<CustomJSONData::CustomBeatmapData*>(controller->_beatmapData);
  if (!customBeatmap || !customBeatmap->customData) return;

  if (auto entry = runners.find(controller); entry != runners.end()) {
    entry->second->bpmController = bpmController;
    return;
  }

  safe_ptr<UnityEngine::GameObject*> host;
  safe_ptr<CoroutineRunner*> runner;
  try {
    host = UnityEngine::GameObject::New_ctor("Tracks Coroutine Runner");
    // The controller is a managed service, not a scene component. Its Dispose
    // owns this object's lifetime, independently of the current additive scene.
    UnityEngine::Object::DontDestroyOnLoad(host.ptr());
    runner = host->AddComponent<CoroutineRunner*>();
    runner->controller = controller;
    runner->bpmController = bpmController;
    runner->pollingStarted = false;

    runners.emplace(controller, runner);
  } catch (...) {
    if (runner) runner->Stop();
    if (host) UnityEngine::Object::Destroy(host.ptr());
    throw;
  }
}

GlobalNamespace::BpmController* TracksStatic::GetBpmController(BeatmapCallbacksController* controller) {
  auto entry = runners.find(controller);
  if (entry == runners.end() || !entry->second) return nullptr;
  return entry->second->bpmController;
}

void Tracks::CoroutineRunner::EnsureStarted(BeatmapCallbacksController* controller) {
  auto entry = runners.find(controller);
  if (entry == runners.end()) return;
  auto runner = entry->second;
  if (!runner || runner->pollingStarted) return;

  try {
    // Reserve before StartCoroutine, which executes the first step synchronously.
    runner->pollingStarted = true;
    auto customBeatmap = i2c::cast<CustomJSONData::CustomBeatmapData*>(controller->_beatmapData);
    // Preserve the map's handedness so track positions and rotations are mirrored in left-handed mode.
    GameObjectTrackController::LeftHanded = TracksAD::getBeatmapAD(customBeatmap->customData).leftHanded;

    if (!runner->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(UpdateCoroutines(runner))) ||
        !runner->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(UpdateAnimateTrackCoroutines(runner)))) {
      throw std::runtime_error("Unity did not start both Tracks coroutines");
    }
  } catch (...) {
    // Stop any partial loop pair before propagating the startup failure.
    runner->StopAllCoroutines();
    runner->pollingStarted = false;
    throw;
  }
}

void Tracks::CoroutineRunner::StopFor(BeatmapCallbacksController* controller) {
  auto entry = runners.find(controller);
  if (entry == runners.end()) return;

  auto runner = entry->second;
  if (runner) {
    runner->Stop();
    UnityEngine::Object::Destroy(runner->get_gameObject());
  } else {
    runners.erase(entry);
  }
}

void Tracks::CoroutineRunner::Stop() {
  if (!controller) return;
  auto entry = runners.find(controller);
  if (entry != runners.end() && entry->second.unchecked_ptr() == this) runners.erase(entry);
  bpmController = nullptr;
  controller = nullptr;
  pollingStarted = false;
  StopAllCoroutines();
}

void Tracks::CoroutineRunner::OnDisable() {
  Stop();
  // A disabled dedicated host is no longer useful. Destroy it rather than
  // leaving an inactive persistent object holding managed references.
  UnityEngine::Object::Destroy(get_gameObject());
}

void Tracks::CoroutineRunner::OnDestroy() {
  Stop();
}
