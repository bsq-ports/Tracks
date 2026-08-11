#include "AssociatedData.h"
#include "THooks.h"
#include "TLogger.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "custom-types/shared/delegate.hpp"

#include "Animation/GameObjectTrackController.hpp"

#include "GlobalNamespace/GameplayCoreInstaller.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/PlayerTransforms.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "GlobalNamespace/IGameEnergyCounter.hpp"
#include "GlobalNamespace/ComboController.hpp"
#include "GlobalNamespace/RelativeScoreAndImmediateRankCounter.hpp"
#include "GlobalNamespace/GameSongController.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/AudioClip.hpp"
#include "System/Action_1.hpp"
#include "System/Action_2.hpp"

#include "Animation/PointDefinition.h"
#include "bindings.h"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

// i hate this
static SafePtr<CustomBeatmapData> tempCustomBeatmap;

MAKE_HOOK_MATCH(GameplayCoreInstaller_InstallBindings, &GlobalNamespace::GameplayCoreInstaller::InstallBindings, void,
                GlobalNamespace::GameplayCoreInstaller* self) {

  GameplayCoreInstaller_InstallBindings(self);
  auto colorScheme = self->_sceneSetupData->colorScheme;
  auto beatmap = self->_sceneSetupData->get_transformedBeatmapData();
  auto customBeatmapOpt = il2cpp_utils::try_cast<CustomJSONData::CustomBeatmapData>(beatmap);

  if (!customBeatmapOpt.has_value()) return;
  auto customBeatmap = tempCustomBeatmap = customBeatmapOpt.value();
  auto const& beatmapAD = TracksAD::getBeatmapAD(customBeatmap->customData);

  auto baseProviderContext = beatmapAD.GetBaseProviderContext();

  baseProviderContext->SetFloatValue("baseSongLength", self->_sceneSetupData->songAudioClip ?
                                                       self->_sceneSetupData->songAudioClip->length : 0);
  baseProviderContext->SetFloatValue("baseMultiplier", 1);
  baseProviderContext->SetFloatValue("baseRelativeScore", 1);
  baseProviderContext->SetFloatValue("baseEnergy", 0.5);

  bool leftHanded = self->_sceneSetupData->playerSpecificSettings->leftHanded;

  auto baseEnvironmentColor0 = colorScheme->environmentColor0;
  auto baseEnvironmentColor0Boost = colorScheme->environmentColor0Boost;
  auto baseEnvironmentColor1 = colorScheme->environmentColor1;
  auto baseEnvironmentColor1Boost = colorScheme->environmentColor1Boost;
  auto baseEnvironmentColorW = colorScheme->environmentColorW;
  auto baseEnvironmentColorWBoost = colorScheme->environmentColorWBoost;
  auto baseNoteColor1 = leftHanded ? colorScheme->saberAColor : colorScheme->saberBColor;
  auto baseNoteColor0 = leftHanded ? colorScheme->saberBColor : colorScheme->saberAColor;
  auto baseObstaclesColor = colorScheme->obstaclesColor;
  auto baseSaberAColor = colorScheme->saberAColor;
  auto baseSaberBColor = colorScheme->saberBColor;
  baseProviderContext->SetVector4Value("baseEnvironmentColor0", { baseEnvironmentColor0.r, baseEnvironmentColor0.g,
                                                                  baseEnvironmentColor0.b, baseEnvironmentColor0.a });
  baseProviderContext->SetVector4Value("baseEnvironmentColor0Boost",
                                       { baseEnvironmentColor0Boost.r, baseEnvironmentColor0Boost.g,
                                         baseEnvironmentColor0Boost.b, baseEnvironmentColor0Boost.a });
  baseProviderContext->SetVector4Value("baseEnvironmentColor1", { baseEnvironmentColor1.r, baseEnvironmentColor1.g,
                                                                  baseEnvironmentColor1.b, baseEnvironmentColor1.a });
  baseProviderContext->SetVector4Value("baseEnvironmentColor1Boost",
                                       { baseEnvironmentColor1Boost.r, baseEnvironmentColor1Boost.g,
                                         baseEnvironmentColor1Boost.b, baseEnvironmentColor1Boost.a });
  baseProviderContext->SetVector4Value("baseEnvironmentColorW", { baseEnvironmentColorW.r, baseEnvironmentColorW.g,
                                                                  baseEnvironmentColorW.b, baseEnvironmentColorW.a });
  baseProviderContext->SetVector4Value("baseEnvironmentColorWBoost",
                                       { baseEnvironmentColorWBoost.r, baseEnvironmentColorWBoost.g,
                                         baseEnvironmentColorWBoost.b, baseEnvironmentColorWBoost.a });
  baseProviderContext->SetVector4Value("baseNote0Color",
                                       { baseNoteColor0.r, baseNoteColor0.g, baseNoteColor0.b, baseNoteColor0.a });
  baseProviderContext->SetVector4Value("baseNote1Color",
                                       { baseNoteColor1.r, baseNoteColor1.g, baseNoteColor1.b, baseNoteColor1.a });
  baseProviderContext->SetVector4Value(
      "baseObstaclesColor", { baseObstaclesColor.r, baseObstaclesColor.g, baseObstaclesColor.b, baseObstaclesColor.a });
  baseProviderContext->SetVector4Value("baseSaberAColor",
                                       { baseSaberAColor.r, baseSaberAColor.g, baseSaberAColor.b, baseSaberAColor.a });
  baseProviderContext->SetVector4Value("baseSaberBColor",
                                       { baseSaberBColor.r, baseSaberBColor.g, baseSaberBColor.b, baseSaberBColor.a });
}

MAKE_HOOK_MATCH(PlayerTransforms_Update, &GlobalNamespace::PlayerTransforms::Update, void,
                GlobalNamespace::PlayerTransforms* self) {
  PlayerTransforms_Update(self);

  if (!tempCustomBeatmap) {
    return;
  }

  auto const& beatmapAD = TracksAD::getBeatmapAD(tempCustomBeatmap->customData);

  auto baseProviderContext = beatmapAD.GetBaseProviderContext();

  auto leftHand = self->_leftHandTransform;
  // leftHand = leftHand->parent == nullptr ? leftHand : leftHand->parent;
  auto rightHand = self->_rightHandTransform;
  // rightHand = rightHand->parent == nullptr ? rightHand : rightHand->parent;

  auto baseHeadLocalPosition = self->_headTransform->localPosition;
  auto baseHeadLocalRotation = self->_headTransform->localRotation;
  auto baseHeadLocalScale = self->_headTransform->localScale;
  auto baseHeadPosition = self->_headTransform->position;
  auto baseHeadRotation = self->_headTransform->rotation;
  auto baseLeftHandLocalPosition = leftHand->localPosition;
  auto baseLeftHandLocalRotation = leftHand->localRotation;
  auto baseLeftHandLocalScale = leftHand->localScale;
  auto baseLeftHandPosition = leftHand->position;
  auto baseLeftHandRotation = leftHand->rotation;

  auto baseRightHandLocalPosition = rightHand->localPosition;
  auto baseRightHandLocalRotation = rightHand->localRotation;
  auto baseRightHandLocalScale = rightHand->localScale;
  auto baseRightHandPosition = rightHand->position;
  auto baseRightHandRotation = rightHand->rotation;

  baseProviderContext->SetVector3Value("baseHeadLocalPosition",
                                       { baseHeadLocalPosition.x, baseHeadLocalPosition.y, baseHeadLocalPosition.z });
  baseProviderContext->SetQuatValue("baseHeadLocalRotation", { baseHeadLocalRotation.x, baseHeadLocalRotation.y,
                                                               baseHeadLocalRotation.z, baseHeadLocalRotation.w });
  baseProviderContext->SetVector3Value("baseHeadLocalScale",
                                       { baseHeadLocalScale.x, baseHeadLocalScale.y, baseHeadLocalScale.z });
  baseProviderContext->SetVector3Value("baseHeadPosition",
                                       { baseHeadPosition.x, baseHeadPosition.y, baseHeadPosition.z });
  baseProviderContext->SetQuatValue("baseHeadRotation",
                                    { baseHeadRotation.x, baseHeadRotation.y, baseHeadRotation.z, baseHeadRotation.w });
  baseProviderContext->SetVector3Value(
      "baseLeftHandLocalPosition",
      { baseLeftHandLocalPosition.x, baseLeftHandLocalPosition.y, baseLeftHandLocalPosition.z });
  baseProviderContext->SetQuatValue("baseLeftHandLocalRotation",
                                    { baseLeftHandLocalRotation.x, baseLeftHandLocalRotation.y,
                                      baseLeftHandLocalRotation.z, baseLeftHandLocalRotation.w });
  baseProviderContext->SetVector3Value(
      "baseLeftHandLocalScale", { baseLeftHandLocalScale.x, baseLeftHandLocalScale.y, baseLeftHandLocalScale.z });
  baseProviderContext->SetVector3Value("baseLeftHandPosition",
                                       { baseLeftHandPosition.x, baseLeftHandPosition.y, baseLeftHandPosition.z });
  baseProviderContext->SetQuatValue("baseLeftHandRotation", { baseLeftHandRotation.x, baseLeftHandRotation.y,
                                                              baseLeftHandRotation.z, baseLeftHandRotation.w });
  baseProviderContext->SetVector3Value(
      "baseRightHandLocalPosition",
      { baseRightHandLocalPosition.x, baseRightHandLocalPosition.y, baseRightHandLocalPosition.z });
  baseProviderContext->SetQuatValue("baseRightHandLocalRotation",
                                    { baseRightHandLocalRotation.x, baseRightHandLocalRotation.y,
                                      baseRightHandLocalRotation.z, baseRightHandLocalRotation.w });
  baseProviderContext->SetVector3Value(
      "baseRightHandLocalScale", { baseRightHandLocalScale.x, baseRightHandLocalScale.y, baseRightHandLocalScale.z });
  baseProviderContext->SetVector3Value("baseRightHandPosition",
                                       { baseRightHandPosition.x, baseRightHandPosition.y, baseRightHandPosition.z });
  baseProviderContext->SetQuatValue("baseRightHandRotation", { baseRightHandRotation.x, baseRightHandRotation.y,
                                                               baseRightHandRotation.z, baseRightHandRotation.w });
}

MAKE_HOOK_MATCH(ScoreController_Start, &GlobalNamespace::ScoreController::Start, void,
                GlobalNamespace::ScoreController* self) {
  ScoreController_Start(self);

  if (!tempCustomBeatmap)
    return;
  auto const& beatmapAD = TracksAD::getBeatmapAD(tempCustomBeatmap->customData);
  auto baseProviderContext = beatmapAD.GetBaseProviderContext();

  self->add_scoreDidChangeEvent(custom_types::MakeDelegate<System::Action_2<int, int>*>(std::function<void(int, int)>(
    [self, baseProviderContext](int multipliedScore, int modifiedScore){
      baseProviderContext->SetFloatValue("baseMultipliedScore", multipliedScore);
      baseProviderContext->SetFloatValue("baseModifiedScore", modifiedScore);
      baseProviderContext->SetFloatValue("baseImmediateMaxPossibleMultipliedScore", self->immediateMaxPossibleMultipliedScore);
      baseProviderContext->SetFloatValue("baseImmediateMaxPossibleModifiedScore", self->immediateMaxPossibleModifiedScore);
    }
  )));

  self->add_multiplierDidChangeEvent(custom_types::MakeDelegate<System::Action_2<int, float>*>(std::function<void(int, float)>(
    [baseProviderContext](int multiplier, float normalizedProgress){
      baseProviderContext->SetFloatValue("baseMultiplier", multiplier);
    }
  )));

  self->_gameEnergyCounter->add_gameEnergyDidChangeEvent(custom_types::MakeDelegate<System::Action_1<float>*>(std::function<void(float)>(
    [baseProviderContext](float energy){
      baseProviderContext->SetFloatValue("baseEnergy", energy);
    }
  )));
}

MAKE_HOOK_MATCH(ComboController_Start, &GlobalNamespace::ComboController::Start, void,
                GlobalNamespace::ComboController* self) {
  ComboController_Start(self);

  if (!tempCustomBeatmap)
    return;
  auto const& beatmapAD = TracksAD::getBeatmapAD(tempCustomBeatmap->customData);
  auto baseProviderContext = beatmapAD.GetBaseProviderContext();

  self->add_comboDidChangeEvent(custom_types::MakeDelegate<System::Action_1<int>*>(std::function<void(int)>(
    [baseProviderContext](int combo){
      baseProviderContext->SetFloatValue("baseCombo", combo);
    })));
}

MAKE_HOOK_MATCH(RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank,
                &GlobalNamespace::RelativeScoreAndImmediateRankCounter::UpdateRelativeScoreAndImmediateRank,
                void, GlobalNamespace::RelativeScoreAndImmediateRankCounter* self,
                int score, int modifiedScore, int maxPossibleScore, int maxPossibleModifiedScore) {
  RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank(
      self, score, modifiedScore, maxPossibleScore, maxPossibleModifiedScore);

  if (!tempCustomBeatmap)
    return;
  auto const& beatmapAD = TracksAD::getBeatmapAD(tempCustomBeatmap->customData);
  auto baseProviderContext = beatmapAD.GetBaseProviderContext();

  baseProviderContext->SetFloatValue("baseRelativeScore", self->relativeScore);
}

MAKE_HOOK_MATCH(GameSongController_LateUpdate, &GlobalNamespace::GameSongController::LateUpdate, void,
                GlobalNamespace::GameSongController* self) {
  GameSongController_LateUpdate(self);

  if (self->_songDidFinish)
    return;

  if (!tempCustomBeatmap)
    return;
  auto const& beatmapAD = TracksAD::getBeatmapAD(tempCustomBeatmap->customData);
  auto baseProviderContext = beatmapAD.GetBaseProviderContext();

  baseProviderContext->SetFloatValue("baseSongTime", self->_audioTimeSyncController->_songTime);
}

void InstallBaseProviderHooks() {
  auto logger = Paper::ConstLoggerContext("Tracks | InstallBaseProviderHooks");
  INSTALL_HOOK(logger, GameplayCoreInstaller_InstallBindings);
  INSTALL_HOOK(logger, PlayerTransforms_Update);
  INSTALL_HOOK(logger, ScoreController_Start);
  INSTALL_HOOK(logger, ComboController_Start);
  INSTALL_HOOK(logger, RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank);
  INSTALL_HOOK(logger, GameSongController_LateUpdate);
}

TInstallHooks(InstallBaseProviderHooks)