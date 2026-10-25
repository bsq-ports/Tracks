#pragma once

#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/BpmController.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Tracks, CoroutineRunner, UnityEngine::MonoBehaviour) {
  // A managed field keeps the controller alive and can be cleared at teardown,
  // even if Unity has not collected the stopped coroutine wrappers yet.
  DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapCallbacksController*, controller);
  DECLARE_INSTANCE_FIELD(GlobalNamespace::BpmController*, bpmController);
  DECLARE_INSTANCE_FIELD(bool, pollingStarted);

public:
  static void BindBpm(GlobalNamespace::BeatmapCallbacksController* controller,
                      GlobalNamespace::BpmController* bpmController);
  static void EnsureStarted(GlobalNamespace::BeatmapCallbacksController* controller);
  static void StopFor(GlobalNamespace::BeatmapCallbacksController* controller);

  void Stop();
  DECLARE_INSTANCE_METHOD(void, OnDisable);
  DECLARE_INSTANCE_METHOD(void, OnDestroy);
  DECLARE_DEFAULT_CTOR();
};
