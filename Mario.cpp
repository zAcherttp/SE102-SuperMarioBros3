#pragma once
#include "pch.h"
#include "AssetIDs.h"
#include "Block.h"
#include "Collision.h"
#include "CollisionComponent.h"
#include "Debug.h"
#include "Entity.h"
#include "HeadUpDisplay.h"
#include "Keyboard.h"
#include "Mario.h"
#include "MarioMovementStates.h"
#include "MarioPowerUpStates.h"
#include "MarioStateBase.h"
#include "RedTroopas.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteSheet.h"
#include <cstdlib>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace DirectX::SimpleMath;
using Keyboard = DirectX::Keyboard;

//#define UP			Keyboard::W
//#define DOWN		Keyboard::S
//#define LEFT		Keyboard::A
//#define RIGHT		Keyboard::D
//
//#define START		Keyboard::I
//
//#define B_BTN		Keyboard::J
//#define A_BTN		Keyboard::K

#define UP			Keyboard::Up
#define DOWN		Keyboard::Down
#define LEFT		Keyboard::Left
#define RIGHT		Keyboard::Right

#define START		Keyboard::I

#define B_BTN		Keyboard::A
#define A_BTN		Keyboard::S

constexpr auto PLAYER_HOLD_THROW_SPEED = 4.0f * 60.0f;

Mario::Mario(Vector2 position, int lives, int score, int coins, SpriteSheet* spriteSheet)
	: Entity(position, spriteSheet), m_lives(lives), m_score(score), m_coins(coins)
{
	m_powerupSM = nullptr;
	m_movementSM = nullptr;
	m_inputState = new InputState();
}

void Mario::HandleInput(Keyboard::State* kbState, Keyboard::KeyboardStateTracker* kbsTracker) {

	m_inputState->isStartPressed = kbsTracker->IsKeyPressed(START);

	if (m_collisionComponent->GetIsPushed()) {
		m_inputState->ResetDirectionals();
	}
	else {
		m_inputState->isUpDown = kbState->IsKeyDown(UP);
		m_inputState->isDownDown = kbState->IsKeyDown(DOWN);
		m_inputState->isLeftDown = kbState->IsKeyDown(LEFT);
		m_inputState->isRightDown = kbState->IsKeyDown(RIGHT);
		m_inputState->isUpPressed = kbsTracker->IsKeyPressed(UP);
		m_inputState->isDownPressed = kbsTracker->IsKeyPressed(DOWN);
		m_inputState->isLeftPressed = kbsTracker->IsKeyPressed(LEFT);
		m_inputState->isRightPressed = kbsTracker->IsKeyPressed(RIGHT);
	}

	m_inputState->isADown = kbState->IsKeyDown(A_BTN);
	m_inputState->isBDown = kbState->IsKeyDown(B_BTN);
	m_inputState->isAPressed = kbsTracker->IsKeyPressed(A_BTN);
	m_inputState->isBPressed = kbsTracker->IsKeyPressed(B_BTN);

	if (!m_movementSM || !m_powerupSM || IsTransitioning()) return;

	auto mPState = m_powerupSM->HandleInput(this);
	auto mMState = m_movementSM->HandleInput(this);

	if (mPState) {
		m_powerupSM->Exit(this);
		m_powerupSM = std::move(mPState);
		m_powerupSM->Enter(this);
	}

	if (m_powerupSM->GetStateAnimValue() == ID_ANIM_MARIO_DIE) {
		m_movementSM = std::move(std::make_unique<MarioDieMState>());
		return;
	}

	if (mMState) {
		m_movementSM->Exit(this);
		m_movementSM = std::move(mMState);
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

	m_powerupSM = std::make_unique<MarioSmallState>();
	m_movementSM = std::make_unique<MarioIdleState>(Direction::Right);
	m_powerupSM->Enter(this);
	m_movementSM->Enter(this);
	SetDirection(1);

	Log(LOG_INFO, "Mario position: " + std::to_string(GetPosition().x) + ", " + std::to_string(GetPosition().y));
	Log(LOG_INFO, "It's A Me, Mario");
}

void Mario::Damage() {
	if (m_powerupSM) m_powerupSM->Damage(this);
}

void Mario::PowerUp(PowerUpType type)
{
	PowerUpType currentType = m_powerupSM->GetCurrentPowerUp();
	if (currentType == type || IsTransitioning()) {
		return;
	}
	switch (type) {
	case PowerUpType::SUPER:
		if (currentType > type) return;
		m_powerupSM->PowerUp(this);
		break;
	case PowerUpType::RACCOON:
		m_powerupSM->PowerUp(this);
		break;
	}
}

bool Mario::Kick(Direction dir, Entity* ent)
{
	if (m_movementSM->GetStateName() == "hold") {
		return false;
	}

	m_movementSM->Exit(this);
	m_movementSM = std::move(std::make_unique<MarioKickState>(dir, ent));
	m_movementSM->Enter(this);

	return true;
}

void Mario::Update(float dt)
{
	if (!m_movementSM || !m_powerupSM) return;

	m_isGrounded = Collision::GetInstance()->GroundCheck(this, dt);

	m_collisionComponent->Update(dt);
	m_powerupSM->Update(this, dt);

	if (m_powerupSM->IsDying() || IsTransitioning()) return;

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

PowerUpType Mario::GetPowerUpState() const
{
	return m_powerupSM->GetCurrentPowerUp();
}

bool Mario::IsTransitioning() const
{
	return m_powerupSM->IsTransitioning();
}

bool Mario::IsDying() const
{
	return m_powerupSM->IsDying();
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
	if (axis == Axis::X) {
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
			if (result.contactNormal.x > 0)
				pushDistance -= (pos.x - otherPos.x);
			if (result.contactNormal.x < 0)
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
	//TODO: change to universal Troopa class
	RedTroopas* shell = dynamic_cast<RedTroopas*>(result.collidedWith);
	if (result.contactNormal.x < 0) {
		if (block && block->IsSolid())
		{
			Vector2 vel = GetVelocity();
			vel += result.contactNormal * Vector2(std::abs(vel.x), std::abs(vel.y)) * (1.0f - result.contactTime);
			SetVelocity(vel);
		}
		else if (m_inputState->isBDown && m_movementSM->GetStateName() != "hold" && shell && shell->GetState() == TroopaState::SHELL_IDLE) {
			m_movementSM->Exit(this);
			auto state = std::make_unique<MarioHoldState>(m_movementSM->GetDirection(), result.collidedWith);
			m_movementSM = std::move(state);
			m_movementSM->Enter(this);
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
	RedTroopas* shell = dynamic_cast<RedTroopas*>(result.collidedWith);
	if (result.contactNormal.x > 0) {
		if (block && block->IsSolid())
		{
			Vector2 vel = GetVelocity();
			vel += result.contactNormal * Vector2(std::abs(vel.x), std::abs(vel.y)) * (1.0f - result.contactTime);
			SetVelocity(vel);
		}
		else if (m_inputState->isBDown && m_movementSM->GetStateName() != "hold" && shell && shell->GetState() == TroopaState::SHELL_IDLE) {
			m_movementSM->Exit(this);
			auto state = std::make_unique<MarioHoldState>(m_movementSM->GetDirection(), result.collidedWith);
			m_movementSM = std::move(state);
			m_movementSM->Enter(this);
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