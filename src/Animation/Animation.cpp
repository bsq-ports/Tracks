#include "Animation/Animation.h"
#include "Animation/PointDefinition.h"
#include "AssociatedData.h"
#include "TLogger.h"
#include <memory>

using namespace TracksAD;

namespace Animation {

/// Try to get point definition from beatmap associated data
PointDefinitionW ParsePointData(BeatmapAssociatedData& beatmapAD, rapidjson::Value const& value,
                                Tracks::ffi::WrapBaseValueType type) {
  PointDefinitionW pointData = PointDefinitionW(nullptr);

  switch (value.GetType()) {
  case rapidjson::kNullType:
    return pointData;
    // refers to a PointDefinition that is associated by anme
  case rapidjson::kStringType: {
    auto id = value.GetString();

    // look for point def by id
    auto keyPair = std::pair<std::string, Tracks::ffi::WrapBaseValueType>(std::string(id), type);
    auto it = beatmapAD.pointDefinitions.find(keyPair);
    if (it != beatmapAD.pointDefinitions.end()) {
      return it->second;
    }

    auto itr = beatmapAD.pointDefinitionsJSON.find(id);
    if (itr == beatmapAD.pointDefinitionsJSON.end()) {
      TLogger::Logger.warn("Could not find point definition {}", value.GetString());
      return pointData;
    }

    // Create new point definition from JSON
    TLogger::Logger.fmtLog<Paper::LogLevel::INF>("Using point definition {} {}", value.GetString(), (int)type);
    auto baseProviderContext = beatmapAD.GetBaseProviderContext();
    pointData = PointDefinitionW(*itr->second, type, baseProviderContext);
    beatmapAD.AddPointDefinition(id, pointData);

    break;
  }
    // is a point object, parse
  default:
    auto baseProviderContext = beatmapAD.GetBaseProviderContext();
    pointData = PointDefinitionW(value, type, baseProviderContext);
    beatmapAD.AddPointDefinition(std::nullopt, pointData);
  }

  return pointData;
}

PointDefinitionW ParsePointData(BeatmapAssociatedData& beatmapAD, rapidjson::Value const& customData,
                                std::string_view customDataKey, Tracks::ffi::WrapBaseValueType type) {
  auto customDataItr = customData.FindMember(customDataKey.data());
  if (customDataItr == customData.MemberEnd()) {
    return PointDefinitionW(nullptr);
  }
  rapidjson::Value const& pointString = customDataItr->value;

  return ParsePointData(beatmapAD, pointString, type);
}

} // namespace Animation