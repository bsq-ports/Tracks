#include "Animation/Animation.h"
#include "Animation/PointDefinition.h"
#include "AssociatedData.h"
#include "TLogger.h"

using namespace TracksAD;

namespace Animation {

PointDefinitionW TryGetPointData(BeatmapAssociatedData& beatmapAD, rapidjson::Value const& customData,
                                 std::string_view pointName, Tracks::ffi::WrapBaseValueType type) {
  PointDefinitionW pointData = PointDefinitionW(nullptr);
  auto tracksContext = beatmapAD.internal_tracks_context;

  auto customDataItr = customData.FindMember(pointName.data());
  if (customDataItr == customData.MemberEnd()) {
    return pointData;
  }
  rapidjson::Value const& pointString = customDataItr->value;

  switch (pointString.GetType()) {
  case rapidjson::kNullType:
    return pointData;
  case rapidjson::kStringType: {
    auto id = pointString.GetString();
    auto existing = tracksContext->GetPointDefinition(id, type);
    if (existing) {
      TLogger::Logger.fmtLog<Paper::LogLevel::INF>("Using existing point definition {} {}", id, (int)type);
      return existing.value();
    }

    auto itr = beatmapAD.pointDefinitionsRaw.find(id);
    if (itr == beatmapAD.pointDefinitionsRaw.end()) {
      TLogger::Logger.warn("Could not find point definition {}", pointString.GetString());
      return pointData;
    }
    TLogger::Logger.fmtLog<Paper::LogLevel::INF>("Using point definition {} {}", pointString.GetString(), (int)type);
    auto json = convert_rapidjson(*itr->second);
    auto baseProviderContext = tracksContext->GetBaseProviderContext();
    auto pointDataAnon = Tracks::ffi::tracks_make_base_point_definition(json, type, baseProviderContext);
    pointData = tracksContext->AddPointDefinition(id, pointDataAnon);

    break;
  }
  default:
    auto json = convert_rapidjson(pointString);
    auto baseProviderContext = tracksContext->GetBaseProviderContext();
    auto pointDataAnon = Tracks::ffi::tracks_make_base_point_definition(json, type, baseProviderContext);
    pointData = tracksContext->AddPointDefinition(std::nullopt, pointDataAnon);
  }

  return pointData;
}

} // namespace Animation