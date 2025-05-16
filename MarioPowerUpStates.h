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

enum class PowerUpType {
	DIE,
	SMALL,
	SUPER,
	RACCOON,
	NONE
};

class MarioPowerUpState : public MarioStateBase {
public:
	MarioPowerUpState(PowerUpType lastPowerUp) : m_lastPowerUp(lastPowerUp), MarioStateBase() {
		m_isInvincible = false;
		m_invincibleTimer = 0.0f;
		m_isFlashing = false;
		m_flashingTimer = 0.0f;
		m_stateHealth = 1;
		m_isDying = false;
		m_deathTimer = 0.0f;
		m_powerDown = false;
		m_powerUp = false;
		m_animSwitchFlag = false;
		m_takenDmg = false;
		m_currentPowerUp = PowerUpType::NONE;
	};
	virtual std::unique_ptr<MarioPowerUpState> HandleInput(Mario* mario) = 0;
	void Update(Mario* mario, float dt) override = 0;
	void Enter(Mario* mario) override;
	void Exit(Mario* mario) override;

	virtual void Damage(Mario* mario);
	virtual void PowerUp(Mario* mario);

	PowerUpType GetCurrentPowerUp() const;
	bool IsTransitioning() const;
	bool IsDying() const;

	int m_stateHealth;
	bool m_takenDmg;

	bool m_powerDown;
	bool m_powerUp;

	bool m_isInvincible;
	float m_invincibleTimer;

	bool m_isFlashing;
	float m_flashingTimer;

	bool m_isDying;
	float m_deathTimer;

	bool m_animSwitchFlag;

	PowerUpType m_lastPowerUp;
	PowerUpType m_currentPowerUp;
};

class MarioDieState : public MarioPowerUpState {
public:
	MarioDieState(PowerUpType lastPowerUp = PowerUpType::NONE) : MarioPowerUpState(lastPowerUp) {
		m_isDying = true;
		m_deathTimer = 0.f;
		m_velocity = Vector2(0, 0);
		m_currentPowerUp = PowerUpType::DIE;
	};
	std::unique_ptr<MarioPowerUpState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	void Damage(Mario* mario) override;
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
	MarioSmallState(PowerUpType lastPowerUp = PowerUpType::NONE) : MarioPowerUpState(lastPowerUp) {
		m_currentPowerUp = PowerUpType::SMALL;
	};
	MarioSmallState(PowerUpType lastPowerUp, float invincibleTimer, bool isInvincible, float flashingTimer, bool isFlashing, bool takenDmg) : MarioPowerUpState(lastPowerUp) {
		m_invincibleTimer = invincibleTimer;
		m_isInvincible = isInvincible;
		m_flashingTimer = flashingTimer;
		m_isFlashing = isFlashing;
		m_takenDmg = takenDmg;
		m_currentPowerUp = PowerUpType::SMALL;
	};
	std::unique_ptr<MarioPowerUpState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	void Damage(Mario* mario) override;
	Vector2 GetStateSizeOffset() const override;
};

class MarioSuperState : public MarioPowerUpState {
public:
	MarioSuperState(PowerUpType lastPowerUp = PowerUpType::NONE) : MarioPowerUpState(lastPowerUp) {
		m_currentPowerUp = PowerUpType::SUPER;
		m_toRaccoon = false;
	};
	MarioSuperState(PowerUpType lastPowerUp, float invincibleTimer, bool isInvincible, float flashingTimer, bool isFlashing, bool takenDmg) : MarioPowerUpState(lastPowerUp) {
		m_invincibleTimer = invincibleTimer;
		m_isInvincible = isInvincible;
		m_flashingTimer = flashingTimer;
		m_isFlashing = isFlashing;
		m_takenDmg = takenDmg;
		m_currentPowerUp = PowerUpType::SUPER;
		m_toRaccoon = false;
	};
	std::unique_ptr<MarioPowerUpState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	void PowerUp(Mario* mario) override;
	Vector2 GetStateSizeOffset() const override;
private:
	float m_smokeTimer = 0.f;
	bool m_toRaccoon;
};

class MarioRaccoonState : public MarioPowerUpState {
public:
	MarioRaccoonState(PowerUpType lastPowerUp = PowerUpType::NONE) : MarioPowerUpState(lastPowerUp) {
		m_currentPowerUp = PowerUpType::RACCOON;
	};
	MarioRaccoonState(PowerUpType lastPowerUp, float invincibleTimer, bool isInvincible, float flashingTimer, bool isFlashing, bool takenDmg) : MarioPowerUpState(lastPowerUp) {
		m_invincibleTimer = invincibleTimer;
		m_isInvincible = isInvincible;
		m_flashingTimer = flashingTimer;
		m_isFlashing = isFlashing;
		m_takenDmg = takenDmg;
		m_currentPowerUp = PowerUpType::RACCOON;
	};
	std::unique_ptr<MarioPowerUpState> HandleInput(Mario* mario) override;
	void Update(Mario* mario, float dt) override;
	std::string GetStateName() const override;
	int GetStateAnimValue() const override;
	void Damage(Mario* mario) override;
	//void PowerUp(Mario* mario) override;
	Vector2 GetStateSizeOffset() const override;
private:
	float m_smokeTimer = 0.f;
};