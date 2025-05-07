#include "pch.h"
#include "MarioStateFactory.h"

MarioMovementState* MarioStateFactory::CreateState(MarioMovementStateType type, Direction direction) {
    switch (type) {
    case MarioMovementStateType::Idle: return new MarioIdleState(direction);
    case MarioMovementStateType::Walk: return new MarioWalkState(direction);
    case MarioMovementStateType::Run: return new MarioRunState(direction);
    case MarioMovementStateType::Jump: return new MarioJumpState(direction);
    case MarioMovementStateType::Skid: return new MarioSkidState(direction);
    case MarioMovementStateType::Sit: return new MarioSitState(direction);
    default: return new MarioIdleState(direction);
    }
}

MarioPowerUpState* MarioStateFactory::CreateState(MarioPowerupStateType type)
{
    switch (type) {
    case MarioPowerupStateType::Small: return new MarioSmallState();
    case MarioPowerupStateType::Super: return new MarioSuperState();
    case MarioPowerupStateType::Raccoon: return new MarioRaccoonState();
    default: return new MarioSmallState();
    }
}
