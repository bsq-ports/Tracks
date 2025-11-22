#include "Animation/Animation.h"
#include "Animation/PointDefinition.h"
#include "AssociatedData.h"
#include "TLogger.h"
#include <optional>

using namespace TracksAD;

namespace Animation {

std::optional<PointDefinitionW> TryGetPointData(BeatmapAssociatedData& beatmapAD, rapidjson::Value const& customData,
                                 std::string_view pointName, Tracks::ffi::WrapBaseValueType type) {
  std::optional<PointDefinitionW> pointData;


  auto customDataItr = customData.FindMember(pointName.data());
  if (customDataItr == customData.MemberEnd()) {
    return pointData;
  }
  rapidjson::Value const& pointString = customDataItr->value;

  switch (pointString.GetType()) {
  case rapidjson::kNullType:
    TLogger::Logger.warn("Point definition {} is null", pointName);
    return pointData;
  case rapidjson::kStringType: {
    auto id = pointString.GetString();
    pointData = beatmapAD.GetPointDefinition(id, type);
    if (pointData) {
      TLogger::Logger.fmtLog<Paper::LogLevel::INF>("Using existing point definition {} {}", id, (int)type);
      return pointData;
    }

    auto itr = beatmapAD.pointDefinitionsRaw.find(id);
    if (itr == beatmapAD.pointDefinitionsRaw.end()) {
      TLogger::Logger.warn("Could not find point definition {}", pointString.GetString());
      return std::nullopt;
    }
    TLogger::Logger.fmtLog<Paper::LogLevel::INF>("Using point definition {} {}", pointString.GetString(), (int)type);
    auto json = convert_rapidjson(*itr->second);
    auto baseProviderContext = beatmapAD.GetBaseProviderContext();
    auto pointDataAnon = Tracks::ffi::tracks_make_base_point_definition(json, type, baseProviderContext);
    pointData = beatmapAD.AddPointDefinition(id, pointDataAnon);

    break;
  }
  default:
    auto json = convert_rapidjson(pointString);
    auto baseProviderContext = beatmapAD.GetBaseProviderContext();
    auto pointDataAnon = Tracks::ffi::tracks_make_base_point_definition(json, type, baseProviderContext);
    pointData = beatmapAD.AddPointDefinition(std::nullopt, pointDataAnon);
  }

  return pointData;
}

} // namespace Animation