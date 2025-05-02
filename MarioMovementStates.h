#pragma once
#include "pch.h"
#include "Debug.h"
#include "State.h"

class MarioWalkState;
class MarioRunState;
class MarioSkidState;
class MarioJumpState;
class MarioSitState;
class Mario;

using KBState = DirectX::Keyboard::State;
using KBSTracker = DirectX::Keyboard::KeyboardStateTracker;

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
	Racoon
};

enum class Direction { Left = -1, Right = 1 };

class MarioStateBase : protected State<Mario> {
protected:
	Direction m_dir;
public:
	MarioStateBase(Direction dir) : m_dir(dir), State() {};
	Direction GetDirection() const;
	virtual MarioStateBase* HandleInput(Mario* mario) = 0;
	void Update(Mario* mario, float dt) override = 0;
	virtual void Enter(Mario* mario) override;
	virtual void Exit(Mario* mario) override = 0;
	virtual std::string GetStateName() const = 0;
	void SetAnimation(Mario* mario, int animId) const;
};

class MarioPowerupState : public MarioStateBase {
public:
	MarioPowerupState(Direction dir) : MarioStateBase(dir) {};
	virtual MarioPowerupState* HandleInput(Mario* mario) = 0;
	void Update(Mario* mario, float dt) override = 0;
	virtual void Exit(Mario* mario) override = 0;
};

class MarioMovementState : public MarioStateBase {
public:
	MarioMovementState(Direction dir) : MarioStateBase(dir) {};
	virtual MarioMovementState* HandleInput(Mario* mario) override = 0;
	void Update(Mario* mario, float dt) override = 0;
	virtual void Exit(Mario* mario) override = 0;
};

class MarioIdleState : public MarioMovementState {
public:
	MarioIdleState(Direction dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;
	std::string GetStateName() const override;
};

class MarioWalkState : public MarioMovementState {
public:
	MarioWalkState(Direction dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;
	std::string GetStateName() const override;
};

class MarioRunState : public MarioMovementState {
public:
	MarioRunState(Direction dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;
	std::string GetStateName() const override;
};

class MarioSkidState : public MarioMovementState {
public:
	MarioSkidState(Direction dir) : MarioMovementState(dir), m_lastDir(dir) {};
	MarioMovementState* HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;
	std::string GetStateName() const override;
private:
	Direction m_lastDir;
};

class MarioJumpState : public MarioMovementState {
public:
	MarioJumpState(Direction dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;
	std::string GetStateName() const override;
};	

class MarioSitState : public MarioMovementState {
public:
	MarioSitState(Direction dir) : MarioMovementState(dir) {};
	MarioMovementState* HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;
	std::string GetStateName() const override;
};

class MarioStateFactory {
public:
    static MarioMovementState* CreateState(MarioMovementStateType type, Direction direction);
	static MarioPowerupState* CreateState(MarioPowerupStateType type, Direction direction);
};
