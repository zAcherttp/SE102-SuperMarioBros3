#pragma once
#include "State.h"

class Mario;

enum class MarioMovementStateType {
	Idle,
	Walk,
	Run,
	Jump,
	Skid,
	Sit
};

enum class MarioPowerupStateType {
	Small,
	Super,
	Raccoon
};

enum class Direction { Left = -1, Right = 1 };

class MarioStateBase : protected State<Mario> {
public:
	MarioStateBase() : State() {};
	virtual MarioStateBase* HandleInput(Mario* mario) = 0;
	void Update(Mario* mario, float dt) override = 0;
	virtual void Enter(Mario* mario) override = 0;
	virtual void Exit(Mario* mario) override = 0;
	virtual std::string GetStateName() const = 0;
	virtual int GetStateAnimValue() const = 0;
	void SetAnimation(Mario* mario, int animId) const;
};

