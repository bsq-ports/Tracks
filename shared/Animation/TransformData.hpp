#include "../Constants.h"
#include "../Vector.h"
#include "../Json.h"

#include <optional>

#include "UnityEngine/Transform.hpp"
#include "GlobalNamespace/StaticBeatmapObjectSpawnMovementData.hpp"


namespace Tracks {
    struct TransformData {
        std::optional<NEVector::Vector3> position;
        std::optional<NEVector::Vector3> localPosition;
        std::optional<NEVector::Quaternion> localRotation;
        std::optional<NEVector::Quaternion> rotation;
        std::optional<NEVector::Vector3> scale;

        TransformData() = default;
        TransformData(rapidjson::Value const& json, bool v2) {
            position = NEJSON::ReadOptionalVector3(
                json, v2 ? TracksAD::Constants::V2_POSITION : TracksAD::Constants::POSITION);
            localPosition = NEJSON::ReadOptionalVector3(
                json, v2 ? TracksAD::Constants::V2_LOCAL_POSITION : TracksAD::Constants::LOCAL_POSITION);
            localRotation = NEJSON::ReadOptionalRotation(
                json, v2 ? TracksAD::Constants::V2_LOCAL_ROTATION : TracksAD::Constants::LOCAL_ROTATION);
            rotation = NEJSON::ReadOptionalRotation(
                json, v2 ? TracksAD::Constants::V2_ROTATION : TracksAD::Constants::ROTATION);
            scale = NEJSON::ReadOptionalVector3(
                json, v2 ? TracksAD::Constants::V2_SCALE : TracksAD::Constants::SCALE);
        }

        static NEVector::Quaternion MirrorQuaternion(NEVector::Quaternion const& quat) {
            return {quat.x * -1, quat.y, quat.z * -1, quat.w};
        }
        static NEVector::Vector3 MirrorVec(NEVector::Vector3 const& v) {
            return {v.x * -1, v.y, v.z};
        }

        void Apply(UnityEngine::Transform* transform, bool leftHanded) const {
            Apply(transform, leftHanded, false);
        }

        void Apply(UnityEngine::Transform* transform, bool leftHanded, bool v2) const {
            auto position = this->position;
            auto localPosition = this->localPosition;
            auto rotation = this->rotation;
            auto localRotation = this->localRotation;
            auto scale = this->scale;

            if (leftHanded)
            {
                // if (position) position = position->Mirror();
                if (position) position = MirrorVec(position.value());
                if (localPosition) localPosition = MirrorVec(localPosition.value());
                if (rotation) rotation = MirrorQuaternion(*rotation);
                if (localRotation) localRotation = MirrorQuaternion(*localRotation);
                if (scale) scale = MirrorVec(*scale);
            }

            if (v2) {
                if (position) {
                    position = *position * GlobalNamespace::StaticBeatmapObjectSpawnMovementData::kNoteLinesDistance;
                }
                if (localPosition) {
                    localPosition = *localPosition * GlobalNamespace::StaticBeatmapObjectSpawnMovementData::kNoteLinesDistance;
                }
            }


            if (localPosition) {
                transform->set_localPosition(*localPosition);
            }else if (position) {
                transform->set_position(*position);
            }

            if (localRotation) {
                transform->set_localRotation(*localRotation);
            } else if (rotation) {
                transform->set_rotation(*rotation);
            }

            if (scale) {
                transform->set_localScale(*scale);
            }
        }
    };
}