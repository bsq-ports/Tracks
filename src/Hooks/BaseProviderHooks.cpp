#include "AssociatedData.h"
#include "THooks.h"
#include "TLogger.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "Animation/GameObjectTrackController.hpp"

#include "GlobalNamespace/GameplayCoreInstaller.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/PlayerTransforms.hpp"
#include "UnityEngine/Transform.hpp"

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

void InstallBaseProviderHooks() {
  auto logger = Paper::ConstLoggerContext("Tracks | InstallBaseProviderHooks");
  INSTALL_HOOK(logger, GameplayCoreInstaller_InstallBindings);
  INSTALL_HOOK(logger, PlayerTransforms_Update);
}

TInstallHooks(InstallBaseProviderHooks)