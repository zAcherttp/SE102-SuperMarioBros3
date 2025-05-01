#pragma once
#include "pch.h"
#include "State.h"

class MarioWalkState;
class MarioRunState;
class MarioSkidState;
class MarioJumpState;
class MarioSitState;
class Mario;

using KBState = DirectX::Keyboard::State;
using KBSTracker = DirectX::Keyboard::KeyboardStateTracker;

class MarioStateBase : protected State<Mario> {
protected:
	int m_dir;
public:
	MarioStateBase(int dir) : m_dir(dir), State() {};
	int GetDirection() const;
	virtual MarioStateBase* HandleInput(Mario* mario, KBState* kbState, KBSTracker* kbsTracker) = 0;
};


class MarioMovementState : protected MarioStateBase {
public:
	MarioMovementState(int dir) : MarioStateBase(dir) {};
	virtual MarioMovementState* HandleInput(Mario* mario, KBState* kbState, KBSTracker* kbsTracker) override = 0;
	void Update(Mario* mario, float dt) override = 0;
	virtual void Enter(Mario* mario) override = 0;
	virtual void Exit(Mario* mario) override = 0;
};


class MarioIdleState : public MarioMovementState {
public:
	MarioIdleState(int dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario, KBState* kbState, KBSTracker* kbsTracker) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;
};

class MarioWalkState : public MarioMovementState {
public:
	MarioWalkState(int dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario, KBState* kbState, KBSTracker* kbsTracker) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;
private:
	bool m_isLeftPressed = true;
	bool m_isRightPressed = false;
};

class MarioRunState : public MarioMovementState {
public:
	MarioRunState(int dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario, KBState* kbState, KBSTracker* kbsTracker) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;
};

class MarioSkidState : public MarioMovementState {
public:
	MarioSkidState(int dir) : MarioMovementState(dir), m_lastDir(dir) {};
	MarioMovementState* HandleInput(Mario* mario, KBState* kbState, KBSTracker* kbsTracker) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;
private:
	int m_lastDir;
};

class MarioJumpState : public MarioMovementState {
public:
	MarioJumpState(int dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario, KBState* kbState, KBSTracker* kbsTracker) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;
};	

class MarioSitState : public MarioMovementState {
public:
	MarioSitState(int dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario, KBState* kbState, KBSTracker* kbsTracker) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;
};
