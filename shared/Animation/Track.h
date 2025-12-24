#pragma once
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include "../Vector.h"
#include "../sv/small_vector.h"
#include "../Constants.h"
#include "PointDefinition.h"
#include "UnityEngine/GameObject.hpp"

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"

#include <chrono>

#include "../bindings.h"
#include "../binding_wrappers.hpp"

namespace Events {
struct AnimateTrackContext;
}

struct PropertyW;
struct PathPropertyW;

using PropertyNames = Tracks::ffi::PropertyNames;

struct TimeUnit {
  Tracks::ffi::CTimeUnit time;

  constexpr TimeUnit(Tracks::ffi::CTimeUnit time) : time(time) {}
  constexpr TimeUnit() = default;
  constexpr TimeUnit(TimeUnit const&) = default;

  [[nodiscard]] constexpr operator Tracks::ffi::CTimeUnit() const {
    return time;
  }

  // get seconds
  constexpr uint64_t get_seconds() const {
    return time._0;
  }

  // get nanoseconds
  constexpr uint64_t get_nanoseconds() const {
    return time._1;
  }

  constexpr bool operator<(TimeUnit o) const {
    return get_seconds() < o.get_seconds() ||
           (o.get_seconds() == get_seconds() && get_nanoseconds() < o.get_nanoseconds());
  }

  constexpr bool operator>(TimeUnit o) const {
    return get_seconds() > o.get_seconds() ||
           (o.get_seconds() == get_seconds() && get_nanoseconds() > o.get_nanoseconds());
  }

  constexpr bool operator==(TimeUnit o) const {
    return get_seconds() == o.get_seconds() && get_nanoseconds() == o.get_nanoseconds();
  }

  constexpr bool operator!=(TimeUnit o) const {
    return get_seconds() != o.get_seconds() || get_nanoseconds() != o.get_nanoseconds();
  }

  constexpr bool operator<=(TimeUnit o) const {
    return o == *this || *this < o;
  }

  constexpr bool operator>=(TimeUnit o) const {
    return o == *this || *this > o;
  }
};

/// Owned by TrackW
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

/// Owned by TrackW
struct PathPropertyW {
  Tracks::ffi::PathProperty* property;
  std::shared_ptr<TracksAD::BaseProviderContextW> internal_tracks_context;

  PathPropertyW(Tracks::ffi::PathProperty* property,
                std::shared_ptr<TracksAD::BaseProviderContextW> internal_tracks_context)
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

  // [[nodiscard]] float GetTime() const {
  //   return Tracks::ffi::path_property_get_time(property);
  // }

  // void SetTime(float time) const {
  //   Tracks::ffi::path_property_set_time(property, time);
  // }

  // void Finish() const {
  //   Tracks::ffi::path_property_finish(property);
  // }

  // void Init(std::optional<PointDefinitionW> newPointData) const {
  //   Tracks::ffi::path_property_init(property, newPointData.value_or(PointDefinitionW(nullptr)));
  // }
};
struct PropertiesMapW {
  PropertiesMapW(Tracks::ffi::CPropertiesMap map)
      : position(map.position), rotation(map.rotation), scale(map.scale), localRotation(map.local_rotation),
        localPosition(map.local_position), dissolve(map.dissolve), dissolveArrow(map.dissolve_arrow), time(map.time),
        cuttable(map.cuttable), color(map.color), attentuation(map.attentuation), fogOffset(map.fog_offset),
        heightFogStartY(map.height_fog_start_y), heightFogHeight(map.height_fog_height) {}

  PropertyW position;
  PropertyW rotation;
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
};

struct PathPropertiesMapW {
  PathPropertiesMapW(Tracks::ffi::CPathPropertiesMap map,
                     std::shared_ptr<TracksAD::BaseProviderContextW> internal_tracks_context)
      : position(map.position, internal_tracks_context), rotation(map.rotation, internal_tracks_context),
        scale(map.scale, internal_tracks_context), localRotation(map.local_rotation, internal_tracks_context),
        localPosition(map.local_position, internal_tracks_context),
        definitePosition(map.definite_position, internal_tracks_context),
        dissolve(map.dissolve, internal_tracks_context), dissolveArrow(map.dissolve_arrow, internal_tracks_context),
        cuttable(map.cuttable, internal_tracks_context), color(map.color, internal_tracks_context) {}

  PathPropertyW position;
  PathPropertyW rotation;
  PathPropertyW scale;
  PathPropertyW localRotation;
  PathPropertyW localPosition;
  PathPropertyW definitePosition;
  PathPropertyW dissolve;
  PathPropertyW dissolveArrow;
  PathPropertyW cuttable;
  PathPropertyW color;
};

struct PropertiesValuesW {
  constexpr PropertiesValuesW(Tracks::ffi::CPropertiesValues values)  {
    if (values.position.has_value) {
      position = NEVector::Vector3{ values.position.value.x, values.position.value.y, values.position.value.z };
    }
    if (values.rotation.has_value) {
      rotation = NEVector::Quaternion{ values.rotation.value.x, values.rotation.value.y, values.rotation.value.z,
                                       values.rotation.value.w };
    }
    if (values.scale.has_value) {
      scale = NEVector::Vector3{ values.scale.value.x, values.scale.value.y, values.scale.value.z };
    }
    if (values.local_rotation.has_value) {
      localRotation = NEVector::Quaternion{ values.local_rotation.value.x, values.local_rotation.value.y,
                                            values.local_rotation.value.z, values.local_rotation.value.w };
    }
    if (values.local_position.has_value) {
      localPosition =
          NEVector::Vector3{ values.local_position.value.x, values.local_position.value.y,
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
      color = NEVector::Vector4{ values.color.value.x, values.color.value.y, values.color.value.z,
                                 values.color.value.w };
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
  std::optional<NEVector::Quaternion> rotation;
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
  constexpr PathPropertiesValuesW(Tracks::ffi::CPathPropertiesValues values)  {
    if (values.position.has_value) {
      position = NEVector::Vector3{ values.position.value.x, values.position.value.y, values.position.value.z };
    }
    if (values.rotation.has_value) {
      rotation = NEVector::Quaternion{ values.rotation.value.x, values.rotation.value.y, values.rotation.value.z,
                                       values.rotation.value.w };
    }
    if (values.scale.has_value) {
      scale = NEVector::Vector3{ values.scale.value.x, values.scale.value.y, values.scale.value.z };
    }
    if (values.local_rotation.has_value) {
      localRotation = NEVector::Quaternion{ values.local_rotation.value.x, values.local_rotation.value.y,
                                            values.local_rotation.value.z, values.local_rotation.value.w };
    }
    if (values.local_position.has_value) {
      localPosition =
          NEVector::Vector3{ values.local_position.value.x, values.local_position.value.y,
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
      color = NEVector::Vector4{ values.color.value.x, values.color.value.y, values.color.value.z,
                                 values.color.value.w };
    }
  }

  std::optional<NEVector::Vector3> position;
  std::optional<NEVector::Quaternion> rotation;
  std::optional<NEVector::Vector3> scale;
  std::optional<NEVector::Quaternion> localRotation;
  std::optional<NEVector::Vector3> localPosition;
  std::optional<float> definitePosition;
  std::optional<float> dissolve;
  std::optional<float> dissolveArrow;
  std::optional<float> cuttable;
  std::optional<NEVector::Vector4> color;
};

struct TrackW {
  Tracks::ffi::TrackKeyFFI track = Tracks::ffi::TrackKeyFFI{ static_cast<uint64_t>(-1) };
  std::shared_ptr<TracksAD::TracksHolderW> internal_tracks_context;
  std::shared_ptr<TracksAD::BaseProviderContextW> base_provider_context;
  bool v2;

  // using CWrappedCallback = void (* *)(struct Tracks::ffi::GameObject, bool, void*);
  using CWrappedCallback = decltype(Tracks::ffi::track_register_game_object_callback(nullptr, nullptr, nullptr));

  constexpr TrackW() = default;
  TrackW(Tracks::ffi::TrackKeyFFI track, bool v2, std::shared_ptr<TracksAD::TracksHolderW> internal_tracks_context,
         std::shared_ptr<TracksAD::BaseProviderContextW> base_provider_context)
      : track(track), v2(v2), internal_tracks_context(internal_tracks_context),
        base_provider_context(base_provider_context) {}

  operator Tracks::ffi::TrackKeyFFI() const {
    return track;
  }

  operator bool() const {
    return track._0 != -1;
  }

  bool operator==(TrackW const& rhs) const {
    return this->track._0 == rhs.track._0;
  }

  bool operator<(TrackW const& rhs) const {
    return this->track._0 < rhs.track._0;
  }

  [[nodiscard]] Tracks::ffi::Track* getTrackPtr() const {
    return Tracks::ffi::tracks_holder_get_track_mut(*internal_tracks_context, track);
  }

  Tracks::ffi::PropertyNames AliasPropertyName(Tracks::ffi::PropertyNames original) const {
    // if v3, return original
    if (!v2) return original;
    // alias offsetPosition into position
    if (original == Tracks::ffi::PropertyNames::OffsetPosition) return Tracks::ffi::PropertyNames::Position;

    return original;
  }

  std::string_view AliasPropertyName(std::string_view original) const {
    // if v3, return original
    if (!v2) return original;
    // alias offsetPosition into position
    if (original == TracksAD::Constants::OFFSET_POSITION) return TracksAD::Constants::POSITION;

    return original;
  }

  [[nodiscard]] PropertyW GetProperty(std::string_view name) const {
    auto ptr = getTrackPtr();
    auto prop = Tracks::ffi::track_get_property(ptr, AliasPropertyName(name).data());
    return PropertyW(prop);
  }
  [[nodiscard]] PropertyW GetPropertyNamed(Tracks::ffi::PropertyNames name) const {
    auto ptr = getTrackPtr();
    auto prop = Tracks::ffi::track_get_property_by_name(ptr, AliasPropertyName(name));
    return PropertyW(prop);
  }

  [[nodiscard]] PathPropertyW GetPathProperty(std::string_view name) const {
    auto ptr = getTrackPtr();
    auto prop = Tracks::ffi::track_get_path_property(ptr, AliasPropertyName(name).data());
    return PathPropertyW(prop, base_provider_context);
  }
  [[nodiscard]] PathPropertyW GetPathPropertyNamed(Tracks::ffi::PropertyNames name) const {
    auto track = getTrackPtr();
    auto prop = Tracks::ffi::track_get_path_property_by_name(getTrackPtr(), AliasPropertyName(name));
    return PathPropertyW(prop, base_provider_context);
  }

  [[nodiscard]]
  PropertiesMapW GetPropertiesMapW() const {
    auto track = getTrackPtr();
    auto map = Tracks::ffi::track_get_properties_map(track);
    return PropertiesMapW(map);
  }

  [[nodiscard]]
  PathPropertiesMapW GetPathPropertiesMapW() const {
    auto track = getTrackPtr();
    auto map = Tracks::ffi::track_get_path_properties_map(track);
    return PathPropertiesMapW(map, base_provider_context);
  }

  [[nodiscard]]
  PropertiesValuesW GetPropertiesValuesW() const {
    auto track = getTrackPtr();
    auto values = Tracks::ffi::track_get_properties_values(track);
    return PropertiesValuesW(values);
  }

  [[nodiscard]]
  PathPropertiesValuesW GetPathPropertiesValuesW(float time) const {
    auto track = getTrackPtr();
    auto values = Tracks::ffi::track_get_path_properties_values(track, time, *base_provider_context);
    return PathPropertiesValuesW(values);
  }

  void RegisterGameObject(UnityEngine::GameObject* gameObject) const {
    auto ptr = getTrackPtr();
    Tracks::ffi::track_register_game_object(ptr, Tracks::ffi::GameObject{ .ptr = gameObject });
  }

  void UnregisterGameObject(UnityEngine::GameObject* gameObject) const {
    auto track = getTrackPtr();
    Tracks::ffi::track_unregister_game_object(track, Tracks::ffi::GameObject{ .ptr = gameObject });
  }

  // very nasty
  CWrappedCallback RegisterGameObjectCallback(std::function<void(UnityEngine::GameObject*, bool)> callback) const {
    if (!callback) {
      return nullptr;
    }

    // leaks memory, oh well
    auto* callbackPtr = new std::function<void(UnityEngine::GameObject*, bool)>(std::move(callback));

    // wrap the callback to a function pointer
    // this is a C-style function pointer that can be used in the FFI
    auto wrapper = +[](Tracks::ffi::GameObject gameObjectWrapper, bool isNew, void* userData) {
      auto* cb = reinterpret_cast<std::function<void(UnityEngine::GameObject*, bool)>*>(userData);
      auto gameObject = reinterpret_cast<UnityEngine::GameObject*>(const_cast<void*>(gameObjectWrapper.ptr));

      (*cb)(gameObject, isNew);
    };

    auto track = getTrackPtr();
    return Tracks::ffi::track_register_game_object_callback(track, wrapper, callbackPtr);
  }

  void RemoveGameObjectCallback(CWrappedCallback callback) const {
    if (!callback) {
      return;
    }

    auto track = getTrackPtr();
    Tracks::ffi::track_remove_game_object_callback(track, callback);
  }

  void RegisterProperty(std::string_view id, PropertyW property) {
    auto track = getTrackPtr();
    Tracks::ffi::track_register_property(track, id.data(), const_cast<Tracks::ffi::ValueProperty*>(property.property));
  }
  void RegisterPathProperty(std::string_view id, PathPropertyW property) const {
    auto track = getTrackPtr();
    Tracks::ffi::track_register_path_property(track, id.data(), property);
  }

  [[nodiscard]] Tracks::ffi::CPropertiesMap GetPropertiesMap() const {
    auto track = getTrackPtr();
    return Tracks::ffi::track_get_properties_map(track);
  }

  [[nodiscard]] Tracks::ffi::CPathPropertiesMap GetPathPropertiesMap() const {
    auto track = getTrackPtr();
    return Tracks::ffi::track_get_path_properties_map(track);
  }

  /**
   * @brief Get the Name object
   *
   * @return std::string_view Return a string view as the original string is leaked from the FFI.
   */
  [[nodiscard]] std::string_view GetName() const {
    auto track = getTrackPtr();
    return Tracks::ffi::track_get_name(track);
  }

  /**
   * @brief Set the Name object
   *
   * @param name The name to set
   */
  void SetName(std::string_view name) const {
    auto track = getTrackPtr();
    Tracks::ffi::track_set_name(track, name.data());
  }

  [[nodiscard]] std::span<UnityEngine::GameObject* const> GetGameObjects() const {
    static_assert(sizeof(UnityEngine::GameObject*) == sizeof(Tracks::ffi::GameObject),
                  "Tracks wrapper and GameObject pointer do not match size!");
    std::size_t count = 0;
    auto track = getTrackPtr();
    auto const* ptr = Tracks::ffi::track_get_game_objects(track, &count);
    auto const* castedPtr = reinterpret_cast<UnityEngine::GameObject* const*>(ptr);

    return std::span<UnityEngine::GameObject* const>(castedPtr, count);
  }
};

namespace std {
template <> struct hash<TrackW> {
  size_t operator()(TrackW const& obj) const {
    size_t track_hash = std::hash<uint64_t>()(obj.track._0);

    return track_hash;
  }
};
} // namespace std
