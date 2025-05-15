#pragma once
#include "pch.h"
#include "MarioStateBase.h"

using KBState = DirectX::Keyboard::State;
using KBSTracker = DirectX::Keyboard::KeyboardStateTracker;
using namespace DirectX::SimpleMath;

class MarioPowerUpState : public MarioStateBase {
public:
	MarioPowerUpState() : MarioStateBase() {};
	virtual std::unique_ptr<MarioPowerUpState> HandleInput(Mario* mario) = 0;
	void Update(Mario* mario, float dt) override = 0;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;
};

class MarioSmallState : public MarioPowerUpState {
public:
	MarioSmallState() : MarioPowerUpState() {};
	std::unique_ptr<MarioPowerUpState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
};

class MarioSuperState : public MarioPowerUpState {
public:
	MarioSuperState() : MarioPowerUpState() {};
	std::unique_ptr<MarioPowerUpState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
};

class MarioRaccoonState : public MarioPowerUpState {
public:
	MarioRaccoonState() : MarioPowerUpState() {};
	std::unique_ptr<MarioPowerUpState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
};