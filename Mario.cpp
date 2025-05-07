#pragma once
#include "pch.h"
#include "Mario.h"
#include "AssetIDs.h"
#include "Debug.h"

using namespace DirectX::SimpleMath;
using Keyboard = DirectX::Keyboard;

#define UP			Keyboard::W
#define DOWN		Keyboard::S
#define LEFT		Keyboard::A
#define RIGHT		Keyboard::D

#define START		Keyboard::I

#define B_BTN		Keyboard::J
#define A_BTN		Keyboard::K

Mario::Mario(Vector2 position, int lives, int score, int coins, SpriteSheet* spriteSheet)
	: Entity(position, spriteSheet), m_lives(lives), m_score(score), m_coins(coins)
{
	this->m_movementSM = nullptr;
	this->m_inputState = new InputState();
}

void Mario::HandleInput(Keyboard::State* kbState, Keyboard::KeyboardStateTracker* kbsTracker) {


	m_inputState->isUpDown = kbState->IsKeyDown(UP);
	m_inputState->isDownDown = kbState->IsKeyDown(DOWN);
	m_inputState->isLeftDown = kbState->IsKeyDown(LEFT);
	m_inputState->isRightDown = kbState->IsKeyDown(RIGHT);
	m_inputState->isADown = kbState->IsKeyDown(A_BTN);
	m_inputState->isBDown = kbState->IsKeyDown(B_BTN);

	m_inputState->isStartPressed = kbsTracker->IsKeyPressed(START);
	m_inputState->isLeftPressed = kbsTracker->IsKeyPressed(LEFT);
	m_inputState->isRightPressed = kbsTracker->IsKeyPressed(RIGHT);
	m_inputState->isUpPressed = kbsTracker->IsKeyPressed(UP);
	m_inputState->isDownPressed = kbsTracker->IsKeyPressed(DOWN);
	m_inputState->isAPressed = kbsTracker->IsKeyPressed(A_BTN);
	m_inputState->isBPressed = kbsTracker->IsKeyPressed(B_BTN);

	if (!m_movementSM) return;

	MarioMovementState* mState = m_movementSM->HandleInput(this);
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

bool Mario::UsesInteractionPoints() const
{
	return true;
}

void Mario::ItsAMe()
{
	// small temporary, powerup states will set these later on
	this->SetSize(Vector2(12, 15));
	this->SetAnimId(ID_ANIM_MARIO_SMALL);
	this->m_movementSM = new MarioIdleState(Direction::Right);
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
		if (m_visible) {
		// round the position to the nearest pixel
		Vector2 pos = m_collisionComponent->GetPosition();
		/*pos.x = static_cast<int>(pos.x + 0.5f);
		pos.y = static_cast<int>(pos.y + 0.5f);*/
		m_animator->Draw(spriteBatch, pos, 0.1f);
	}
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

void Mario::OnCollision(const CollisionEvent& event) {
	if (UsesInteractionPoints()) {
		switch (event.pointType) {
		case InteractionPointType::TopHead:
			OnTopHeadCollision(event.collidedWith, event.normal);
			break;
		case InteractionPointType::LeftFoot:
		case InteractionPointType::RightFoot:
			OnFootCollision(event.collidedWith, event.normal);
			break;
		case InteractionPointType::LeftMiddle:
			OnLeftSideCollision(event.collidedWith, event.normal);
			break;
		case InteractionPointType::RightMiddle:
			OnRightSideCollision(event.collidedWith, event.normal);
			break;
		default:
			break;
		}
	}
}

void Mario::OnNoCollision() {
}

void Mario::OnFootCollision(Entity* other, const Vector2& normal) {
}

void Mario::OnTopHeadCollision(Entity* other, const Vector2& normal) {
	if (other->IsCollidable()) {
		Log(LOG_INFO, "Mario hit the top of " + other->GetAnimId());
	}
}

void Mario::OnRightSideCollision(Entity* other, const Vector2& normal) {
}

void Mario::OnLeftSideCollision(Entity* other, const Vector2& normal) {
}