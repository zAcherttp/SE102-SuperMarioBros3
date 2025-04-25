#pragma once
#include "pch.h"
#include "State.h"

class MarioWalkState;
class MarioRunState;
class MarioSkidState;
class MarioJumpState;
class MarioSitState;
class Mario;

class MarioStateBase : protected State<Mario> {
protected:
	int m_dir;
public:
	MarioStateBase(int dir) : m_dir(dir), State() {};
	int GetDirection() const;
	virtual MarioStateBase* HandleInput(Mario* mario, DirectX::Keyboard::KeyboardStateTracker* kbStates) = 0;
};


class MarioMovementState : protected MarioStateBase {
public:
	MarioMovementState(int dir) : MarioStateBase(dir) {};
	virtual MarioMovementState* HandleInput(Mario* mario, DirectX::Keyboard::KeyboardStateTracker* kbStates) override = 0;
	virtual void Update(Mario* mario, float dt) override = 0;
	virtual void Enter(Mario* mario) override = 0;
};

using KBState = DirectX::Keyboard;

class MarioIdleState : public MarioMovementState {
public:
	MarioIdleState(int dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario, KBState::KeyboardStateTracker* kbState) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit() override;
};

class MarioWalkState : public MarioMovementState {
public:
	MarioWalkState(int dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario, KBState::KeyboardStateTracker* kbState) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit() override;
};

class MarioRunState : public MarioMovementState {
public:
	MarioRunState(int dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario, KBState::KeyboardStateTracker* kbState) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit() override;
};

class MarioSkidState : public MarioMovementState {
public:
	MarioSkidState(int dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario, KBState::KeyboardStateTracker* kbState) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit() override;
};

class MarioJumpState : public MarioMovementState {
public:
	MarioJumpState(int dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario, KBState::KeyboardStateTracker* kbState) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit() override;
};	

class MarioSitState : public MarioMovementState {
public:
	MarioSitState(int dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario, KBState::KeyboardStateTracker* kbState) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit() override;
};
