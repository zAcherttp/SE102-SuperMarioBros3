#pragma once
#include "pch.h"
#include "Mario.h"

Mario::Mario(Vector2 position, int lives, int score, int coins)
	: Entity(position), m_lives(lives), m_score(score), m_coins(coins)
{
	this->m_movementSM = new MarioIdleState(1);
}

void Mario::HandleInput(DirectX::Keyboard::KeyboardStateTracker* kbState) {
	MarioMovementState* mState = m_movementSM->HandleInput(this, kbState);
	/*m_powerupSM->HandleInput(kbState);*/
	if (mState != nullptr) {
		delete m_movementSM;
		m_movementSM = mState;

		m_movementSM->Enter(this);
	}
}

void Mario::Update(float dt)
{
	m_movementSM->Update(this, dt);
}

void Mario::Render() {

}

bool Mario::IsGrounded() const {
	return true;
}