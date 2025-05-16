#include "pch.h"
#include "AssetIDs.h"
#include "Mario.h"
#include "MarioPowerUpStates.h"
#include "Effect.h"
#include "EffectManager.h"

std::string MarioDieState::GetStateName() const { return "die"; }
std::string MarioSmallState::GetStateName() const { return "small"; }
std::string MarioSuperState::GetStateName() const { return "super"; }
std::string MarioRaccoonState::GetStateName() const { return "raccoon"; }

int MarioDieState::GetStateAnimValue() const { return ID_ANIM_MARIO_DIE; }
int MarioSmallState::GetStateAnimValue() const { return ID_ANIM_MARIO_SMALL; }
int MarioSuperState::GetStateAnimValue() const { return ID_ANIM_MARIO_SUPER; }
int MarioRaccoonState::GetStateAnimValue() const { return ID_ANIM_MARIO_RACCOON; }

Vector2 MarioDieState::GetStateSizeOffset() const { return Vector2(12, 15); }
Vector2 MarioSmallState::GetStateSizeOffset() const { return Vector2(12, 15); }
Vector2 MarioSuperState::GetStateSizeOffset() const { return Vector2(14, 27); }
Vector2 MarioRaccoonState::GetStateSizeOffset() const { return Vector2(14, 27); }

void MarioPowerUpState::Enter(Mario* mario)
{
	Log(__FUNCTION__, GetStateName() + " HP: " + std::to_string(m_stateHealth));
	mario->SetSize(mario->GetCurrentMStateSizeOffset() + mario->GetCurrentPStateSizeOffset());
	SetAnimation(mario, mario->GetCurrentMStateAnimValue() + mario->GetCurrentPStateAnimValue());
}

void MarioPowerUpState::Exit(Mario* mario)
{
	mario->SetSize(mario->GetCurrentMStateSizeOffset() + mario->GetCurrentPStateSizeOffset());
	SetAnimation(mario, mario->GetCurrentMStateAnimValue() + mario->GetCurrentPStateAnimValue());
}

void MarioPowerUpState::Update(Mario* mario, float dt) {
	// Process invincible state
	if (m_isInvincible) {
		m_invincibleTimer += dt;

		// Handle flashing effect during invincibility
		if (m_isFlashing) {
			m_flashingTimer += dt;

			// Toggle visibility every 1/30 second
			if (m_flashingTimer >= 0.033f) {
				// Toggle visibility
				mario->SetIsVisible(!mario->GetIsVisible());

				// If becoming visible again, alternate between animations
				if (mario->GetIsVisible() && m_takenDmg) {
					m_animSwitchFlag = !m_animSwitchFlag;
					if (m_animSwitchFlag) {
						SetAnimation(mario, ID_ANIM_MARIO_SUPER + ID_ANIM_MARIO_IDLE);
					}
					else {
						SetAnimation(mario, ID_ANIM_MARIO_SUPER_BONKED);
					}
				}

				m_flashingTimer = 0.0f;
			}
		}

		// Handle state switch during the first half of invincible
		// switch immediately if Small -> Die
		// switch after half invincible time if Super -> Small
		// switch after smoke effect done if Raccoon -> Super
		if (m_invincibleTimer > PLAYER_INVINCIBLE_TIME / 2.f) {
			if (m_stateHealth <= 0) m_powerDown = true;
		}

		// End invincibility after the set time
		if (m_invincibleTimer >= PLAYER_INVINCIBLE_TIME) {
			m_isInvincible = false;
			m_isFlashing = false;
			m_takenDmg = false;
			mario->SetIsVisible(true);
			SetAnimation(mario, mario->GetCurrentMStateAnimValue() + mario->GetCurrentPStateAnimValue());
		}
	}
}

void MarioPowerUpState::Damage(Mario* mario)
{
	if (m_isInvincible) return;

	// Start invincibility
	m_isInvincible = true;
	m_invincibleTimer = 0.0f;

	m_stateHealth--;
	m_takenDmg = true;
	Log(__FUNCTION__, GetStateName() + " HP: " + std::to_string(m_stateHealth));

	m_flashingTimer = 0.0f;
	m_isFlashing = true;

	return;
}

void MarioPowerUpState::PowerUp(Mario* mario)
{
	m_isInvincible = true;
	m_invincibleTimer = 0.0f;
	m_takenDmg = false;
	m_flashingTimer = 0.0f;
	m_isFlashing = false;

	if (m_currentPowerUp == PowerUpType::SMALL) {
		Vector2 offset = Vector2(0, 27.f - GetStateSizeOffset().y) / 2.f;
		mario->SetPosition(mario->GetPosition() - offset);
	}
}

void MarioDieState::Enter(Mario* mario)
{
	mario->SetVelocity(m_velocity);
	mario->SetIsCollidable(false);
	mario->SetSize(mario->GetCurrentPStateSizeOffset());
	SetAnimation(mario, mario->GetCurrentPStateAnimValue());
}

void MarioDieState::Exit(Mario* mario)
{
	mario;
}

std::unique_ptr<MarioPowerUpState> MarioDieState::HandleInput(Mario* mario)
{
	// do absolutely nothing
	mario;
	return nullptr;
}

void MarioDieState::Damage(Mario* mario) {
	mario;
	return;
}

void MarioDieState::Update(Mario* mario, float dt)
{
	m_deathTimer += dt;

	if (m_deathTimer > PLAYER_DEATH_BOUNCE_DELAY && !m_bounced) {
		m_bounced = true;
		m_velocity = Vector2(0, -PLAYER_DEATH_BOUNCE_VEL);
	}
	if (m_bounced) {
		m_velocity.y += 225.f * dt;
	}
	if (m_deathTimer > PLAYER_DEATH_TIME) {
		Game::GetInstance()->RestartWorld();
	}

	mario->SetPosition(mario->GetPosition() + m_velocity * dt);
}

void MarioSmallState::Damage(Mario* mario)
{
	mario;
	if (m_isInvincible) return;

	m_stateHealth--;

	return;
}

std::unique_ptr<MarioPowerUpState> MarioSmallState::HandleInput(Mario* mario)
{
	mario;
	if (m_powerDown) {
		return std::make_unique<MarioDieState>(PowerUpType::SMALL);
	}
	if (m_powerUp) {
		return std::make_unique<MarioSuperState>(m_currentPowerUp, m_invincibleTimer, m_isInvincible, m_flashingTimer, m_isFlashing, m_takenDmg);
	}
	return nullptr;
}

void MarioSmallState::Update(Mario* mario, float dt)
{
	// Process invincible state
	if (m_stateHealth <= 0) {
		m_powerDown = true;
		return;
	}
	if (m_isInvincible) {
		m_invincibleTimer += dt;

		m_flashingTimer += dt;

		// Toggle visibility every 1/15cal second
		if (m_flashingTimer >= 0.067f) {
			// Toggle visibility
			if (m_takenDmg) {
				mario->SetIsVisible(!mario->GetIsVisible());
			}
			else {
				m_isFlashing = !m_isFlashing;
				if (m_isFlashing) {
					m_animSwitchFlag = !m_animSwitchFlag;
					if (m_animSwitchFlag) {
						SetAnimation(mario, ID_ANIM_MARIO_SUPER + ID_ANIM_MARIO_IDLE);
					}
					else {
						SetAnimation(mario, ID_ANIM_MARIO_SUPER_BONKED);
					}
				}
			}


			m_flashingTimer = 0.0f;
		}

		// Handle state switch during the first half of invincible
		// switch immediately if Small -> Die
		// switch after half invincible time if Super -> Small
		// switch after smoke effect done if Raccoon -> Super
		if (m_invincibleTimer < PLAYER_INVINCIBLE_TIME / 2.f) {
			m_powerUp = true;
			m_isFlashing = false;
		}

		// End invincibility after the set time
		if (m_invincibleTimer >= PLAYER_INVINCIBLE_TIME) {
			m_isInvincible = false;
			m_isFlashing = false;
			m_takenDmg = false;
			mario->SetIsVisible(true);
			SetAnimation(mario, mario->GetCurrentMStateAnimValue() + mario->GetCurrentPStateAnimValue());
		}
	}
}

std::unique_ptr<MarioPowerUpState> MarioSuperState::HandleInput(Mario* mario)
{
	mario;
	if (m_powerDown) {
		return std::make_unique<MarioSmallState>(m_currentPowerUp, m_invincibleTimer, m_isInvincible, m_flashingTimer, m_isFlashing, m_takenDmg);
	}
	if (m_powerUp) {
		return std::make_unique<MarioRaccoonState>(m_currentPowerUp, m_invincibleTimer, m_isInvincible, m_flashingTimer, m_isFlashing, m_takenDmg);
	}
	return nullptr;
}

void MarioSuperState::PowerUp(Mario* mario) {
	m_isInvincible = true;
	m_invincibleTimer = 0.0f;
	m_takenDmg = false;
	m_flashingTimer = 0.0f;
	m_isFlashing = false;

	//TODO:
	//EffectManager::GetInstance()->CreateEffect(mario->GetPosition(), Vector2(16, 16), EffectType::SMOKE);
	mario->SetIsVisible(false);

	if (m_currentPowerUp == PowerUpType::SMALL) {
		Vector2 offset = Vector2(0, 27.f - GetStateSizeOffset().y) / 2.f;
		mario->SetPosition(mario->GetPosition() - offset);
	}
}

void MarioSuperState::Update(Mario* mario, float dt)
{
	// Process invincible state
	if (m_isInvincible) {
		m_invincibleTimer += dt;

		// Handle flashing effect during invincibility
		if (m_isFlashing) {
			m_flashingTimer += dt;

			// Toggle visibility every 1/30 second
			if (m_flashingTimer >= 0.033f) {
				// Toggle visibility
				mario->SetIsVisible(!mario->GetIsVisible());

				// If becoming visible again, alternate between animations
				if (mario->GetIsVisible() && m_takenDmg && m_stateHealth <= 0) {
					m_animSwitchFlag = !m_animSwitchFlag;
					if (m_animSwitchFlag) {
						SetAnimation(mario, ID_ANIM_MARIO_SUPER + ID_ANIM_MARIO_IDLE);
					}
					else {
						SetAnimation(mario, ID_ANIM_MARIO_SUPER_BONKED);
					}
				}

				m_flashingTimer = 0.0f;
			}
		}
		else if (!m_takenDmg) {
			m_smokeTimer += dt;

			if (m_smokeTimer > 0.5f) {
				m_powerUp = true;
			}
		}

		// Handle state switch during the first half of invincible
		// switch immediately if Small -> Die
		// switch after half invincible time if Super -> Small
		// switch after smoke effect done if Raccoon -> Super
		if (m_invincibleTimer > PLAYER_INVINCIBLE_TIME / 2.f) {
			if (m_stateHealth <= 0) m_powerDown = true;
		}

		// End invincibility after the set time
		if (m_invincibleTimer >= PLAYER_INVINCIBLE_TIME) {
			m_isInvincible = false;
			m_isFlashing = false;
			m_takenDmg = false;
			mario->SetIsVisible(true);
			SetAnimation(mario, mario->GetCurrentMStateAnimValue() + mario->GetCurrentPStateAnimValue());
		}
	}
}

std::unique_ptr<MarioPowerUpState> MarioRaccoonState::HandleInput(Mario* mario)
{
	if (m_powerDown) {
		mario->SetIsVisible(true);
		m_isFlashing = true;
		return std::make_unique<MarioSuperState>(m_currentPowerUp, m_invincibleTimer, m_isInvincible, m_flashingTimer, m_isFlashing, m_takenDmg);
	}
	return nullptr;
}

void MarioRaccoonState::Damage(Mario* mario) {
	if (m_isInvincible) return;

	// Start invincibility

	mario->SetIsVisible(false);
	m_isInvincible = true;
	m_invincibleTimer = 0.0f;

	m_stateHealth--;
	m_takenDmg = true;

	m_flashingTimer = 0.0f;
	m_isFlashing = false;

	EffectManager::GetInstance()->CreateEffect(mario->GetPosition(), Vector2(16, 16), EffectType::BONK);

	return;
}

void MarioRaccoonState::Update(Mario* mario, float dt)
{
	// Process invincible state
	if (m_isInvincible) {
		m_invincibleTimer += dt;
		m_smokeTimer += dt;

		if (m_smokeTimer > 0.5f) {
			if (m_stateHealth <= 0) m_powerDown = true;
		}
	}
}

PowerUpType MarioPowerUpState::GetCurrentPowerUp() const
{
	return m_currentPowerUp;
}

bool MarioPowerUpState::IsTransitioning() const {
	return m_isInvincible && m_invincibleTimer < PLAYER_INVINCIBLE_TIME / 2.f;
}

bool MarioPowerUpState::IsDying() const
{
	return m_isDying;
}
