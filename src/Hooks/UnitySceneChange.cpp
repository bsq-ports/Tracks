#include <string>

#include "THooks.h"
#include "GameplayLifetime.hpp"
#include "GameplaySceneLifetime.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "Animation/GameObjectTrackController.hpp"

#include "GlobalNamespace/GameScenesManager.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/LoadSceneMode.hpp"
#include "System/Action.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;

static GameplaySceneLifetime gameplayLifetime;

MAKE_HOOK_MATCH(SceneManager_Internal_SceneLoaded, &UnityEngine::SceneManagement::SceneManager::Internal_SceneLoaded,
                void, UnityEngine::SceneManagement::Scene scene, UnityEngine::SceneManagement::LoadSceneMode mode) {

  if (scene.IsValid()) {
    gameplayLifetime.Loaded(scene.get_handle(), std::string(scene.get_name()));
  }
  if (scene.IsValid() && scene.get_name() == "GameCore") {
    Tracks::GameObjectTrackController::ClearData();
  }

  SceneManager_Internal_SceneLoaded(scene, mode);
}

MAKE_HOOK_MATCH(SceneManager_Internal_SceneUnloaded, &UnityEngine::SceneManagement::SceneManager::Internal_SceneUnloaded,
                void, UnityEngine::SceneManagement::Scene scene) {
  const bool unloadedGameplay = gameplayLifetime.Unloaded(scene.get_handle());
  // Unity callbacks may still use the outgoing gameplay objects. They can also
  // synchronously load a replacement GameCore, whose references must survive.
  SceneManager_Internal_SceneUnloaded(scene);
  if (unloadedGameplay && !gameplayLifetime.HasGameplay()) ClearTracksGameplayReferences();
}

void InstallSceneManagerHooks() {
  auto logger = Paper::ConstLoggerContext("Tracks | InstallBeatmapObjectCallbackControllerHooks");
  INSTALL_HOOK(logger, SceneManager_Internal_SceneLoaded);
  INSTALL_HOOK(logger, SceneManager_Internal_SceneUnloaded);
}

TInstallHooks(InstallSceneManagerHooks)
