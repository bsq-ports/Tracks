#pragma once

// Import point definition wrappers from interop module
#include "../interop/definition_wrappers.h"

// PointDefinitionW and PointDefinitionManager are now defined in interop/definition_wrappers.h
// This header simply re-exports them for backward compatibility.

/// Manager for point definitions - stores and retrieves named point definitions
class PointDefinitionManager {
public:
  std::unordered_map<std::string, rapidjson::Value const*, TracksAD::string_hash, TracksAD::string_equal> pointData;

  void AddPoint(std::string const& pointDataName, rapidjson::Value const& pointData);
};