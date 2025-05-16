#pragma once
#include "pch.h"
#include "MarioStateBase.h"

using KBState = DirectX::Keyboard::State;
using KBSTracker = DirectX::Keyboard::KeyboardStateTracker;
using namespace DirectX::SimpleMath;

constexpr auto PLAYER_INVINCIBLE_TIME = 2.0f;
constexpr auto PLAYER_DEATH_TIME = 5.0f;
constexpr auto PLAYER_DEATH_BOUNCE_DELAY = 1.0f;
constexpr auto PLAYER_DEATH_BOUNCE_VEL = 180.f;

enum class PowerUp {
	DIE,
	SMALL,
	SUPER,
	RACCOON,
	NONE
};

class MarioPowerUpState : public MarioStateBase {
public:
	MarioPowerUpState(PowerUp lastPowerUp) : m_lastPowerUp(lastPowerUp), MarioStateBase() {
		m_isInvincible = false;
		m_invincibleTimer = 0.0f;
		m_isFlashing = false;
		m_flashingTimer = 0.0f;
		m_stateHealth = 1;
		m_isDying = false;
		m_deathTimer = 0.0f;
		m_switch = false;
		m_animSwitchFlag = false;
		m_takenDmg = false;
	};
	virtual std::unique_ptr<MarioPowerUpState> HandleInput(Mario* mario) = 0;
	void Update(Mario* mario, float dt) override = 0;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;

	virtual void Damage();

	bool IsTransitioning() const;
	bool IsDying() const;

	int m_stateHealth;
	bool m_takenDmg;
	bool m_switch;

	bool m_isInvincible;
	float m_invincibleTimer;

	bool m_isFlashing;
	float m_flashingTimer;

	bool m_isDying;
	float m_deathTimer;

	bool m_animSwitchFlag;

	PowerUp m_lastPowerUp;
};

class MarioDieState : public MarioPowerUpState {
public:
	MarioDieState(PowerUp lastPowerUp = PowerUp::NONE) : MarioPowerUpState(lastPowerUp) {
		m_isDying = true;
		m_deathTimer = 0.f;
		m_velocity = Vector2(0, 0);
	};
	std::unique_ptr<MarioPowerUpState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	void Damage() override;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
private:
	bool m_bounced = false;
	Vector2 m_velocity;
};

class MarioSmallState : public MarioPowerUpState {
public:
	MarioSmallState(PowerUp lastPowerUp = PowerUp::NONE) : MarioPowerUpState(lastPowerUp) {};
	MarioSmallState(PowerUp lastPowerUp, float invincibleTimer, bool isInvincible, float flashingTimer, bool isFlashing) : MarioPowerUpState(lastPowerUp) {
		m_invincibleTimer = invincibleTimer;
		m_isInvincible = isInvincible;
		m_flashingTimer = flashingTimer;
		m_isFlashing = isFlashing;
	};
	std::unique_ptr<MarioPowerUpState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	void Damage() override;
	Vector2 GetStateSizeOffset() const override;
};

class MarioSuperState : public MarioPowerUpState {
public:
	MarioSuperState(PowerUp lastPowerUp = PowerUp::NONE) : MarioPowerUpState(lastPowerUp) {};
	MarioSuperState(PowerUp lastPowerUp, float invincibleTimer, bool isInvincible, float flashingTimer, bool isFlashing) : MarioPowerUpState(lastPowerUp) {
		m_invincibleTimer = invincibleTimer;
		m_isInvincible = isInvincible;
		m_flashingTimer = flashingTimer;
		m_isFlashing = isFlashing;
	};
	std::unique_ptr<MarioPowerUpState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
};

class MarioRaccoonState : public MarioPowerUpState {
public:
	MarioRaccoonState(PowerUp lastPowerUp = PowerUp::NONE) : MarioPowerUpState(lastPowerUp) {};
	MarioRaccoonState(PowerUp lastPowerUp, float invincibleTimer, bool isInvincible, float flashingTimer, bool isFlashing) : MarioPowerUpState(lastPowerUp) {
		m_invincibleTimer = invincibleTimer;
		m_isInvincible = isInvincible;
		m_flashingTimer = flashingTimer;
		m_isFlashing = isFlashing;
	};
	std::unique_ptr<MarioPowerUpState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	Vector2 GetStateSizeOffset() const override;
};