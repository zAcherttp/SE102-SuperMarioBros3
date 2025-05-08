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

constexpr float PLATFORM_COLLISION_TOP_PADDING = 5.f;
constexpr float PUSH_VELOCITY_X = 20.f;
constexpr float PUSH_VELOCITY_Y = 60.f;


Mario::Mario(Vector2 position, int lives, int score, int coins, SpriteSheet* spriteSheet)
	: Entity(position, spriteSheet), m_lives(lives), m_score(score), m_coins(coins)
{
	this->m_powerupSM = nullptr;
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

	m_powerupSM->Update(this, dt);
	m_movementSM->Update(this, dt);
	m_animator->Update(dt, m_vel.x);

	if (m_powerupSM) SetSize(m_powerupSM->GetSize());

	HeadUpDisplay::GetInstance()->UpdatePMeter(
		m_vel.Length(),
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

	points.push_back({ InteractionPointType::TopHead, Vector2(0, -size.y / 2 - 1) });
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
}

void Mario::OnFootCollision(const CollisionResult& result) {
	if (!result.collidedWith) return;

	Vector2 pos = m_collisionComponent->GetPosition();
	Vector2 size = GetSize();
	Vector2 otherPos = result.collidedWith->GetPosition();
	Vector2 otherSize = result.collidedWith->GetSize();

	// Calculate corners since positions are at the center
	Vector2 marioTopLeft = Vector2(pos.x - size.x / 2, pos.y - size.y / 2);
	Vector2 marioBottomRight = Vector2(pos.x + size.x / 2, pos.y + size.y / 2);
	Vector2 otherTopLeft = Vector2(otherPos.x - otherSize.x / 2, otherPos.y - otherSize.y / 2);
	Vector2 otherBottomRight = Vector2(otherPos.x + otherSize.x / 2, otherPos.y + otherSize.y / 2);

	bool fullyInside =
		marioTopLeft.x >= otherTopLeft.x &&
		marioBottomRight.x <= otherBottomRight.x &&
		marioTopLeft.y >= otherTopLeft.y &&
		marioBottomRight.y <= otherBottomRight.y;

	bool pointInside = otherTopLeft.x <= result.pos.x && result.pos.x <= otherBottomRight.x &&
		otherTopLeft.y <= result.pos.y && result.pos.y <= otherBottomRight.y;

	// Check if Mario is landing on top of something
	if (result.normal.y < 0.0f) {
		Block* block = dynamic_cast<Block*>(result.collidedWith);
		if (block && !block->IsSolid() && !fullyInside
			// check if collision is within the vertical bounds of the block
			&& (otherTopLeft.x < result.pos.x && result.pos.x < otherBottomRight.x)
			// check if collision is on top of the block
			&& result.pos.y <= otherTopLeft.y)
		{
			//Log(LOG_INFO, "Collision pos: " + std::to_string(result.pos.x) + ", " + std::to_string(result.pos.y));
			if (m_vel.y > 0) {
				m_vel.y = 0;
			}
		}
		else if (block && block->IsSolid()) {
			m_vel.y = 0;
		}
		else {
			// stomp enemies
		}
	}

	// Handle push up
	// if (pointInside) {
	// 	m_vel.y = -PUSH_VELOCITY_Y;
		//Log(LOG_INFO, "Push up: " + std::to_string(m_vel.y));
		// Check if we're fully out of the top edge
		// if (marioBottomRight.y <= otherTopLeft.y) {
		// 	m_vel.y = 0.0f;
		// 	//clamp at ground to avoid bouncing
		// 	SetPosition(Vector2(GetPosition().x, otherTopLeft.y - size.y / 2));
	// 	}
	// }
}

void Mario::OnTopHeadCollision(const CollisionResult& result) {
	if (!result.collidedWith) return;

	Vector2 pos = m_collisionComponent->GetPosition();
	Vector2 size = GetSize();
	Vector2 otherPos = result.collidedWith->GetPosition();
	Vector2 otherSize = result.collidedWith->GetSize();

	Vector2 marioTopLeft = Vector2(pos.x - size.x / 2, pos.y - size.y / 2);
	Vector2 marioBottomRight = Vector2(pos.x + size.x / 2, pos.y + size.y / 2);
	Vector2 otherTopLeft = Vector2(otherPos.x - otherSize.x / 2, otherPos.y - otherSize.y / 2);
	Vector2 otherBottomRight = Vector2(otherPos.x + otherSize.x / 2, otherPos.y + otherSize.y / 2);

	bool pointInside = otherTopLeft.x <= result.pos.x && result.pos.x <= otherBottomRight.x &&
		otherTopLeft.y <= result.pos.y && result.pos.y <= otherBottomRight.y;

	// Handle push down
	if (pointInside) {
		m_vel.y = PUSH_VELOCITY_Y;
		//Log(LOG_INFO, "Push up: " + std::to_string(m_vel.y));
		// Check if we're fully out of the bottom edge
		if (marioBottomRight.y <= otherTopLeft.y) {
			m_vel.y = 0.0f;
		}
	}
}

void Mario::OnRightSideCollision(const CollisionResult& result) {
	if (!result.collidedWith) return;

	Vector2 pos = m_collisionComponent->GetPosition();
	Vector2 size = GetSize();
	Vector2 otherPos = result.collidedWith->GetPosition();
	Vector2 otherSize = result.collidedWith->GetSize();

	Vector2 marioTopLeft = Vector2(pos.x - size.x / 2, pos.y - size.y / 2);
	Vector2 marioBottomRight = Vector2(pos.x + size.x / 2, pos.y + size.y / 2);
	Vector2 otherTopLeft = Vector2(otherPos.x - otherSize.x / 2, otherPos.y - otherSize.y / 2);
	Vector2 otherBottomRight = Vector2(otherPos.x + otherSize.x / 2, otherPos.y + otherSize.y / 2);

	bool pointInside = otherTopLeft.x <= result.pos.x && result.pos.x <= otherBottomRight.x &&
		otherTopLeft.y <= result.pos.y && result.pos.y <= otherBottomRight.y;

	// Handle push left
	if (pointInside) {
		m_vel.x = -PUSH_VELOCITY_X;
		// Check if we're fully out of the right edge
		if (marioBottomRight.x <= otherTopLeft.x) {
			m_vel.x = 0.0f;
			Log(LOG_INFO, "STOPPED");
			//clamp at edge to stop further moving
			SetPosition(Vector2(otherTopLeft.x - size.x / 2, GetPosition().y));
		}
	}
}

void Mario::OnLeftSideCollision(const CollisionResult& result) {
	if (!result.collidedWith) return;

	Vector2 pos = m_collisionComponent->GetPosition();
	Vector2 size = GetSize();
	Vector2 otherPos = result.collidedWith->GetPosition();
	Vector2 otherSize = result.collidedWith->GetSize();

	Vector2 marioTopLeft = Vector2(pos.x - size.x / 2, pos.y - size.y / 2);
	Vector2 marioBottomRight = Vector2(pos.x + size.x / 2, pos.y + size.y / 2);
	Vector2 otherTopLeft = Vector2(otherPos.x - otherSize.x / 2, otherPos.y - otherSize.y / 2);
	Vector2 otherBottomRight = Vector2(otherPos.x + otherSize.x / 2, otherPos.y + otherSize.y / 2);

	bool pointInside = otherTopLeft.x <= result.pos.x && result.pos.x <= otherBottomRight.x &&
		otherTopLeft.y <= result.pos.y && result.pos.y <= otherBottomRight.y;

	// Handle push right
	if (pointInside) {
		m_vel.x = PUSH_VELOCITY_X;
		//Log(LOG_INFO, "Push right: " + std::to_string(m_vel.x));
		// Check if we're fully out of the left edge


		//TODO: somehow clamp mario pos to entity edge || call this function when no collision happens
		if (marioTopLeft.x >= otherBottomRight.x) {
			m_vel.x = 0.0f;
			SetPosition(Vector2(otherBottomRight.x + size.x / 2, GetPosition().y));
		}
	}
}