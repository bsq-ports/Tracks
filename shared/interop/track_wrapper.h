#pragma once

#include "../bindings.h"

#include "./context_wrappers.h"
#include "./property_wrappers.h"
#include "../Constants.h"

#include "UnityEngine/GameObject.hpp"

namespace TracksRS {


struct TrackW {
  Tracks::ffi::TrackKeyFFI track = Tracks::ffi::TrackKeyFFI{ static_cast<uint64_t>(-1) };
  std::shared_ptr<TracksRS::TracksHolderW> internal_tracks_context;
  std::shared_ptr<TracksRS::BaseProviderContextW> base_provider_context;
  bool v2;

  // using CWrappedCallback = void (* *)(struct Tracks::ffi::GameObject, bool, void*);
  using CWrappedCallback = decltype(Tracks::ffi::track_register_game_object_callback(nullptr, nullptr, nullptr));

  constexpr TrackW() = default;
  TrackW(Tracks::ffi::TrackKeyFFI track, bool v2, std::shared_ptr<TracksRS::TracksHolderW> internal_tracks_context,
         std::shared_ptr<TracksRS::BaseProviderContextW> base_provider_context)
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
} // namespace TracksAD

namespace std {
template <> struct hash<TracksRS::TrackW> {
  size_t operator()(TracksRS::TrackW const& obj) const {
    size_t track_hash = std::hash<uint64_t>()(obj.track._0);

    return track_hash;
  }
};
} // namespace std
