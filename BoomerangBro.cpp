#include "pch.h"
#include "AssetIDs.h"
#include "Block.h"
#include "Boomerang.h"
#include "BoomerangBro.h"
#include "Collision.h"
#include "Debug.h"
#include "Game.h"
#include "GameConfig.h"
#include "Mario.h"
#include "World.h"

using namespace GameConstants::Enemies::BoomerangBro;

BoomerangBro::BoomerangBro(Vector2 position, Vector2 size, SpriteSheet* spriteSheet)
	: Enemy(position, size, spriteSheet)  // 2 seconds in firing state
	, m_boomerang1(nullptr)
	, m_boomerang2(nullptr)
	, m_world(nullptr)
	, m_isFlipped(false) // Default: looking left (original sprite direction)
	, m_state(BoomerangBroState::WALKRIGHT)
	, m_isThrowingPhase(false)
	, m_phaseTimer(0.0f)
	, m_waitTimer(0.0f)
	, m_jumpTimer(0.0f)
	, m_isDying(false)
{
	SetupCollisionComponent();
	Log(__FUNCTION__, "Collision component initialized");
	m_isCollidable = true;

	// m_boomerang1 = new Boomerang(position + BULLET_OFFSET, Vector2(BULLET_WIDTH, BULLET_HEIGHT), spriteSheet);	// m_boomerang2 = new Boomerang(position + BULLET_OFFSET + Vector2(BOOMERANG_OFFSET_X, 0.0f), Vector2(BULLET_WIDTH, BULLET_HEIGHT), spriteSheet);
	SetAnimation(ID_ANIM_BOOMERANG_BRO_WALK);
	m_leftX = (int)position.x;
	m_rightX = (int)(position.x + PATROL_DISTANCE);
	m_jumpTime = (float)(rand() % 3 + 5);
	m_mario = dynamic_cast<Mario*>(Game::GetInstance()->GetCurrentWorld()->GetPlayer());
}

void BoomerangBro::Update(float dt)
{
	Vector2 pos = GetPosition();
	Vector2 vel = GetVelocity();

	m_isGrounded = Collision::GetInstance()->GroundCheck(this, dt);
	if (!m_isGrounded) {
		vel.y += GameConstants::Physics::GRAVITY * dt;
		SetVelocity(vel);
	}

	if (m_isDying && m_isActive) {
		m_deathTimer += dt;
		if (m_dyingType == DyingType::STOMPED)
		{
			if (m_deathTimer >= GameConstants::Enemies::DEATH_BONK_ANI_TIME) {
				m_isActive = false;
				m_visible = false;
				return;
			}
			SetPosition(GetPosition() + GetVelocity() * dt);
			Entity::Update(dt);
			return;
		}
	}
	m_phaseTimer += dt;

	if (m_phaseTimer >= PHASE_DURATION) {
		m_isThrowingPhase = !m_isThrowingPhase;
		m_phaseTimer = 0.0f;
		m_isThrowingPhase ? SetAnimation(ID_ANIM_BOOMERANG_BRO_THROW) : SetAnimation(ID_ANIM_BOOMERANG_BRO_WALK);
		m_state = BoomerangBroState::WALKRIGHT;
	}
	if (m_boomerang1)
	{
		m_boomerang1->m_lifeTimer += dt;
		if (m_boomerang1->m_lifeTimer >= LIFETIME) {
			m_boomerang1->Deactivate();
			m_boomerang1 = nullptr;
		}
	}
	if (m_boomerang2)
	{
		m_boomerang2->m_lifeTimer += dt;
		if (m_boomerang2->m_lifeTimer >= LIFETIME) {
			m_boomerang2->Deactivate();
			m_boomerang2 = nullptr;
		}
	}

	m_jumpTimer += dt;
	if (m_jumpTimer >= m_jumpTime) {
		Jump();
		m_jumpTimer = 0.0f;
		m_jumpTime = (float)(rand() % 3 + 5);
	}

	switch (m_state)
	{
	case BoomerangBroState::WALKRIGHT:
		if (pos.x < m_rightX) {
			vel.x = 50.0f;
		}
		if (pos.x >= m_rightX) {
			m_state = BoomerangBroState::WAIT;
			vel.x = 0.0f;
			m_waitTimer = 0.0f;
			pos.x = (float)m_rightX;
		}
		SetVelocity(Vector2(vel.x, GetVelocity().y));
		break;
	case BoomerangBroState::WALKLEFT:
		if (pos.x > m_leftX) {
			vel.x = -50.0f;
		}
		if (pos.x <= m_leftX) {
			m_state = BoomerangBroState::WAIT;
			vel.x = 0.0f;
			m_waitTimer = 0.0f;
			pos.x = (float)m_leftX;
		}
		SetVelocity(Vector2(vel.x, GetVelocity().y));
		break;
	case BoomerangBroState::WAIT:
		m_animator->Pause();
		vel.x = 0.0f;
		m_waitTimer += dt;
		if (m_waitTimer >= 0.3f) {
			m_animator->Play();
			pos.x >= m_rightX ? m_state = BoomerangBroState::WALKLEFT : m_state = BoomerangBroState::WALKRIGHT;
			m_waitTimer = 0.0f;
		}

		SetVelocity(Vector2(vel.x, GetVelocity().y));
		break;
	}

	if (m_boomerang1 && m_boomerang1->GetState() == BoomerangState::RETURN && CheckAABBCollision(this, m_boomerang1)) {
		OnBoomerangCaught(m_boomerang1);
	}
	if (m_boomerang2 && m_boomerang2->GetState() == BoomerangState::RETURN && CheckAABBCollision(this, m_boomerang2)) {
		OnBoomerangCaught(m_boomerang2);
	}


	if (!m_isDying) UpdateBroOrientation(m_mario);


	if (m_isThrowingPhase) {
		if (m_animator->GetCurrentFrame() == 4 || m_animator->GetCurrentFrame() == 5) {
			if (!m_isBommerangCreated) {
				CreateBoomerangs();
				m_isBommerangCreated = true;
				m_isBoomerangThrown = false;
			}
			Hold();
		}
		else if (m_animator->GetCurrentFrame() == 6 || m_animator->GetCurrentFrame() == 7) {
			if (!m_isBoomerangThrown) {
				Throw();
				m_isBommerangCreated = false;
				m_isBoomerangThrown = true;
			}
		}
	}
	else {
		m_isBommerangCreated = false;
		m_isBoomerangThrown = false;
	}

	SetPosition(GetPosition() + GetVelocity() * dt);
	Entity::Update(dt);
}

void BoomerangBro::Render(DirectX::SpriteBatch* spriteBatch)
{
	if (m_visible) {
		Vector2 pos = m_collisionComponent->GetPosition();
		pos.x = (float)(int)(pos.x + 0.5f);
		pos.y = (float)(int)(pos.y + 0.5f);
		m_animator->Draw(spriteBatch, pos, 0.01f);
	}
	if (m_boomerang1) {
		m_boomerang1->Render(spriteBatch);
	}
	if (m_boomerang2) {
		m_boomerang2->Render(spriteBatch);
	}
}

void BoomerangBro::OnCollision(const CollisionResult& event)
{
	Mario* mario = dynamic_cast<Mario*>(event.collidedWith);
	Block* block = dynamic_cast<Block*>(event.collidedWith);


	if (event.contactNormal.y < 0 && block)
	{
		Vector2 vel = GetVelocity();
		vel.y = 0.0f;
		SetVelocity(vel);
		return;
	}

	if (event.contactNormal.y > 0)
	{
		if (mario)
		{
			EffectManager::GetInstance()->CreatePointEffect(GetPosition(), 1000);
			Die(DyingType::STOMPED);
			Vector2 vel = mario->GetVelocity();
			vel.y = GameConstants::Player::BOUNCE_FORCE;
			mario->SetVelocity(vel);
		}
		return;
	}

}

void BoomerangBro::Hold()
{
	if (m_boomerang1 && m_boomerang1->GetState() == BoomerangState::HOLDING) {
		m_boomerang1->UpdatePosition(GetPosition());
	}
	if (m_boomerang2 && m_boomerang2->GetState() == BoomerangState::HOLDING) {
		m_boomerang2->UpdatePosition(GetPosition());
	}
}

void BoomerangBro::Throw()
{
	if (m_boomerang1 && m_boomerang1->GetState() == BoomerangState::HOLDING) {
		Log(LOG_INFO, "BoomerangBro throwing boomerang 1");
		m_boomerang1->SetState(BoomerangState::OUTWARD);
	}
	if (m_boomerang2 && m_boomerang2->GetState() == BoomerangState::HOLDING) {
		Log(LOG_INFO, "BoomerangBro throwing boomerang 2");
		m_boomerang2->SetState(BoomerangState::OUTWARD);
	}
}

void BoomerangBro::Jump()
{
	if (m_isGrounded) {
		Vector2 vel = GetVelocity();
		vel.y = -150.0f;
		SetVelocity(vel);
		m_isGrounded = false;
	}
}

void BoomerangBro::UpdateBroOrientation(Mario* mario)
{

	if (mario == nullptr)
	{
		return;
	}

	Vector2 marioPos = mario->GetPosition();
	Vector2 broPos = GetPosition();

	bool shouldFlip = (marioPos.x > broPos.x);

	if (shouldFlip != m_isFlipped)
	{
		m_isFlipped = shouldFlip;
		this->GetAnimator()->SetFlipHorizontal(m_isFlipped);
	}
}

void BoomerangBro::CreateBoomerangs()
{
	Game* game = Game::GetInstance();
	SpriteSheet* spriteSheet = game->GetSpriteSheet();

	// Always try to create boomerang1 first
	if (!m_boomerang1 && !m_boomerang2)
	{
		m_boomerang1 = new Boomerang(GetPosition(), Vector2(BOOMERANG_WIDTH, BOOMERANG_HEIGHT), spriteSheet, !m_isFlipped);
		Game::GetInstance()->GetCurrentWorld()->AddEntity(m_boomerang1);
		Log(LOG_INFO, "Boomerang1 created");
	}
	// If boomerang1 exists and is active, create boomerang2
	else if (m_boomerang1 && !m_boomerang2)
	{
		m_boomerang2 = new Boomerang(GetPosition(), Vector2(BOOMERANG_WIDTH, BOOMERANG_HEIGHT), spriteSheet, !m_isFlipped);
		Game::GetInstance()->GetCurrentWorld()->AddEntity(m_boomerang2);
		Log(LOG_INFO, "Boomerang2 created while boomerang1 is active");
	}
}

void BoomerangBro::SetupCollisionComponent()
{
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x, curSize.y);
	m_collisionComponent->SetSize(newSize);

	m_collisionComponent->SetPosition(GetPosition() + Vector2(newSize.x / 2, newSize.y / 2));

	m_collisionComponent->SetSize(newSize);
}

void BoomerangBro::OnBoomerangCaught(Boomerang* boomerang)
{

	if (m_boomerang1 == boomerang) {
		m_boomerang1 = nullptr;
		Log(LOG_INFO, "Boomerang1 caught and deactivated");
	}
	if (m_boomerang2 == boomerang) {
		m_boomerang2 = nullptr;
		Log(LOG_INFO, "Boomerang2 caught and deactivated");
	}
}

bool BoomerangBro::CheckAABBCollision(const Entity* entityA, const Entity* entityB)
{
	// Get positions and sizes of both entities
	Vector2 positionA = entityA->GetPosition();
	Vector2 sizeA = entityA->GetSize();

	Vector2 positionB = entityB->GetPosition();
	Vector2 sizeB = entityB->GetSize();

	// Create bounding boxes
	Rectangle boxA = { long(positionA.x), long(positionA.y), long(sizeA.x), long(sizeA.y) };
	Rectangle boxB = { long(positionB.x), long(positionB.y), long(sizeB.x), long(sizeB.y) };

	// Check for intersection
	return (boxA.x < boxB.x + boxB.width &&
		boxA.x + boxA.width > boxB.x &&
		boxA.y < boxB.y + boxB.height &&
		boxA.y + boxA.height > boxB.y);
}

void BoomerangBro::Die(DyingType type)
{
	if (type == DyingType::STOMPED)
	{
		Log(LOG_INFO, "BoomerangBro is dying by being stomped");
		if (!m_isDying) {
			m_dyingType = type;
			m_isDying = true;
			m_isCollidable = false;
			m_deathTimer = 0.0f;
			m_visible = true;
			SetAnimation(ID_ANIM_BOOMERANG_BRO_DIE);
			m_animator->SetFlipVertical(true);
			m_animator->SetFlipHorizontal(m_isFlipped);
			SetVelocity(Vector2(GetVelocity().x * 0.4f, 70.0f));
			Log(LOG_INFO, "Animation:" + std::to_string(m_animator->GetCurrentAnimation()) + " for BoomerangBro: " + std::to_string(GetPosition().x) + ", flipped: " + std::to_string(m_isFlipped));
		}
		return;
	}
}
