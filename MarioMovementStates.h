#pragma once
#include "pch.h"
#include "Debug.h"
#include "MarioStateBase.h"
#include "Troopa.h"
#include <memory>
#include <string>

class Entity;

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
	virtual std::unique_ptr<MarioMovementState> HandleInput(Mario* mario) = 0;
	void Update(Mario* mario, float dt) override = 0;
	virtual void Enter(Mario* mario) override;
	virtual void Exit(Mario* mario) override;
};

class MarioDieMState : public MarioMovementState {
public:
	MarioDieMState(Direction dir = Direction::Left) : MarioMovementState(dir) {};
	std::unique_ptr<MarioMovementState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	void Enter(Mario* mario) override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
};

class MarioIdleState : public MarioMovementState {
public:
	MarioIdleState(Direction dir) : MarioMovementState(dir) {};
	std::unique_ptr<MarioMovementState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
};

class MarioWalkState : public MarioMovementState {
public:
	MarioWalkState(Direction dir) : MarioMovementState(dir) {};
	std::unique_ptr<MarioMovementState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
};

class MarioRunState : public MarioMovementState {
public:
	MarioRunState(Direction dir) : MarioMovementState(dir) {};
	std::unique_ptr<MarioMovementState> HandleInput(Mario* mario) override;
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
	std::unique_ptr<MarioMovementState> HandleInput(Mario* mario) override;
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
	std::unique_ptr<MarioMovementState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;

private:
	bool m_isFalling = false;
};

class MarioSitState : public MarioMovementState {
public:
	MarioSitState(Direction dir) : MarioMovementState(dir) {};
	std::unique_ptr<MarioMovementState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
};

class MarioHoldState : public MarioMovementState {
public:
	MarioHoldState(Direction dir, Entity* heldEntity);
	std::unique_ptr<MarioMovementState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
	Vector2 GetHeldEntityOffset(Mario* mario, const int& state);
	void CheckHeldShellCollisions(Mario* mario, Troopa* shell, float dt);

private:
	Entity* m_heldEntity;

	bool m_isJumping = false;
	const float m_jumpDebounce = 0.2f;
	const float m_dirChangeTimeSpan = 0.3f;

	float m_jumpTimer;
	float m_dirChangeState;
	int m_dirState = -1;

	bool m_isWalking = false;

	bool m_isInterrupted = false;

	Direction m_newDir;
	Vector2 m_heldEntityOffset;
};

class MarioKickState : public MarioMovementState {
public:
	MarioKickState(Direction dir, Entity* entity)
		: MarioMovementState(dir), m_entity(entity) {
	};
	std::unique_ptr<MarioMovementState> HandleInput(Mario* mario) override;
	void Enter(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;

private:
	Entity* m_entity;
	float m_kickTimer = 0.4f;
};

class MarioFlyState : public MarioMovementState {
public:
	MarioFlyState(Direction dir) : MarioMovementState(dir) {};
	std::unique_ptr<MarioMovementState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;

private:
	float m_flyTimer = 0.0f;
	float m_flapTimer = 0.0f;
	bool m_isFlying = true;
};

class MarioHoverState : public MarioMovementState {
public:
	MarioHoverState(Direction dir) : MarioMovementState(dir) {};
	std::unique_ptr<MarioMovementState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;

private:
	float m_hoverTimer = 0.0f;
	bool m_isHovering = false;
};

class MarioSweepState : public MarioMovementState {
public:
	MarioSweepState(Direction dir) : MarioMovementState(dir) {};
	std::unique_ptr<MarioMovementState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	void Enter(Mario* mario) override;
	void Sweep(Mario* mario, float dt) const;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;

private:
	float m_sweepTimer = 0.0f;
	bool m_originalFlipState = false;
};
