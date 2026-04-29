#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <variant>
#include <unordered_map>

#include "../Vector.h"
#include "../Hash.h"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "../bindings.h"
#include "context_wrappers.h"
#include "property_wrappers.h"

#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

extern Tracks::ffi::FFIJsonValue const* convert_rapidjson(rapidjson::Value const& value);

/// Wrapper for Tracks::ffi::BasePointDefinition - represents a point definition for interpolation
namespace TracksRS {

class PointDefinitionW {
public:
  explicit PointDefinitionW(rapidjson::Value const& value, Tracks::ffi::WrapBaseValueType type,
                            std::shared_ptr<TracksRS::BaseProviderContextW> base_provider_context) {
    auto* json = convert_rapidjson(value);
    this->base_provider_context = base_provider_context;

    internalPointDefinition = std::shared_ptr<Tracks::ffi::BasePointDefinition>(
        Tracks::ffi::tracks_make_base_point_definition(json, type, *base_provider_context),
        [](Tracks::ffi::BasePointDefinition* ptr) {
          if (!ptr) return;
          Tracks::ffi::base_point_definition_free(ptr);
        });
  }

  // takes ownership
  PointDefinitionW(Tracks::ffi::BasePointDefinition* pointDefinition,
                   std::shared_ptr<TracksRS::BaseProviderContextW> context)
      : base_provider_context(context) {
    internalPointDefinition =
        std::shared_ptr<Tracks::ffi::BasePointDefinition>(pointDefinition, [](Tracks::ffi::BasePointDefinition* ptr) {
          if (!ptr) return;
          Tracks::ffi::base_point_definition_free(ptr);
        });
  }

  ~PointDefinitionW() = default;

  PointDefinitionW(PointDefinitionW const& other) = default;
  explicit PointDefinitionW(std::nullptr_t) : internalPointDefinition(nullptr) {};

  [[nodiscard]]
  Tracks::ffi::WrapBaseValueType GetType() const {
    return Tracks::ffi::tracks_base_point_definition_get_type(internalPointDefinition.get());
  }

  [[nodiscard]]
  Tracks::ffi::WrapBaseValue Interpolate(float time) const {
    bool last;
    return Interpolate(time, last);
  }

  Tracks::ffi::WrapBaseValue Interpolate(float time, bool& last) const {
    auto result = Tracks::ffi::tracks_interpolate_base_point_definition(internalPointDefinition.get(), time, &last,
                                                                        *base_provider_context);
    return result;
  }

  NEVector::Vector3 InterpolateVec3(float time, bool& last) const {
    auto result = Interpolate(time, last);
    return { result.value.vec3.x, result.value.vec3.y, result.value.vec3.z };
  }

  NEVector::Quaternion InterpolateQuaternion(float time, bool& last) const {
    auto result = Interpolate(time, last);
    return { result.value.quat.x, result.value.quat.y, result.value.quat.z, result.value.quat.w };
  }

  float InterpolateLinear(float time, bool& last) const {
    auto result = Interpolate(time, last);
    return result.value.float_v;
  }

  NEVector::Vector4 InterpolateVector4(float time, bool& last) const {
    auto result = Interpolate(time, last);
    return { result.value.vec4.x, result.value.vec4.y, result.value.vec4.z, result.value.vec4.w };
  }

  NEVector::Vector3 InterpolateVec3(float time) const {
    bool last;
    return InterpolateVec3(time, last);
  }

  NEVector::Quaternion InterpolateQuaternion(float time) const {
    bool last;
    return InterpolateQuaternion(time, last);
  }

  float InterpolateLinear(float time) const {
    bool last;
    return InterpolateLinear(time, last);
  }

  NEVector::Vector4 InterpolateVector4(float time) const {
    bool last;
    return InterpolateVector4(time, last);
  }

  uintptr_t count() const {
    return Tracks::ffi::tracks_base_point_definition_count(internalPointDefinition.get());
  }

  bool hasBaseProvider() const {
    return Tracks::ffi::tracks_base_point_definition_has_base_provider(internalPointDefinition.get());
  }

  operator Tracks::ffi::BasePointDefinition const*() const {
    return internalPointDefinition.get();
  }

  operator Tracks::ffi::BasePointDefinition*() {
    return internalPointDefinition.get();
  }

private:
  constexpr PointDefinitionW() = default;

  std::shared_ptr<Tracks::ffi::BasePointDefinition> internalPointDefinition;
  std::shared_ptr<TracksRS::BaseProviderContextW> base_provider_context;
};



} // namespace TracksRS