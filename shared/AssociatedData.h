#pragma once

#include <string_view>
#include <memory>

#include "Constants.h"
#include "Animation/Easings.h"
#include "Animation/Track.h"
#include "Animation/PointDefinition.h"
#include "Animation/Animation.h"
#include "Hash.h"
#include "Vector.h"
#include "bindings.h"
#include "binding_wrappers.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "custom-json-data/shared/CustomEventData.h"
#include "sv/small_vector.h"

namespace UnityEngine {
class Renderer;
}

namespace CustomJSONData {
class CustomBeatmapData;
class JSONWrapper;
} // namespace CustomJSONData

namespace TracksAD {
using TracksVector = sbo::small_vector<TrackW, 1>;

enum class EventType { unknown, animateTrack, assignPathAnimation };

class BeatmapAssociatedData {
public:
  BeatmapAssociatedData() {
    tracks_holder = std::make_shared<TracksHolderW>();
    base_provider_context = std::make_shared<BaseProviderContextW>();
    coroutine_manager = std::make_shared<CoroutineManagerW>();
    v2 = false;
  }
  ~BeatmapAssociatedData() = default;

  [[deprecated("Don't copy this!")]] BeatmapAssociatedData(BeatmapAssociatedData const&) = default;

  bool valid = false;
  bool leftHanded = false;
  bool v2;

  // custom hash and equality for pair to avoid specializing std::hash after instantiation
  struct PairHash {
    size_t operator()(std::pair<std::string, Tracks::ffi::WrapBaseValueType> const& pair) const noexcept {
      size_t name = std::hash<std::string>()(pair.first);
      size_t ty = std::hash<Tracks::ffi::WrapBaseValueType>()(pair.second);
      // boost-like combinator for better mixing
      return name ^ (ty + 0x9e3779b97f4a7c15ULL + (name << 6) + (name >> 2));
    }
  };

  struct PairEqual {
    bool operator()(std::pair<std::string, Tracks::ffi::WrapBaseValueType> const& a,
                    std::pair<std::string, Tracks::ffi::WrapBaseValueType> const& b) const noexcept {
      return a.first == b.first && a.second == b.second;
    }
  };

  std::unordered_map<std::string, rapidjson::Value const*, string_hash, string_equal> pointDefinitionsJSON;
  std::unordered_map<std::pair<std::string, Tracks::ffi::WrapBaseValueType>, PointDefinitionW, PairHash, PairEqual> pointDefinitions;
  std::vector<PointDefinitionW> pointDefinitionAnonymous;

  /**
   * @brief Get the Point Definition object and adds to map if named
   * 
   * @param customData The object map e.g `{"_color": [[0,0], [1,1]]}` or `{"_position": "foo"}`
   * @param customDataKey The key to use to look in map
   * @param type Type of point definition
   * @return PointDefinitionW 
   */
  [[nodiscard]]
  PointDefinitionW getPointDefinition(rapidjson::Value const& customData, std::string_view customDataKey,
                                                       Tracks::ffi::WrapBaseValueType type) {
    auto pointData = Animation::ParsePointData(*this, customData, customDataKey, type);

    return pointData;
  }

  /**
   * @brief Adds a point definition to the beatmap. Keeps them alive.
   * 
   * @param id ID of the point definition, or null if anonymous.
   * @param pointDefinition 
   */
  void AddPointDefinition(std::optional<std::string_view> id, PointDefinitionW pointDefinition) {
    if (!pointDefinition) {
      return;
    }
    if (!id) {
      pointDefinitionAnonymous.emplace_back(pointDefinition);
      return;
    }

    auto type = pointDefinition.GetType();
    auto pair = std::pair<std::string, Tracks::ffi::WrapBaseValueType>(std::string(*id), type);
    pointDefinitions.emplace(pair, pointDefinition);
  }

  TrackW getTrack(Tracks::ffi::TrackKeyFFI const& trackKey) {
    return TrackW(trackKey, v2, tracks_holder, base_provider_context);
  }

  // get or create
  TrackW getTrack(std::string_view name) {
    auto it = tracks_holder->GetTrackKey(name);
    if (it) {
      return getTrack(*it);
    }

    auto freeTrack = Tracks::ffi::track_create();
    auto trackKey = tracks_holder->AddTrack(freeTrack);

    auto ownedTrack = getTrack(trackKey);
    ownedTrack.SetName(name);

    return ownedTrack;
  }

  std::shared_ptr<BaseProviderContextW> GetBaseProviderContext() const {
    return base_provider_context;
  }

  std::shared_ptr<CoroutineManagerW> GetCoroutineManager() const {
    return coroutine_manager;
  }

  std::shared_ptr<TracksHolderW> GetTracksHolder() const {
    return tracks_holder;
  }

private:
  std::shared_ptr<TracksHolderW> tracks_holder;
  std::shared_ptr<BaseProviderContextW> base_provider_context;
  std::shared_ptr<CoroutineManagerW> coroutine_manager;
};

struct BeatmapObjectAssociatedData {
  // Should this be an optional? - Fern
  TracksVector tracks;
};

using PropertyId = std::variant<std::string, Tracks::ffi::PropertyNames>;
using PathPropertyId = std::variant<std::string, Tracks::ffi::PropertyNames>;

struct CustomEventAssociatedData {
  // This can probably be omitted or a set
  // TracksVector tracks;
  float duration;
  Functions easing;
  uint32_t repeat;

  EventType type;
  sbo::small_vector<std::shared_ptr<EventDataW>, 1> rustEventData;

  bool parsed = false;
};

void LoadTrackEvent(CustomJSONData::CustomEventData* customEventData, TracksAD::BeatmapAssociatedData& beatmapAD,
                    bool v2);
void readBeatmapDataAD(CustomJSONData::CustomBeatmapData* beatmapData);
BeatmapAssociatedData& getBeatmapAD(CustomJSONData::JSONWrapper* customData);
BeatmapObjectAssociatedData& getAD(CustomJSONData::JSONWrapper* customData);
CustomEventAssociatedData& getEventAD(CustomJSONData::CustomEventData const* customEventData);


} // namespace TracksAD

namespace NEJSON {
static std::optional<TracksAD::TracksVector> ReadOptionalTracks(rapidjson::Value const& object,
                                                                std::string_view const key,
                                                                TracksAD::BeatmapAssociatedData& beatmapAD) {
  auto tracksIt = object.FindMember(key.data());
  if (tracksIt != object.MemberEnd()) {
    TracksAD::TracksVector tracks;

// Removed problematic std::hash specialization for std::pair<...> to avoid "specialization after instantiation" errors;
// a custom PairHash / PairEqual in the TracksAD namespace is used instead.
    return tracks;
  }
  return std::nullopt;
}
} // namespace NEJSON