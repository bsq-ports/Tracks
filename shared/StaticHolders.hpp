#pragma once
#include "beatsaber-hook/shared/safeptr.hpp"

#include "GlobalNamespace/BpmController.hpp"

struct __attribute__((visibility("default"))) TracksStatic {
  static safe_ptr<GlobalNamespace::BpmController*> bpmController;
};
