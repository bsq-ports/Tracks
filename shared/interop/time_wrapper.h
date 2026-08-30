#pragma once
#include "../bindings.h"

namespace TracksRS {

/// Time unit wrapper with nanosecond precision
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

} // namespace TracksRS