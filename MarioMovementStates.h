#pragma once
#include "pch.h"
#include "Debug.h"
#include "MarioStateBase.h"

class MarioWalkState;
class MarioRunState;
class MarioSkidState;
class MarioJumpState;
class MarioSitState;

using KBState = DirectX::Keyboard::State;
using KBSTracker = DirectX::Keyboard::KeyboardStateTracker;

class MarioMovementState : public MarioStateBase {
protected:
	Direction m_dir;
public:
	MarioMovementState(Direction dir) : m_dir(dir) {};
	Direction GetDirection() const;
	virtual MarioMovementState* HandleInput(Mario* mario) override = 0;
	void Update(Mario* mario, float dt) override = 0;
	virtual void Enter(Mario* mario) override;
	virtual void Exit(Mario* mario) override;
};

class MarioIdleState : public MarioMovementState {
public:
	MarioIdleState(Direction dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
};

class MarioWalkState : public MarioMovementState {
public:
	MarioWalkState(Direction dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
};

class MarioRunState : public MarioMovementState {
public:
	MarioRunState(Direction dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
private:
	bool m_isSprinting = false;
};

class MarioSkidState : public MarioMovementState {
public:
	MarioSkidState(Direction dir) : MarioMovementState(dir), m_lastDir(dir) {};
	MarioMovementState* HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
private:
	Direction m_lastDir;
};

class MarioJumpState : public MarioMovementState {
public:
	MarioJumpState(Direction dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
};	

class MarioSitState : public MarioMovementState {
public:
	MarioSitState(Direction dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
};
