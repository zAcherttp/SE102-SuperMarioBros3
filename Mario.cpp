#pragma once
#include "pch.h"
#include "Mario.h"
#include "AssetIDs.h"
#include "Debug.h"

using namespace DirectX::SimpleMath;

Mario::Mario(Vector2 position, int lives, int score, int coins, SpriteSheet* spriteSheet)
	: Entity(position, spriteSheet), m_lives(lives), m_score(score), m_coins(coins)
{
	this->m_movementSM = nullptr;
}

void Mario::HandleInput(DirectX::Keyboard::KeyboardStateTracker* kbState) {
	if (!m_movementSM) return;

	MarioMovementState* mState = m_movementSM->HandleInput(this, kbState);
	/*m_powerupSM->HandleInput(kbState);*/
	if (mState != nullptr) {
		m_movementSM->Exit(this);

		delete m_movementSM;
		m_movementSM = mState;

		m_movementSM->Enter(this);
	}
}

void Mario::ItsAMe()
{
	// small temporary, powerup states will set these later on
	this->SetSize(Vector2(16, 16));
	this->SetAnimId(ID_ANIM_MARIO_SMALL);
	this->m_movementSM = new MarioIdleState(1);
	this->m_movementSM->Enter(this);

	Log(LOG_INFO, "It's A Me, Mario");
}

void Mario::Update(float dt)
{
	if (!m_movementSM) return;

	m_movementSM->Update(this, dt);
	m_animator->Update(dt, m_vel.x);
}

void Mario::Render(DirectX::SpriteBatch* spriteBatch) {
	Entity::Render(spriteBatch);
}

bool Mario::IsGrounded() const {
	return true;
}