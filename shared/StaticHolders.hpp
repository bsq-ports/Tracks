#pragma once
#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/BpmController.hpp"

struct __attribute__((visibility("default"))) TracksStatic {
  // Borrowed from the live runner; do not retain across callbacks or teardown.
  static GlobalNamespace::BpmController* GetBpmController(
      GlobalNamespace::BeatmapCallbacksController* controller);
};
