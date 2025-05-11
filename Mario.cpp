	#pragma once
#include "pch.h"
#include "Mario.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "Block.h"

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
	this->m_powerupSM = nullptr;
	this->m_movementSM = nullptr;
	this->m_inputState = new InputState();
}

void Mario::HandleInput(Keyboard::State* kbState, Keyboard::KeyboardStateTracker* kbsTracker) {

	if (!m_collisionComponent->GetIsPushed()) {
		m_inputState->isUpDown = kbState->IsKeyDown(UP);
		m_inputState->isDownDown = kbState->IsKeyDown(DOWN);
		m_inputState->isLeftDown = kbState->IsKeyDown(LEFT);
		m_inputState->isRightDown = kbState->IsKeyDown(RIGHT);
		m_inputState->isUpPressed = kbsTracker->IsKeyPressed(UP);
		m_inputState->isDownPressed = kbsTracker->IsKeyPressed(DOWN);
		m_inputState->isLeftPressed = kbsTracker->IsKeyPressed(LEFT);
		m_inputState->isRightPressed = kbsTracker->IsKeyPressed(RIGHT);
	}
	else {
		m_inputState->ResetDirectionals();
	}
	m_inputState->isADown = kbState->IsKeyDown(A_BTN);
	m_inputState->isBDown = kbState->IsKeyDown(B_BTN);

	m_inputState->isStartPressed = kbsTracker->IsKeyPressed(START);

	m_inputState->isAPressed = kbsTracker->IsKeyPressed(A_BTN);
	m_inputState->isBPressed = kbsTracker->IsKeyPressed(B_BTN);

	if (!m_movementSM) return;

	MarioPowerUpState* mPState = m_powerupSM->HandleInput(this);
	MarioMovementState* mMState = m_movementSM->HandleInput(this);

	if (mPState != nullptr) {
		m_powerupSM->Exit(this);

		delete m_powerupSM;
		m_powerupSM = mPState;

		m_powerupSM->Enter(this);
	}

	if (mMState != nullptr) {
		m_movementSM->Exit(this);

		delete m_movementSM;
		m_movementSM = mMState;

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
	// set default for now, later game class will have mario factory
	
	this->m_powerupSM = new MarioSmallState();
	this->m_movementSM = new MarioIdleState(Direction::Right);
	this->m_powerupSM->Enter(this);
	this->m_movementSM->Enter(this);

	Log(LOG_INFO, "Mario position: " + std::to_string(this->GetPosition().x) + ", " + std::to_string(this->GetPosition().y));
	Log(LOG_INFO, "It's A Me, Mario");
}

void Mario::Update(float dt)
{
	if (!m_movementSM || !m_powerupSM) return;

	m_isGrounded = Collision::GetInstance()->GroundCheck(this, dt);

	m_collisionComponent->Update(dt);

	m_powerupSM->Update(this, dt);
	m_movementSM->Update(this, dt);
	m_animator->Update(dt, m_collisionComponent->GetVelocity().x);

	HeadUpDisplay::GetInstance()->UpdatePMeter(
		m_collisionComponent->GetVelocity().Length(),
		IsGrounded(),
		m_inputState->isBDown,
		m_inputState->isLeftDown || m_inputState->isRightDown);
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
	points.push_back({ InteractionPointType::LeftLower, Vector2(-size.x / 2, size.y / 4) });
	points.push_back({ InteractionPointType::RightUpper, Vector2(size.x / 2, -size.y / 4) });
	points.push_back({ InteractionPointType::RightLower, Vector2(size.x / 2, size.y / 4) });
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

void Mario::OnCollision(const CollisionResult& result) {
	/*if (!result.collidedWith) return;

	if (result.contactNormal.Dot(Vector2(0, 1)) == 0) {
		SetVelocity(Vector2(0, GetVelocity().y));
	}
	if(result.contactNormal.Dot(Vector2(1, 0)) == 0) {
		SetVelocity(Vector2(GetVelocity().x, 0));
	}*/
	result;
}

void Mario::OnNoCollision(float dt, Axis axis) {
	Vector2 vel = GetVelocity();
	if(axis == Axis::X) {
		SetPosition(GetPosition() + Vector2(vel.x * dt, 0));
	}
	else {
		SetPosition(GetPosition() + Vector2(0, vel.y * dt));
	}
}

void Mario::OnFootCollision(const CollisionResult& result) {
	if (!result.collidedWith) return;

	Block* block = dynamic_cast<Block*>(result.collidedWith);
	if (result.contactNormal.y < 0) {
		if (block)
		{
			Vector2 vel = GetVelocity();
			vel += result.contactNormal * Vector2(std::abs(vel.x), std::abs(vel.y)) * (1.0f - result.contactTime);
			SetVelocity(vel);
		}
	}
	else if (result.contactNormal.x != 0) {
		if (block && block->IsSolid())
		{
			Vector2 otherSize = result.collidedWith->GetSize();
			Vector2 otherPos = result.collidedWith->GetPosition();
			Vector2 pos = GetPosition();
			Vector2 size = GetSize();
			float pushDistance = (size.x + otherSize.x) / 2;
			if(result.contactNormal.x > 0)
				pushDistance -= (pos.x - otherPos.x);
			if(result.contactNormal.x < 0)
				pushDistance -= (otherPos.x - pos.x);

			m_collisionComponent->Push(Vector2(pushDistance, 0), 0.1);
		}
	}
}

void Mario::OnTopHeadCollision(const CollisionResult& result) {
	if (!result.collidedWith) return;

	Block* block = dynamic_cast<Block*>(result.collidedWith);
	if (result.contactNormal.y > 0) {
		if (block && block->IsSolid())
		{
			// resolve velocity
			Vector2 vel = GetVelocity();
			vel += result.contactNormal * Vector2(std::abs(vel.x), std::abs(vel.y)) * (1.0f - result.contactTime);
			SetVelocity(vel);

			block->Bump();
		}
	}
	else if (result.contactNormal.x != 0) {
		if (block && block->IsSolid())
		{
			Vector2 otherSize = result.collidedWith->GetSize();
			Vector2 otherPos = result.collidedWith->GetPosition();
			Vector2 pos = GetPosition();
			Vector2 size = GetSize();
			float pushDistance = 0.f;
			if (result.contactNormal.x > 0)
				pushDistance = (size.x + otherSize.x) / 2 - (pos.x - otherPos.x);
			if (result.contactNormal.x < 0)
				pushDistance = (otherPos.x - pos.x) - (size.x + otherSize.x) / 2;

			m_collisionComponent->Push(Vector2(pushDistance, 0), 0.1);
		}
	}
}

void Mario::OnRightSideCollision(const CollisionResult& result) {
	if (!result.collidedWith) return;

	Block* block = dynamic_cast<Block*>(result.collidedWith);
	if (result.contactNormal.x < 0) {
		if (block && block->IsSolid())
		{
			Vector2 vel = GetVelocity();
			vel += result.contactNormal * Vector2(std::abs(vel.x), std::abs(vel.y)) * (1.0f - result.contactTime);
			SetVelocity(vel);
		}
		else {

		}
	}
	else if (result.contactNormal.y != 0) {
		if (block && block->IsSolid())
		{
			Vector2 otherSize = result.collidedWith->GetSize();
			Vector2 otherPos = result.collidedWith->GetPosition();
			
			float pushDistance = otherPos.x - otherSize.x / 2 - result.contactPoint.x;

			m_collisionComponent->Push(Vector2(pushDistance, 0), 0.1);
		}
	}
}

void Mario::OnLeftSideCollision(const CollisionResult& result) {
	if (!result.collidedWith) return;

	Block* block = dynamic_cast<Block*>(result.collidedWith);
	if (result.contactNormal.x > 0) {
		if (block && block->IsSolid())
		{
			Vector2 vel = GetVelocity();
			vel += result.contactNormal * Vector2(std::abs(vel.x), std::abs(vel.y)) * (1.0f - result.contactTime);
			SetVelocity(vel);
		}
		else {

		}
	}
	else if (result.contactNormal.y != 0) {
		if (block && block->IsSolid())
		{
			Vector2 otherSize = result.collidedWith->GetSize();
			Vector2 otherPos = result.collidedWith->GetPosition();

			float pushDistance = otherPos.x + otherSize.x / 2 - result.contactPoint.x;

			m_collisionComponent->Push(Vector2(pushDistance, 0), 0.1);
		}
	}
}