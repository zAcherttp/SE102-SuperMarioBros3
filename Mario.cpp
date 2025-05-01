#pragma once
#include "pch.h"
#include "Mario.h"
#include "AssetIDs.h"
#include "Debug.h"

using namespace DirectX::SimpleMath;
using Keyboard = DirectX::Keyboard;

Mario::Mario(Vector2 position, int lives, int score, int coins, SpriteSheet* spriteSheet)
	: Entity(position, spriteSheet), m_lives(lives), m_score(score), m_coins(coins)
{
	this->m_movementSM = nullptr;
}

void Mario::HandleInput(Keyboard::State* kbState, Keyboard::KeyboardStateTracker* kbsTracker) {
	if (!m_movementSM) return;

	MarioMovementState* mState = m_movementSM->HandleInput(this, kbState, kbsTracker);
	/*m_powerupSM->HandleInput(kbState);*/
	if (mState != nullptr) {
		m_movementSM->Exit(this);

		delete m_movementSM;
		m_movementSM = mState;

		m_movementSM->Enter(this);
	}
}

std::vector<std::pair<InteractionPointType, Vector2>> Mario::GetInteractionPoints() const
{
	//TODO: change to this
	//if (m_powerupSM && typeid(*m_powerupSM) == typeid(MarioSuperState)) {
	//	if (m_movementSM && typeid(*m_movementSM) == typeid(MarioSitState)) {
	//		return GetBigMarioSitInteractionPoints();
	//	}
	//	return GetBigMarioInteractionPoints();
	//}

	return GetSmallMarioInteractionPoints();
}

void Mario::ItsAMe()
{
	// small temporary, powerup states will set these later on
	this->SetSize(Vector2(12, 15));
	this->SetAnimId(ID_ANIM_MARIO_SMALL);
	this->m_movementSM = new MarioIdleState(1);
	this->m_movementSM->Enter(this);

	Log(LOG_INFO, "Mario position: " + std::to_string(this->GetPosition().x) + ", " + std::to_string(this->GetPosition().y));
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

std::vector<std::pair<InteractionPointType, Vector2>> Mario::GetSmallMarioInteractionPoints() const
{
	std::vector<std::pair<InteractionPointType, Vector2>> points;
	Vector2 size = GetCollisionComponent()->GetSize();

	points.push_back({ InteractionPointType::TopHead, Vector2(0, -size.y / 2) });
	points.push_back({ InteractionPointType::LeftUpper, Vector2(-size.x / 2, -size.y / 4) });
	points.push_back({ InteractionPointType::LeftLower, Vector2(-size.x / 2, 0) });
	points.push_back({ InteractionPointType::RightUpper, Vector2(size.x / 2, -size.y / 4) });
	points.push_back({ InteractionPointType::RightLower, Vector2(size.x / 2, 0) });
	points.push_back({ InteractionPointType::LeftFoot, Vector2(-size.x / 4, size.y / 2) });
	points.push_back({ InteractionPointType::RightFoot, Vector2(size.x / 4, size.y / 2) });
	
	return points;
}

std::vector<std::pair<InteractionPointType, Vector2>> Mario::GetBigMarioInteractionPoints() const
{
	std::vector<std::pair<InteractionPointType, Vector2>> points;
	Vector2 size = GetCollisionComponent()->GetSize();

	points.push_back({ InteractionPointType::TopHead, Vector2(0, -size.y / 2) });
	points.push_back({ InteractionPointType::LeftUpper, Vector2(-size.x / 2, -size.y / 3) });
	points.push_back({ InteractionPointType::LeftLower, Vector2(-size.x / 2, 0) });
	points.push_back({ InteractionPointType::RightUpper, Vector2(size.x / 2, -size.y / 3) });
	points.push_back({ InteractionPointType::RightLower, Vector2(size.x / 2, 0) });
	points.push_back({ InteractionPointType::LeftFoot, Vector2(-size.x / 4, size.y / 2) });
	points.push_back({ InteractionPointType::RightFoot, Vector2(size.x / 4, size.y / 2) });

	return points;
}

std::vector<std::pair<InteractionPointType, Vector2>> Mario::GetBigMarioSitInteractionPoints() const
{
	return std::vector<std::pair<InteractionPointType, Vector2>>();
}
