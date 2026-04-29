#pragma once
#include "PointDefinition.h"
#include "Easings.h"
#include "Track.h"
#include "PointDefinition.h"

#include <span>
#include <optional>

#define TRACKS_LIST_OPERATE_MULTIPLE(target, list, op)                                                                 \
  if (!list.empty()) {                                                                                                 \
    target.emplace();                                                                                                  \
    for (auto const& i : list) {                                                                                       \
      target = *target op i;                                                                                           \
    }                                                                                                                  \
  }

namespace GlobalNamespace {
class BeatmapData;
}

namespace TracksAD {
struct BeatmapAssociatedData;
}

namespace Animation {

[[nodiscard]]
static auto getCurrentTime() {
  return Tracks::ffi::get_time();
}

/**
 * @brief Parse point definition from custom data. 3 possible cases:
 * 1. Point definition is a string, look up in beatmap associated data
 * 2. Point definition is an object/array, create new point definition from it
 * 3. Point definition is null/invalid, return null point definition
 *
 * Point definition is then cached in beatmap associated data
 *
 * @param beatmapAD The Beatmap context
 * @param customData the object containing the point data map e.g `{"_color": [[0,0], [1,1]]}` or `{"_posiiton":
 * "pointDef"}`
 * @param customDataKey the key to look for in customData
 * @param type The type of the point definition e.g float, vec3, quat or vec4
 * @return PointDefinitionW
 */
PointDefinitionW ParsePointData(TracksAD::BeatmapAssociatedData& beatmapAD, rapidjson::Value const& customData,
                                std::string_view customDataKey, Tracks::ffi::WrapBaseValueType type);

#pragma region track_utils

[[nodiscard]] static constexpr std::optional<NEVector::Vector3>
MirrorVectorNullable(std::optional<NEVector::Vector3> const& vector) {
  if (!vector) {
    return vector;
  }

  auto modifiedVector = *vector;
  modifiedVector.x *= -1;

  return modifiedVector;
}

[[nodiscard]] constexpr static std::optional<NEVector::Quaternion>
MirrorQuaternionNullable(std::optional<NEVector::Quaternion> const& quaternion) {
  if (!quaternion) {
    return quaternion;
  }

  auto modifiedVector = *quaternion;

  return NEVector::Quaternion(modifiedVector.x, modifiedVector.y * -1, modifiedVector.z * -1, modifiedVector.w);
}

} // namespace Animation