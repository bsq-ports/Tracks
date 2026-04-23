#pragma once


#include "bindings.h"

namespace TracksRS {
    struct PropertyW;
    struct PathPropertyW;
    struct TimeUnit;
    struct BaseProviderContextW;
    struct TracksHolderW;
    struct TrackW;
    struct PointDefinitionW;
    struct CoroutineManagerW;
    struct EventDataW;
}

// Forward declarations - full definitions are in interop/property_wrappers.h
using PropertyW = TracksRS::PropertyW;
using PathPropertyW = TracksRS::PathPropertyW;
using EventDataW = TracksRS::EventDataW;

using TimeUnit = TracksRS::TimeUnit;

using BaseProviderContextW = TracksRS::BaseProviderContextW;
using TracksHolderW = TracksRS::TracksHolderW;

using TrackW = TracksRS::TrackW;
using PointDefinitionW = TracksRS::PointDefinitionW;
using CoroutineManagerW = TracksRS::CoroutineManagerW;
using PropertyNames = Tracks::ffi::PropertyNames;