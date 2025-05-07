#pragma once
#include "MarioMovementStates.h"
#include "MarioPowerUpStates.h"

class MarioStateFactory {
public:
	static MarioMovementState* CreateState(MarioMovementStateType type, Direction direction);
	static MarioPowerUpState* CreateState(MarioPowerupStateType type);
};

