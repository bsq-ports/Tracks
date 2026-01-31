#pragma once


#include "GlobalNamespace/IVariableMovementDataProvider.hpp"
#include "Vector.h"

// speed up interface calls

template<typename Concrete>
struct BS_HOOKS_HIDDEN VariableMovementWrapper {
  GlobalNamespace::IVariableMovementDataProvider* variable;
  Concrete* concrete;

  VariableMovementWrapper(GlobalNamespace::IVariableMovementDataProvider* variable) : variable(variable) {
    concrete = il2cpp_utils::try_cast<Concrete>(variable).value_or(nullptr);
  }

  constexpr VariableMovementWrapper(VariableMovementWrapper const&) = default;
  constexpr VariableMovementWrapper(VariableMovementWrapper&&) = default;

  operator GlobalNamespace::IVariableMovementDataProvider* () {
    return variable;
  }

  // declspec property getters for nicer call syntax (original names)
  __declspec(property(get = get_wasUpdatedThisFrame)) bool wasUpdatedThisFrame;
  __declspec(property(get = get_moveDuration)) float moveDuration;
  __declspec(property(get = get_halfJumpDuration)) float halfJumpDuration;
  __declspec(property(get = get_jumpDistance)) float jumpDistance;
  __declspec(property(get = get_jumpDuration)) float jumpDuration;
  __declspec(property(get = get_spawnAheadTime)) float spawnAheadTime;
  __declspec(property(get = get_waitingDuration)) float waitingDuration;
  __declspec(property(get = get_noteJumpSpeed)) float noteJumpSpeed;
  __declspec(property(get = get_moveStartPosition)) NEVector::Vector3 moveStartPosition;
  __declspec(property(get = get_moveEndPosition)) NEVector::Vector3 moveEndPosition;
  __declspec(property(get = get_jumpEndPosition)) NEVector::Vector3 jumpEndPosition;

  bool get_wasUpdatedThisFrame() const {
    if (!concrete) return variable->get_wasUpdatedThisFrame();

    return concrete->get_wasUpdatedThisFrame();
  }

  float get_moveDuration() const {
    if (!concrete) return variable->get_moveDuration();

    return concrete->get_moveDuration();
  }

  float get_halfJumpDuration() const {
    if (!concrete) return variable->get_halfJumpDuration();

    return concrete->get_halfJumpDuration();
  }

  float get_jumpDistance() const {
    if (!concrete) return variable->get_jumpDistance();

    return concrete->get_jumpDistance();
  }

  float get_jumpDuration() const {
    if (!concrete) return variable->get_jumpDuration();

    return concrete->get_jumpDuration();
  }

  float get_spawnAheadTime() const {
    if (!concrete) return variable->get_spawnAheadTime();

    return concrete->get_spawnAheadTime();
  }

  float get_waitingDuration() const {
    if (!concrete) return variable->get_waitingDuration();

    return concrete->get_waitingDuration();
  }

  float get_noteJumpSpeed() const {
    if (!concrete) return variable->get_noteJumpSpeed();

    return concrete->get_noteJumpSpeed();
  }

  NEVector::Vector3 get_moveStartPosition() const {
    if (!concrete) return variable->get_moveStartPosition();

    return concrete->get_moveStartPosition();
  }

  NEVector::Vector3 get_moveEndPosition() const {
    if (!concrete) return variable->get_moveEndPosition();

    return concrete->get_moveEndPosition();
  }

  NEVector::Vector3 get_jumpEndPosition() const {
    if (!concrete) return variable->get_jumpEndPosition();

    return concrete->get_jumpEndPosition();
  }

  float CalculateCurrentNoteJumpGravity(float gravityBase) const {
    if (!concrete) return variable->CalculateCurrentNoteJumpGravity(gravityBase);

    return concrete->CalculateCurrentNoteJumpGravity(gravityBase);
  }
};