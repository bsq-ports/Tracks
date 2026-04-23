#pragma once

#include <cstdint>
#include <memory>
#include <optional>

#include "../Vector.h"
#include "../bindings.h"

#include "./context_wrappers.h"
#include "./time_wrapper.h"

namespace TracksRS {

using PropertyNames = Tracks::ffi::PropertyNames;

/// Wrapper for Tracks::ffi::ValueProperty - represents an animated property value
/// Owned by TrackW, use as a non-owning reference
struct PropertyW {
  Tracks::ffi::ValueProperty* property;

  constexpr PropertyW() = default;
  constexpr PropertyW(Tracks::ffi::ValueProperty* property) : property(property) {}

  operator Tracks::ffi::ValueProperty const*() const {
    return property;
  }
  operator Tracks::ffi::ValueProperty*() const {
    return property;
  }
  operator bool() const {
    return property != nullptr;
  }

  [[nodiscard]] Tracks::ffi::WrapBaseValueType GetType() const {
    CRASH_UNLESS(property);
    return Tracks::ffi::property_get_type(property);
  }

  [[nodiscard]] Tracks::ffi::CValueProperty GetValue() const {
    CRASH_UNLESS(property);
    return Tracks::ffi::property_get_value(property);
  }

  [[nodiscard]] TimeUnit GetTime() const {
    CRASH_UNLESS(property);
    return Tracks::ffi::property_get_last_updated(property);
  }

  constexpr bool hasUpdated(Tracks::ffi::CValueProperty value, TimeUnit lastCheckedTime = {}) const {
    return lastCheckedTime == TimeUnit() || TimeUnit(value.last_updated) >= lastCheckedTime;
  }

  [[nodiscard]] std::optional<NEVector::Quaternion> GetQuat(TimeUnit lastCheckedTime = {}) const {
    auto value = GetValue();
    if (!value.value.has_value) {
      return std::nullopt;
    }
    if (!hasUpdated(value, lastCheckedTime)) {
      return std::nullopt;
    }
    if (value.value.value.ty != Tracks::ffi::WrapBaseValueType::Quat) {
      return std::nullopt;
    }
    auto v = value.value.value.value;
    return NEVector::Quaternion{ v.quat.x, v.quat.y, v.quat.z, v.quat.w };
  }

  [[nodiscard]] std::optional<NEVector::Vector3> GetVec3(TimeUnit lastCheckedTime = {}) const {
    auto value = GetValue();
    if (!value.value.has_value) return std::nullopt;
    if (!hasUpdated(value, lastCheckedTime)) return std::nullopt;
    if (value.value.value.ty != Tracks::ffi::WrapBaseValueType::Vec3) return std::nullopt;

    auto v = value.value.value.value;
    return NEVector::Vector3{ v.vec3.x, v.vec3.y, v.vec3.z };
  }

  [[nodiscard]] std::optional<NEVector::Vector4> GetVec4(TimeUnit lastCheckedTime = {}) const {
    auto value = GetValue();
    if (!value.value.has_value) return std::nullopt;
    if (!hasUpdated(value, lastCheckedTime)) return std::nullopt;
    if (value.value.value.ty != Tracks::ffi::WrapBaseValueType::Vec4) return std::nullopt;
    auto v = value.value.value.value;

    return NEVector::Vector4{ v.vec4.x, v.vec4.y, v.vec4.z, v.vec4.w };
  }

  [[nodiscard]] std::optional<float> GetFloat(TimeUnit lastCheckedTime = {}) const {
    auto value = GetValue();
    if (!value.value.has_value) return std::nullopt;
    if (!hasUpdated(value, lastCheckedTime)) return std::nullopt;
    if (value.value.value.ty != Tracks::ffi::WrapBaseValueType::Float) return std::nullopt;

    return value.value.value.value.float_v;
  }
};

/// Wrapper for Tracks::ffi::PathProperty - represents an animated path/curve
/// Owned by TrackW, use as a non-owning reference with context reference for interpolation
struct PathPropertyW {
  Tracks::ffi::PathProperty* property;
  std::shared_ptr<TracksRS::BaseProviderContextW> internal_tracks_context;

  PathPropertyW(Tracks::ffi::PathProperty* property,
                std::shared_ptr<TracksRS::BaseProviderContextW> internal_tracks_context)
      : property(property), internal_tracks_context(internal_tracks_context) {}

  operator Tracks::ffi::PathProperty*() const {
    return property;
  }

  operator Tracks::ffi::PathProperty const*() const {
    return property;
  }

  operator bool() const {
    return property != nullptr;
  }

  [[nodiscard]]
  float GetTime() const {
    return Tracks::ffi::path_property_get_time(property);
  }

  [[nodiscard]]
  std::optional<Tracks::ffi::WrapBaseValue> Interpolate(float time) const {
    auto result = Tracks::ffi::path_property_interpolate(property, time, *this->internal_tracks_context);
    if (!result.has_value) {
      return std::nullopt;
    }
    return result.value;
  }

  [[nodiscard]]
  std::optional<NEVector::Vector3> InterpolateVec3(float time) const {
    auto result = Interpolate(time);
    if (!result) return std::nullopt;
    auto unwrapped = *result;
    if (unwrapped.ty != Tracks::ffi::WrapBaseValueType::Vec3) {
      return std::nullopt;
    }

    return NEVector::Vector3{ unwrapped.value.vec3.x, unwrapped.value.vec3.y, unwrapped.value.vec3.z };
  }

  [[nodiscard]]
  std::optional<NEVector::Vector4> InterpolateVec4(float time) const {
    auto result = Interpolate(time);
    if (!result) return std::nullopt;
    auto unwrapped = *result;
    if (unwrapped.ty != Tracks::ffi::WrapBaseValueType::Vec4) {
      return std::nullopt;
    }

    return NEVector::Vector4{ unwrapped.value.vec4.x, unwrapped.value.vec4.y, unwrapped.value.vec4.z,
                              unwrapped.value.vec4.w };
  }

  [[nodiscard]]
  std::optional<NEVector::Quaternion> InterpolateQuat(float time) const {
    auto result = Interpolate(time);
    if (!result) return std::nullopt;
    auto unwrapped = *result;
    if (unwrapped.ty != Tracks::ffi::WrapBaseValueType::Quat) {
      return std::nullopt;
    }

    return NEVector::Quaternion{ unwrapped.value.quat.x, unwrapped.value.quat.y, unwrapped.value.quat.z,
                                 unwrapped.value.quat.w };
  }

  [[nodiscard]]
  std::optional<float> InterpolateLinear(float time) const {
    auto result = Interpolate(time);
    if (!result) return std::nullopt;
    if (result->ty != Tracks::ffi::WrapBaseValueType::Float) {
      return std::nullopt;
    }

    return result->value.float_v;
  }

  [[nodiscard]] Tracks::ffi::WrapBaseValueType GetType() const {
    return Tracks::ffi::path_property_get_type(property);
  }
};

/// Collection of all properties on a Track
struct PropertiesMapW {
  PropertyW position;
  PropertyW offsetPosition;
  PropertyW rotation;
  PropertyW offsetRotation;
  PropertyW scale;
  PropertyW localRotation;
  PropertyW localPosition;
  PropertyW dissolve;
  PropertyW dissolveArrow;
  PropertyW time;
  PropertyW cuttable;
  PropertyW color;
  PropertyW attentuation;
  PropertyW fogOffset;
  PropertyW heightFogStartY;
  PropertyW heightFogHeight;

  PropertiesMapW(Tracks::ffi::CPropertiesMap map)
      : position(map.position), offsetPosition(map.offset_position), rotation(map.rotation),
        offsetRotation(map.offset_rotation), scale(map.scale), localRotation(map.local_rotation),
        localPosition(map.local_position), dissolve(map.dissolve), dissolveArrow(map.dissolve_arrow), time(map.time),
        cuttable(map.cuttable), color(map.color), attentuation(map.attentuation), fogOffset(map.fog_offset),
        heightFogStartY(map.height_fog_start_y), heightFogHeight(map.height_fog_height) {}
};

struct PathPropertiesMapW {
  PathPropertyW position;
  PathPropertyW offsetPosition;
  PathPropertyW rotation;
  PathPropertyW offsetRotation;
  PathPropertyW scale;
  PathPropertyW localRotation;
  PathPropertyW localPosition;
  PathPropertyW definitePosition;
  PathPropertyW dissolve;
  PathPropertyW dissolveArrow;
  PathPropertyW cuttable;
  PathPropertyW color;

  PathPropertiesMapW(Tracks::ffi::CPathPropertiesMap map, std::shared_ptr<TracksRS::BaseProviderContextW> context)
      : position(map.position, context), offsetPosition(map.offset_position, context), rotation(map.rotation, context),
        offsetRotation(map.offset_rotation, context), scale(map.scale, context),
        localRotation(map.local_rotation, context), localPosition(map.local_position, context),
        definitePosition(map.definite_position, context), dissolve(map.dissolve, context),
        dissolveArrow(map.dissolve_arrow, context), cuttable(map.cuttable, context), color(map.color, context) {}
};

// Property wrapper value struct - contains all possible property values from a track
struct PropertiesValuesW {
  constexpr PropertiesValuesW(Tracks::ffi::CPropertiesValues values) {
    if (values.position.has_value) {
      position = NEVector::Vector3{ values.position.value.x, values.position.value.y, values.position.value.z };
    }
    if (values.offset_position.has_value) {
      offsetPosition = NEVector::Vector3{ values.offset_position.value.x, values.offset_position.value.y,
                                          values.offset_position.value.z };
    }
    if (values.rotation.has_value) {
      rotation = NEVector::Quaternion{ values.rotation.value.x, values.rotation.value.y, values.rotation.value.z,
                                       values.rotation.value.w };
    }
    if (values.offset_rotation.has_value) {
      offsetRotation = NEVector::Quaternion{ values.offset_rotation.value.x, values.offset_rotation.value.y,
                                             values.offset_rotation.value.z, values.offset_rotation.value.w };
    }
    if (values.scale.has_value) {
      scale = NEVector::Vector3{ values.scale.value.x, values.scale.value.y, values.scale.value.z };
    }
    if (values.local_rotation.has_value) {
      localRotation = NEVector::Quaternion{ values.local_rotation.value.x, values.local_rotation.value.y,
                                            values.local_rotation.value.z, values.local_rotation.value.w };
    }
    if (values.local_position.has_value) {
      localPosition = NEVector::Vector3{ values.local_position.value.x, values.local_position.value.y,
                                         values.local_position.value.z };
    }
    if (values.dissolve.has_value) {
      dissolve = values.dissolve.value;
    }
    if (values.dissolve_arrow.has_value) {
      dissolveArrow = values.dissolve_arrow.value;
    }
    if (values.time.has_value) {
      time = values.time.value;
    }
    if (values.cuttable.has_value) {
      cuttable = values.cuttable.value;
    }
    if (values.color.has_value) {
      color =
          NEVector::Vector4{ values.color.value.x, values.color.value.y, values.color.value.z, values.color.value.w };
    }
    if (values.attentuation.has_value) {
      attentuation = values.attentuation.value;
    }
    if (values.fog_offset.has_value) {
      fogOffset = values.fog_offset.value;
    }
    if (values.height_fog_start_y.has_value) {
      heightFogStartY = values.height_fog_start_y.value;
    }
    if (values.height_fog_height.has_value) {
      heightFogHeight = values.height_fog_height.value;
    }
  }

  std::optional<NEVector::Vector3> position;
  std::optional<NEVector::Vector3> offsetPosition;
  std::optional<NEVector::Quaternion> rotation;
  std::optional<NEVector::Quaternion> offsetRotation;
  std::optional<NEVector::Vector3> scale;
  std::optional<NEVector::Quaternion> localRotation;
  std::optional<NEVector::Vector3> localPosition;
  std::optional<float> dissolve;
  std::optional<float> dissolveArrow;
  std::optional<float> time;
  std::optional<float> cuttable;
  std::optional<NEVector::Vector4> color;
  std::optional<float> attentuation;
  std::optional<float> fogOffset;
  std::optional<float> heightFogStartY;
  std::optional<float> heightFogHeight;
};

struct PathPropertiesValuesW {
  constexpr PathPropertiesValuesW(Tracks::ffi::CPathPropertiesValues values) {
    if (values.position.has_value) {
      position = NEVector::Vector3{ values.position.value.x, values.position.value.y, values.position.value.z };
    }
    if (values.offset_position.has_value) {
      offsetPosition = NEVector::Vector3{ values.offset_position.value.x, values.offset_position.value.y,
                                          values.offset_position.value.z };
    }
    if (values.rotation.has_value) {
      rotation = NEVector::Quaternion{ values.rotation.value.x, values.rotation.value.y, values.rotation.value.z,
                                       values.rotation.value.w };
    }
    if (values.offset_rotation.has_value) {
      offsetRotation = NEVector::Quaternion{ values.offset_rotation.value.x, values.offset_rotation.value.y,
                                             values.offset_rotation.value.z, values.offset_rotation.value.w };
    }
    if (values.scale.has_value) {
      scale = NEVector::Vector3{ values.scale.value.x, values.scale.value.y, values.scale.value.z };
    }
    if (values.local_rotation.has_value) {
      localRotation = NEVector::Quaternion{ values.local_rotation.value.x, values.local_rotation.value.y,
                                            values.local_rotation.value.z, values.local_rotation.value.w };
    }
    if (values.local_position.has_value) {
      localPosition = NEVector::Vector3{ values.local_position.value.x, values.local_position.value.y,
                                         values.local_position.value.z };
    }
    if (values.definite_position.has_value) {
      definitePosition = values.definite_position.value;
    }
    if (values.dissolve.has_value) {
      dissolve = values.dissolve.value;
    }
    if (values.dissolve_arrow.has_value) {
      dissolveArrow = values.dissolve_arrow.value;
    }
    if (values.cuttable.has_value) {
      cuttable = values.cuttable.value;
    }
    if (values.color.has_value) {
      color =
          NEVector::Vector4{ values.color.value.x, values.color.value.y, values.color.value.z, values.color.value.w };
    }
  }

  std::optional<NEVector::Vector3> position;
  std::optional<NEVector::Vector3> offsetPosition;
  std::optional<NEVector::Quaternion> rotation;
  std::optional<NEVector::Quaternion> offsetRotation;
  std::optional<NEVector::Vector3> scale;
  std::optional<NEVector::Quaternion> localRotation;
  std::optional<NEVector::Vector3> localPosition;
  std::optional<float> definitePosition;
  std::optional<float> dissolve;
  std::optional<float> dissolveArrow;
  std::optional<float> cuttable;
  std::optional<NEVector::Vector4> color;
};

} // namespace TracksRS