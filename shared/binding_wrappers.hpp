#pragma once

#include "Vector.h"
#include "bindings.h"

#include <optional>
#include <stdexcept>
#include <string_view>

namespace TracksAD {
class TracksHolderW {
public:
  Tracks::ffi::TracksHolder* internal_tracks_context = nullptr;

  TracksHolderW() {
    internal_tracks_context = Tracks::ffi::tracks_holder_create();
  }
  TracksHolderW(TracksHolderW const&) = delete;

  TracksHolderW(TracksHolderW&& o) noexcept : internal_tracks_context(o.internal_tracks_context) {
    o.internal_tracks_context = nullptr;
  }
  ~TracksHolderW() {
    if (!internal_tracks_context) {
      return;
    }
    Tracks::ffi::tracks_holder_destroy(internal_tracks_context);
  }

  operator Tracks::ffi::TracksHolder const*() const {
    return internal_tracks_context;
  }
  operator Tracks::ffi::TracksHolder*() {
    return internal_tracks_context;
  }

  [[nodiscard]]
  Tracks::ffi::TrackKeyFFI AddTrack(Tracks::ffi::Track* track) const {
    if (!internal_tracks_context) {
      throw std::runtime_error("TracksContext is null");
    }
    auto ptr = Tracks::ffi::tracks_holder_add_track(internal_tracks_context, track);

    return ptr;
  }

  [[nodiscard]]
  Tracks::ffi::Track* GetTrack(Tracks::ffi::TrackKeyFFI const& index) const {
    if (!internal_tracks_context) {
      throw std::runtime_error("TracksContext is null");
    }
    auto track = Tracks::ffi::tracks_holder_get_track_mut(internal_tracks_context, index);
    return track;
  }

  [[nodiscard]]
  std::optional<Tracks::ffi::TrackKeyFFI> GetTrackKey(std::string_view name) const {
    if (!internal_tracks_context) {
      throw std::runtime_error("TracksContext is null");
    }
    auto key = Tracks::ffi::tracks_holder_get_track_key(internal_tracks_context, name.data());
    if (key._0 == -1) return std::nullopt;
    return key;
  }
};

struct BaseProviderContextW {
  Tracks::ffi::BaseProviderContext* internal_base_provider_context;

  BaseProviderContextW() {
    internal_base_provider_context = Tracks::ffi::base_provider_context_create();
  }
  BaseProviderContextW(Tracks::ffi::BaseProviderContext* context) : internal_base_provider_context(context){};
  BaseProviderContextW(BaseProviderContextW const&) = delete;
  BaseProviderContextW(BaseProviderContextW&& o) noexcept : internal_base_provider_context(o.internal_base_provider_context) {
    o.internal_base_provider_context = nullptr;
  }

  operator Tracks::ffi::BaseProviderContext*() {
    return internal_base_provider_context;
  }

  operator Tracks::ffi::BaseProviderContext const*() const {
    return internal_base_provider_context;
  }

  ~BaseProviderContextW() {
    if (internal_base_provider_context) {
      Tracks::ffi::base_provider_context_destroy(internal_base_provider_context);
    }
  }

  Tracks::ffi::WrapBaseValueType GetType(std::string_view key) const {
    return Tracks::ffi::base_provider_context_get_type(internal_base_provider_context, key.data());
  }

  [[nodiscard]]
  Tracks::ffi::WrapBaseValue GetBaseValue(std::string_view key) const {
    return Tracks::ffi::base_provider_context_get_value(internal_base_provider_context, key.data());
  }


  void SetBaseValue(std::string_view key, Tracks::ffi::WrapBaseValue const& value) {
    Tracks::ffi::base_provider_context_set_value(internal_base_provider_context, key.data(), value);
  }

  float GetFloatValue(std::string_view key) const {
    auto value = GetBaseValue(key);
    if (value.ty != Tracks::ffi::WrapBaseValueType::Float) {
      throw std::runtime_error("Value is not a float");
    }
    return value.value.float_v;
  }

  NEVector::Vector3 GetVector3Value(std::string_view key) const {
    auto value = GetBaseValue(key);
    if (value.ty != Tracks::ffi::WrapBaseValueType::Vec3) {
      throw std::runtime_error("Value is not a Vec3");
    }
    auto vec = value.value.vec3;
    return NEVector::Vector3{vec.x, vec.y, vec.z};
  }

  NEVector::Quaternion GetQuatValue(std::string_view key) const {
    auto value = GetBaseValue(key);
    if (value.ty != Tracks::ffi::WrapBaseValueType::Quat) {
      throw std::runtime_error("Value is not a Quat");
    }
    auto quat = value.value.quat;
    return NEVector::Quaternion{quat.x, quat.y, quat.z, quat.w};
  }

  NEVector::Vector4 GetVector4Value(std::string_view key) const {
    auto value = GetBaseValue(key);
    if (value.ty != Tracks::ffi::WrapBaseValueType::Vec4) {
      throw std::runtime_error("Value is not a Vec4");
    }
    auto vec = value.value.vec4;
    return NEVector::Vector4{vec.x, vec.y, vec.z, vec.w};
  }

  void SetFloatValue(std::string_view key, float value) {
    Tracks::ffi::WrapBaseValue wrapValue;
    wrapValue.ty = Tracks::ffi::WrapBaseValueType::Float;
    wrapValue.value.float_v = value;
    SetBaseValue(key, wrapValue);
  }

  void SetVector3Value(std::string_view key, NEVector::Vector3 const& value) {
    Tracks::ffi::WrapBaseValue wrapValue;
    wrapValue.ty = Tracks::ffi::WrapBaseValueType::Vec3;
    wrapValue.value.vec3 = Tracks::ffi::WrapVec3{value.x, value.y, value.z};
    SetBaseValue(key, wrapValue);
  }

  void SetQuatValue(std::string_view key, NEVector::Quaternion const& value) {
    Tracks::ffi::WrapBaseValue wrapValue;
    wrapValue.ty = Tracks::ffi::WrapBaseValueType::Quat;
    wrapValue.value.quat = Tracks::ffi::WrapQuat{value.x, value.y, value.z, value.w};
    SetBaseValue(key, wrapValue);
  }

  void SetVector4Value(std::string_view key, NEVector::Vector4 const& value) {
    Tracks::ffi::WrapBaseValue wrapValue;
    wrapValue.ty = Tracks::ffi::WrapBaseValueType::Vec4;
    wrapValue.value.vec4 = Tracks::ffi::WrapVec4{value.x, value.y, value.z, value.w};
    SetBaseValue(key, wrapValue);
  }

};

struct EventDataW {
  Tracks::ffi::EventData* internal_event_data;

  EventDataW(Tracks::ffi::EventData* event_data) : internal_event_data(event_data) {};
  EventDataW(EventDataW const&) = delete;
  EventDataW(EventDataW&& o) noexcept : internal_event_data(o.internal_event_data) {
    o.internal_event_data = nullptr;
  }

  operator Tracks::ffi::EventData*() {
    return internal_event_data;
  }

  operator Tracks::ffi::EventData const*() const {
    return internal_event_data;
  }

  ~EventDataW() {
    if (internal_event_data) {
      Tracks::ffi::event_data_dispose(internal_event_data);
    }
  }
};

struct CoroutineManagerW {
  Tracks::ffi::CoroutineManager* internal_coroutine;

  CoroutineManagerW() {
    internal_coroutine = Tracks::ffi::create_coroutine_manager();
  }
  CoroutineManagerW(Tracks::ffi::CoroutineManager* coroutine) : internal_coroutine(coroutine) {};
  CoroutineManagerW(CoroutineManagerW const&) = delete;
  CoroutineManagerW(CoroutineManagerW&& o) noexcept : internal_coroutine(o.internal_coroutine) {
    o.internal_coroutine = nullptr;
  }

  operator Tracks::ffi::CoroutineManager*() {
    return internal_coroutine;
  }

  operator Tracks::ffi::CoroutineManager const*() const {
    return internal_coroutine;
  }

  ~CoroutineManagerW() {
    if (internal_coroutine) {
      Tracks::ffi::destroy_coroutine_manager(internal_coroutine);
    }
  }

  void StartCoroutine(float bpm, float songTime, BaseProviderContextW const& context, TracksHolderW& tracksHolder,
                      EventDataW const& eventData) {
    Tracks::ffi::start_event_coroutine(internal_coroutine, bpm, songTime, context.internal_base_provider_context,
                                       tracksHolder, eventData.internal_event_data);
  }

  void PollCoroutines(float songTime, BaseProviderContextW const& context, TracksHolderW& tracksHolder) {
    Tracks::ffi::poll_events(internal_coroutine, songTime, context.internal_base_provider_context,
                             tracksHolder);
  }
};
} // namespace TracksAD