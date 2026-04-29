#pragma once

#include "../bindings.h"
#include "interop/context_wrappers.h"

namespace TracksRS {

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
    Tracks::ffi::poll_events(internal_coroutine, songTime, context.internal_base_provider_context, tracksHolder);
  }
};
} // namespace TracksAD