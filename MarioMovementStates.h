#pragma once

#include "State.h"

class MarioWalkState;
class MarioRunState;
class MarioSkidState;
class MarioJumpState;
class MarioSitState;

class MarioStateBase : protected State {
protected:
	int m_dir;
public:
	MarioStateBase(Entity* mario, int dir) : State(mario) {
		m_dir = dir;
	};

	Mario* GetMario();
	int GetDirection() const;
};


class MarioMovementState : protected MarioStateBase {
public:
	MarioMovementState(Entity* mario, int dir) : MarioStateBase(mario, dir) {};
};

using KBState = DirectX::Keyboard;

class MarioIdleState : public MarioMovementState {
public:
	MarioIdleState(Entity* mario, int dir) : MarioMovementState(mario, dir) {};
	State* HandleInput(KBState::KeyboardStateTracker* kbState) override;
	void Update(float dt) override;
	void Enter() override;
};

class MarioWalkState : public MarioMovementState {
public:
	MarioWalkState(Entity* mario, int dir) : MarioMovementState(mario, dir) {};
	State* HandleInput(KBState::KeyboardStateTracker* kbState) override;
	void Update(float dt) override;
	void Enter() override;
};

class MarioRunState : public MarioMovementState {
public:
	MarioRunState(Entity* mario, int dir) : MarioMovementState(mario, dir) {};
	State* HandleInput(KBState::KeyboardStateTracker* kbState) override;
	void Update(float dt) override;
	void Enter() override;
};

class MarioSkidState : public MarioMovementState {
public:
	MarioSkidState(Entity* mario, int dir) : MarioMovementState(mario, dir) {};
	State* HandleInput(KBState::KeyboardStateTracker* kbState) override;
	void Update(float dt) override;
	void Enter() override;
};

class MarioJumpState : public MarioMovementState {
public:
	MarioJumpState(Entity* mario, int dir) : MarioMovementState(mario, dir) {};
	State* HandleInput(KBState::KeyboardStateTracker* kbState) override;
	void Update(float dt) override;
	void Enter() override;
};	

class MarioSitState : public MarioMovementState {
public:
	MarioSitState(Entity* mario, int dir) : MarioMovementState(mario, dir) {};
	State* HandleInput(KBState::KeyboardStateTracker* kbState) override;
	void Update(float dt) override;
	void Enter() override;
};
