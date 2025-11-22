#pragma once

#include <string_view>
#include <memory>

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
namespace Constants {
inline static constexpr std::string_view const V2_DURATION = "_duration";
inline static constexpr std::string_view const V2_EASING = "_easing";
inline static constexpr std::string_view const V2_NAME = "_name";
inline static constexpr std::string_view const V2_POINT_DEFINITIONS = "_pointDefinitions";
inline static constexpr std::string_view const V2_POINTS = "_points";
inline static constexpr std::string_view const V2_TRACK = "_track";
inline static constexpr std::string_view const V2_ANIMATION = "_animation";
inline static constexpr std::string_view const V2_LOCAL_ROTATION = "_localRotation";
inline static constexpr std::string_view const V2_POSITION = "_position";
inline static constexpr std::string_view const V2_LOCAL_POSITION = "_localPosition";
inline static constexpr std::string_view const V2_ROTATION = "_rotation";
inline static constexpr std::string_view const V2_SCALE = "_scale";
inline static constexpr std::string_view const V2_DEFINITE_POSITION = "_definitePosition";
inline static constexpr std::string_view const V2_DISSOLVE = "_dissolve";
inline static constexpr std::string_view const V2_DISSOLVE_ARROW = "_dissolveArrow";
inline static constexpr std::string_view const V2_CUTTABLE = "_interactable";
inline static constexpr std::string_view const V2_COLOR = "_color";
inline static constexpr std::string_view const V2_TIME = "_time";
inline static constexpr std::string_view const V2_ATTENUATION = "_attenuation";
inline static constexpr std::string_view const V2_OFFSET = "_offset";
inline static constexpr std::string_view const V2_HEIGHT_FOG_STARTY = "_startY";
inline static constexpr std::string_view const V2_HEIGHT_FOG_HEIGHT = "_height";

inline static constexpr std::string_view const INTERACTABLE = "interactable";
inline static constexpr std::string_view const DURATION = "duration";
inline static constexpr std::string_view const EASING = "easing";
inline static constexpr std::string_view const NAME = "name";
inline static constexpr std::string_view const POINT_DEFINITIONS = "pointDefinitions";
inline static constexpr std::string_view const POINTS = "points";
inline static constexpr std::string_view const TRACK = "track";
inline static constexpr std::string_view const ANIMATION = "animation";
inline static constexpr std::string_view const POSITION = "position";
inline static constexpr std::string_view const OFFSET_POSITION = "offsetPosition";
inline static constexpr std::string_view const OFFSET_ROTATION = "offsetWorldRotation";
inline static constexpr std::string_view const LOCAL_POSITION = "localPosition";
inline static constexpr std::string_view const ROTATION = "rotation";
inline static constexpr std::string_view const LOCAL_ROTATION = "localRotation";
inline static constexpr std::string_view const SCALE = "scale";
inline static constexpr std::string_view const DEFINITE_POSITION = "definitePosition";
inline static constexpr std::string_view const EVENT = "event";
inline static constexpr std::string_view const REPEAT = "repeat";
inline static constexpr std::string_view const TYPE = "type";
inline static constexpr std::string_view const EVENT_DEFINITIONS = "eventDefinitions";
inline static constexpr std::string_view const ATTENUATION = "attenuation";
inline static constexpr std::string_view const OFFSET = "offset";
inline static constexpr std::string_view const HEIGHT_FOG_STARTY = "startY";
inline static constexpr std::string_view const HEIGHT_FOG_HEIGHT = "height";
inline static constexpr std::string_view const DISSOLVE = "dissolve";
inline static constexpr std::string_view const DISSOLVE_ARROW = "dissolveArrow";
inline static constexpr std::string_view const CUTTABLE = "interactable";
inline static constexpr std::string_view const COLOR = "color";
inline static constexpr std::string_view const TIME = "time";

inline static constexpr std::string_view const LEFT_HANDED_ID = "leftHanded";
} // namespace Constants

using TracksVector = sbo::small_vector<TrackW, 1>;

enum class EventType { unknown, animateTrack, assignPathAnimation };

class BeatmapAssociatedData {
public:
  BeatmapAssociatedData() {
    internal_tracks_context = std::make_shared<TracksContext>();
  }
  ~BeatmapAssociatedData() = default;

  [[deprecated("Don't copy this!")]] BeatmapAssociatedData(BeatmapAssociatedData const&) = default;

  bool valid = false;
  bool leftHanded = false;
  bool v2;

  std::unordered_map<std::string, rapidjson::Value const*, string_hash, string_equal> pointDefinitionsRaw;

  // TODO: Use this to cache instead of Animation::TryGetPointData
  std::unordered_map<std::string, PointDefinitionW, string_hash, string_equal> pointDefinitions;

  inline PointDefinitionW getPointDefinition(rapidjson::Value const& val, std::string_view key,
                                             Tracks::ffi::WrapBaseValueType type) {
    PointDefinitionW pointData = Animation::TryGetPointData(*this, val, key, type);

    return pointData;
  }

  TrackW getTrack(Tracks::ffi::TrackKeyFFI const& trackKey) {
    return TrackW(trackKey, v2, internal_tracks_context.get()->internal_tracks_context);
  }

  // get or create
  TrackW getTrack(std::string_view name) {
    auto it = internal_tracks_context->GetTrackKey(name);
    if (it) {
      return getTrack(*it);
    }

    auto freeTrack = Tracks::ffi::track_create();
    auto trackKey =
        internal_tracks_context->AddTrack(freeTrack);

    auto ownedTrack = getTrack(trackKey);
    ownedTrack.SetName(name);

    return ownedTrack;
  }

  [[nodiscard]] std::optional<PointDefinitionW> GetPointDefinition(std::string_view name,
                                                                   Tracks::ffi::WrapBaseValueType ty) const {
    return internal_tracks_context->GetPointDefinition(name, ty);
  }

  [[nodiscard]]
  PointDefinitionW AddPointDefinition(std::optional<std::string_view> id,
                                      Tracks::ffi::BasePointDefinition* pointDefinition) const {
    return internal_tracks_context->AddPointDefinition(id, pointDefinition);
  }

  Tracks::ffi::BaseProviderContext* GetBaseProviderContext() const {
    return internal_tracks_context->GetBaseProviderContext();
  }

  Tracks::ffi::CoroutineManager* GetCoroutineManager() const {
    return internal_tracks_context->GetCoroutineManager();
  }

  Tracks::ffi::TracksHolder* GetTracksHolder() const {
    return internal_tracks_context->GetTracksHolder();
  }

private:
  std::shared_ptr<TracksContext> internal_tracks_context;
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

void clearEventADs();
} // namespace TracksAD

namespace NEJSON {
static std::optional<TracksAD::TracksVector> ReadOptionalTracks(rapidjson::Value const& object,
                                                                std::string_view const key,
                                                                TracksAD::BeatmapAssociatedData& beatmapAD) {
  auto tracksIt = object.FindMember(key.data());
  if (tracksIt != object.MemberEnd()) {
    TracksAD::TracksVector tracks;

    auto size = tracksIt->value.IsString() ? 1 : tracksIt->value.Size();
    tracks.reserve(size);

    if (tracksIt->value.IsString()) {
      tracks.emplace_back(beatmapAD.getTrack(tracksIt->value.GetString()));
    } else if (tracksIt->value.IsArray()) {
      for (auto const& it : tracksIt->value.GetArray()) {
        tracks.emplace_back(beatmapAD.getTrack(it.GetString()));
      }
    }

    return tracks;
  }
  return std::nullopt;
}
} // namespace NEJSON