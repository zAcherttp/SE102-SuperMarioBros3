#include "pch.h"
#include "AssetIDs.h"
#include "Block.h"
#include "Debug.h"
#include "GameConfig.h"
#include "Goomba.h"
#include "Mario.h"
#include "ParaGoomba.h"
#include "ScrewBlock.h"
#include "World.h"

using namespace GameConstants;
using namespace Enemies::ParaGoomba;

ParaGoomba::ParaGoomba(Vector2 position, Vector2 size, SpriteSheet* spriteSheet)
	: Enemy(position, size, spriteSheet)
	, m_animTimer(0.0f)
	, m_frameTime(FRAME_TIME)  // Time per frame for animation
	, m_flipFrame(false)
	, m_deathTimer(0.0f)
	, m_isDying(false)
	, m_jumpTimer(0.0f)
	, m_jumpInterval(JUMP_INTERVAL)  // Time between jumps
	, m_isJumping(false)
	, m_jumpCount(0)
	, m_jumpsBeforeBigJump(JUMPS_BEFORE_BIG_JUMP)  // After 3 small jumps, do a big jump
	, m_hasWings(true)  // Start with wings
	, m_currentPhase(0)  // Start with closed wings
	, m_phaseTimer(0.0f)

	, m_closedWingsDuration(CLOSED_WINGS_DURATION) 
	, m_mediumFlapsDuration(MEDIUM_FLAP_DURATION)  
	, m_rapidFlapsDuration(RAPID_FLAP_DURATION)   
	, m_mediumFlapSpeed(MEDIUM_FLAP_SPEED)     
	, m_rapidFlapSpeed(RAPID_FLAP_SPEED)        
	, m_smallJumpForce(SMALL_JUMP_FORCE)
	, m_bigJumpForce(BIG_JUMP_FORCE)    
	, m_animDelay(0.0f)
{
	m_wing_left = new Wings(position, Vector2(WINGS_WIDTH, WINGS_HEIGHT), spriteSheet);
	m_wing_left->SetOffset(Vector2(-6.0f, -10.0f)); // Offset to the left
	m_wing_left->SetDirection(1);

	m_wing_right = new Wings(position, Vector2(WINGS_WIDTH, WINGS_HEIGHT), spriteSheet);
	m_wing_right->SetOffset(Vector2(6.0f, -10.0f)); // Offset to the right
	m_wing_right->SetDirection(-1);

	SetAnimation(ID_ANIM_PARAGOOMBA_WALK, true);
	m_visible = true;

	SetVelocity(Vector2(-Enemies::Goomba::WALK_SPEED, 0.0f));
	SetupCollisionComponent();

	m_isCollidable = true;

}

void ParaGoomba::Render(DirectX::SpriteBatch* spriteBatch)
{
	Entity::Render(spriteBatch);

	if (!m_isDying && m_hasWings) {
		if (m_wing_left->IsActive()) {
			m_wing_left->Render(spriteBatch);
		}
		if (m_wing_right->IsActive()) {
			m_wing_right->Render(spriteBatch);
		}
	}
}

void ParaGoomba::OnCollision(const CollisionResult& event)
{
	Mario* mario = dynamic_cast<Mario*>(event.collidedWith);
	Block* block = dynamic_cast<Block*>(event.collidedWith);

	if (event.collidedWith->GetCollisionGroup() == CollisionGroup::NONSOLID && event.contactNormal.x != 0) {
		return;
	}

	if (event.contactNormal.x != 0 && block)
	{
		if (block->IsSolid())
		{
			Vector2 vel = GetVelocity();
			vel.x = -vel.x;
			SetVelocity(vel);

		}

	}

	if (event.contactNormal.y < 0 && block)
	{
		Vector2 vel = GetVelocity();
		//vel += event.contactNormal * Vector2(std::abs(vel.x), std::abs(vel.y)) * (1.0f - event.contactTime);
		vel.y = 0.0f;
		SetVelocity(vel);
		m_isJumping = false;
		return;
	}

	if (event.contactNormal.y > 0) // Collision from above
	{
		if (mario) {
			if (m_hasWings) {
				EffectManager::GetInstance()->CreatePointEffect(GetPosition(), 100, true);
				TransformToGoomba();

				Vector2 vel = mario->GetVelocity();
				vel.y = Player::BOUNCE_FORCE;
				mario->SetVelocity(vel);
			}
			else {

				Die(DyingType::STOMPED);

				Vector2 vel = mario->GetVelocity();
				vel.y = Player::BOUNCE_FORCE;
				mario->SetVelocity(vel);
			}
		}
		if (block) {
			if (block->IsSolid()) {
				Vector2 vel = GetVelocity();
				vel.y = 0.0f;
				SetVelocity(vel);
			}
		}
		return;
	}

	else if (mario && !m_isDying) {
		mario->Damage();
	}
}

void ParaGoomba::Die(DyingType type)
{
	EffectManager::GetInstance()->CreatePointEffect(GetPosition(), 100, true);
	if (type == DyingType::STOMPED)
	{
		if (!m_isDying) {
			m_dyingType = type;
			m_isDying = true;
			m_deathTimer = 0.0f;
			m_isCollidable = false;
			SetAnimation(ID_ANIM_PARAGOOMBA_DIE, false);
			SetVelocity(Vector2::Zero);
		}
		return;
	}
	if (type == DyingType::BONKED)
	{
		if (m_hasWings)
		{
			TransformToGoomba();
		}
		if (!m_isDying) {
			m_dyingType = type;
			m_isDying = true;
			m_isCollidable = false;
			m_deathTimer = 0.0f;
			SetAnimation(ID_ANIM_PARAGOOMBA_WALK, false);
			m_animator->SetFlipVertical(true);
			SetVelocity(Vector2(Enemies::Goomba::WALK_SPEED,
				-Enemies::DEATH_BOUNCE_VELOCITY));
		}
		return;
	}
}

void ParaGoomba::TransformToGoomba()
{
	m_wing_left->Deactivate();
	m_wing_right->Deactivate();

	m_hasWings = false;

	m_isJumping = false;
}

bool ParaGoomba::HasWings() const
{
	return m_hasWings;
}

void ParaGoomba::Update(float dt)
{
	m_isGrounded = Collision::GetInstance()->GroundCheck(this, dt);

	if (!m_isGrounded) {
		Vector2 vel = GetVelocity();
		vel.y += Enemies::ParaGoomba::GRAVITY * dt;
		SetVelocity(vel);
	}

	if (m_isDying && m_isActive) {
		m_deathTimer += dt;
		if (m_dyingType == DyingType::STOMPED)
		{
			if (m_deathTimer >= Enemies::DEATH_STOMP_ANI_TIME) {
				// After 0.5 seconds, deactivate the Goomba
				m_isActive = false;
				m_visible = false;// Remove collision component
			}
			return;
		}
		if (m_dyingType == DyingType::BONKED)
		{
			if (m_deathTimer >= Enemies::DEATH_BONK_ANI_TIME) {
				// After 2.0 seconds, deactivate the Goomba
				m_isActive = false;
				m_visible = false;// Remove collision component
				return;
			}
			SetPosition(GetPosition() + GetVelocity() * dt);
			return;
		}
	}


	Mario* mario = dynamic_cast<Mario*>(World::GetInstance()->GetPlayer());

	static float directionUpdateTimer = 0;
	static int attempt = 0;
	const float UPDATE_INTERVAL = 1.5f;
	const int MAX_ATTEMPTS = 10;

	if (abs(mario->GetPosition().x - GetPosition().x) < 80.0f)
	{
		directionUpdateTimer += dt;
		if (directionUpdateTimer >= UPDATE_INTERVAL) {
			directionUpdateTimer = 0;

			if (mario && attempt < MAX_ATTEMPTS) {
				WalkInMarioDirection(mario);
				attempt++;
			}
		}
	}



	//// JUMPING PHASE LOGIC
	if (m_hasWings)
	{
		m_phaseTimer += dt;

		switch (m_currentPhase)
		{
		case 0:
		{
			if (m_isGrounded && m_phaseTimer >= m_closedWingsDuration)
			{
				m_currentPhase = 1;
				m_phaseTimer = 0.0f;
				m_jumpCount = 0;
				m_jumpTimer = 0.0f;
				m_animDelay = 0.0f;
			}
		}
		break;

		case 1:
		{
			m_jumpTimer += dt;

			if (m_isGrounded && m_jumpCount < m_jumpsBeforeBigJump && m_jumpTimer >= 0.35f)
			{
				m_jumpCount++;
				m_jumpTimer = 0.0f;
				Jump(m_smallJumpForce);
				Vector2 vel = GetVelocity();
				// SetPosition(GetPosition() + vel * dt);
			}

			if ((m_jumpCount == m_jumpsBeforeBigJump && m_isGrounded)) {
				m_currentPhase = 2;
				m_phaseTimer = 0.0f;
				m_jumpTimer = 0.0f;
				m_jumpCount = 0;
				m_animDelay = 0.0f;
			}

		}
		break;

		case 2:
		{
			if (!m_isJumping && m_isGrounded)
			{
				m_jumpCount++;
				if (m_jumpCount == 1)
				{
					Jump(m_bigJumpForce);
					Vector2 vel = GetVelocity();
					// SetPosition(GetPosition() + vel * dt);
				}
			}


			if (!m_isJumping && m_isGrounded && m_jumpCount >= 2)
			{
				m_phaseTimer += dt;


				if (m_phaseTimer >= 0.5f)
				{
					m_currentPhase = 0;
					m_phaseTimer = 0.0f;
					m_jumpCount = 0;
				}
			}
		}
		break;
		}
		Entity::Update(dt);
	}

	m_animTimer += dt;
	if (m_animTimer >= m_frameTime)
	{
		m_animTimer = 0.0f;
		m_flipFrame = !m_flipFrame;

		if (GetAnimator())
		{
			GetAnimator()->SetFlipHorizontal(m_flipFrame);
		}
	}

	// Update wings animator
	if (m_hasWings)
	{
		Vector2 currentPosition = GetPosition();

		m_wing_left->UpdatePosition(currentPosition);
		m_wing_right->UpdatePosition(currentPosition);

		switch (m_currentPhase)
		{
		case 0:
			m_wing_left->SetAnimation(ID_ANIM_WINGS_FLAP_DOWN, false);
			m_wing_right->SetAnimation(ID_ANIM_WINGS_FLAP_DOWN, false);
			break;

		case 1:
			m_animDelay += dt;
			if (m_animDelay >= 0.2f) {
				m_wing_left->HandleFlapping(dt, m_mediumFlapSpeed);
				m_wing_right->HandleFlapping(dt, m_mediumFlapSpeed);
			}
			break;

		case 2:
			m_animDelay += dt;
			if (m_animDelay >= 0.1f && m_animDelay < 0.7f) {
				m_wing_left->HandleFlapping(dt, m_rapidFlapSpeed);
				m_wing_right->HandleFlapping(dt, m_rapidFlapSpeed);
			}
			if (m_animDelay >= 0.7f && m_animDelay < 0.8f) {
				m_wing_left->SetAnimation(ID_ANIM_WINGS_FLAP_DOWN, false);
				m_wing_right->SetAnimation(ID_ANIM_WINGS_FLAP_DOWN, false);
			}
			if (m_animDelay >= 0.8f) {
				m_wing_left->HandleFlapping(dt, m_rapidFlapSpeed);
				m_wing_right->HandleFlapping(dt, m_rapidFlapSpeed);
			}
			break;
		}
	}

	Vector2 vel = GetVelocity();
	SetPosition(GetPosition() + vel * dt);
}

void ParaGoomba::SetupCollisionComponent()
{
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x, curSize.y);
	m_collisionComponent->SetSize(newSize);

	m_collisionComponent->SetPosition(GetPosition() + Vector2(newSize.x / 2, newSize.y / 2));
}


void ParaGoomba::WalkInMarioDirection(Mario* mario)
{
	if (mario) {
		Vector2 marioPos = mario->GetPosition();
		Vector2 goombaPos = GetPosition();

		Vector2 vel = GetVelocity();

		SetVelocity(vel);
		if (marioPos.x > goombaPos.x) {
			vel.x = Enemies::Goomba::WALK_SPEED;
			SetVelocity(Vector2(vel));
		}
		else {
			vel.x = -Enemies::Goomba::WALK_SPEED;
			SetVelocity(Vector2(vel));
		}
	}
}

void ParaGoomba::Jump(float strength) {
	if (m_isGrounded) {
		Vector2 vel = GetVelocity();
		vel.y = strength;
		SetVelocity(vel);
		m_isGrounded = false;
		m_isJumping = true;
	}
}