#include "Animation/PointDefinition.h"
#include "Animation/Track.h"
#include "bindings.h"
#include <stdexcept>

namespace TracksAD {
class TracksContext {
public:
  Tracks::ffi::TracksContext* internal_tracks_context = nullptr;

  TracksContext() {
    internal_tracks_context = Tracks::ffi::tracks_context_create();
  }
  TracksContext(TracksContext const&) = delete;

  TracksContext(TracksContext&& o) noexcept : internal_tracks_context(o.internal_tracks_context) {
    o.internal_tracks_context = nullptr;
  }
  ~TracksContext() {
    if (!internal_tracks_context) {
      return;
    }
    Tracks::ffi::tracks_context_destroy(internal_tracks_context);
  }

  operator Tracks::ffi::TracksContext const*() const {
    return internal_tracks_context;
  }
  operator Tracks::ffi::TracksContext*() const {
    return internal_tracks_context;
  }

  [[nodiscard]] Tracks::ffi::CoroutineManager* GetCoroutineManager() const {
    if (!internal_tracks_context) {
      throw std::runtime_error("TracksContext is null");
    }
    return Tracks::ffi::tracks_context_get_coroutine_manager(internal_tracks_context);
  }

  [[nodiscard]] Tracks::ffi::TracksHolder* GetTracksHolder() const {
    if (!internal_tracks_context) {
      throw std::runtime_error("TracksContext is null");
    }
    return Tracks::ffi::tracks_context_get_tracks_holder(internal_tracks_context);
  }

  [[nodiscard]] Tracks::ffi::BaseProviderContext* GetBaseProviderContext() const {
    if (!internal_tracks_context) {
      throw std::runtime_error("TracksContext is null");
    }
    return Tracks::ffi::tracks_context_get_base_provider_context(internal_tracks_context);
  }

  PointDefinitionW AddPointDefinition(std::optional<std::string_view> id,
                                      Tracks::ffi::BasePointDefinition* pointDefinition) const {
    if (!internal_tracks_context) {
      throw std::runtime_error("TracksContext is null");
    }
    auto ptr = Tracks::ffi::tracks_context_add_point_definition(internal_tracks_context, id.value_or("").data(),
                                                                pointDefinition);

    return { ptr, GetBaseProviderContext() };
  }

  [[nodiscard]] std::optional<PointDefinitionW> GetPointDefinition(std::string_view name,
                                                                   Tracks::ffi::WrapBaseValueType ty) const {
    if (!internal_tracks_context) {
      throw std::runtime_error("TracksContext is null");
    }
    auto pointDefinition = Tracks::ffi::tracks_context_get_point_definition(internal_tracks_context, name.data(), ty);
    if (!pointDefinition) {
      return std::nullopt;
    }

    return PointDefinitionW(pointDefinition, GetBaseProviderContext());
  }

  Tracks::ffi::TrackKeyFFI AddTrack(TrackW track) const {
    if (!internal_tracks_context) {
      throw std::runtime_error("TracksContext is null");
    }
    auto ptr = Tracks::ffi::tracks_context_add_track(internal_tracks_context, track);

    return ptr;
  }

  TrackW GetTrack(Tracks::ffi::TrackKeyFFI const& index) const {
    if (!internal_tracks_context) {
      throw std::runtime_error("TracksContext is null");
    }
    auto track = Tracks::ffi::tracks_context_get_track(internal_tracks_context, index);
    return TrackW(track, false, internal_tracks_context);
  }

  std::optional<Tracks::ffi::TrackKeyFFI> GetTrackKey(std::string_view name) const {
    if (!internal_tracks_context) {
      throw std::runtime_error("TracksContext is null");
    }
    auto key = Tracks::ffi::tracks_context_get_track_key(internal_tracks_context, name.data());
    if (key._0 == -1) return std::nullopt;
    return key;
  }
};
}