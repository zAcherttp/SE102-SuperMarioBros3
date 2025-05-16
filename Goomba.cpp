#include "pch.h"
#include "Goomba.h"
#include "Debug.h"
#include "AssetIDs.h"
#include "Mario.h"
#include "GameConfig.h"
#include "ParaGoomba.h"
#include "Block.h"

Goomba::Goomba(Vector2 position, Vector2 size, SpriteSheet* spriteSheet)
	: Entity(position, size, spriteSheet)
	, m_animTimer(0.0f)
	, m_frameTime(0.15f)  // Controls how fast the flip animation occurs
	, m_flipFrame(false)
{
	// Define the animation using the single frame we have

	SetAnimation(ID_ANIM_GOOMBA_WALK, true);
	m_visible = true; // Set visibility to true
	m_isDying = false; // Initialize dying state

	// Set initial velocity - slow movement to the left
	SetVelocity(Vector2(-GameConfig::Enemies::Goomba::WALK_SPEED, 0.0f));

	// Setup collision component
	SetupCollisionComponent();

}

void Goomba::Render(DirectX::SpriteBatch* spriteBatch)
{
	Entity::Render(spriteBatch);
}

void Goomba::OnCollision(const CollisionResult& event)
{

	Mario* mario = dynamic_cast<Mario*>(event.collidedWith);
	ParaGoomba* paraGoomba = dynamic_cast<ParaGoomba*>(event.collidedWith);
	Block* block = dynamic_cast<Block*>(event.collidedWith);
	Goomba* goomba = dynamic_cast<Goomba*>(event.collidedWith);

	if (event.collidedWith->GetCollisionGroup() == CollisionGroup::NONSOLID) {
		return;
	}

	if (event.contactNormal.x != 0)
	{
		if (block && block->IsSolid() || goomba)
		{
			float targetSpeed = GameConfig::Enemies::Goomba::WALK_SPEED;
			Vector2 vel = GetVelocity();

			vel.x = -vel.x;
			SetVelocity(vel);
			return;
		}
		if (mario) {
			mario->Damage();
		}
	}
	if (event.contactNormal.y > 0)
	{
		if (mario) {
			Die(DyingType::STOMPED);

			Vector2 vel = mario->GetVelocity();
			vel.y = GameConfig::Mario::BOUNCE_FORCE;
			mario->SetVelocity(vel);
		}
		return;
	}
	if (event.contactNormal.y < 0 && event.collidedWith != mario) {

		Vector2 vel = GetVelocity();
		vel.y = 0.0f;
		SetVelocity(vel);
		m_isGrounded = true;
		return;
	}
}

void Goomba::Die(DyingType type)
{
	if (!m_isDying)
	{
		m_dyingType = type;
		m_isDying = true;
		m_isCollidable = false;
		m_deathTimer = 0.0f;
	}
	if (type == DyingType::STOMPED) {
		SetAnimation(ID_ANIM_GOOMBA_DIE, false);
		return;
	}
	if (type == DyingType::BONKED) {
		SetAnimation(ID_ANIM_GOOMBA_WALK, false);
		m_animator->SetFlipVertical(true);
		SetVelocity(Vector2(GameConfig::Enemies::Goomba::WALK_SPEED, -GameConfig::Enemies::DEATH_BOUNCE_VELOCITY));
		return;
	}
}

void Goomba::Update(float dt)
{
	m_isGrounded = Collision::GetInstance()->GroundCheck(this, dt);
	if (!m_isGrounded) {
		Vector2 vel = GetVelocity();
		vel.y += GameConfig::Physics::GRAVITY * dt;
		SetVelocity(vel);
	}

	if (m_isDying && m_isActive) {
		m_deathTimer += dt;
		if (m_dyingType == DyingType::STOMPED)
		{
			if (m_deathTimer >= GameConfig::Enemies::DEATH_STOMP_ANI_TIME) {

				m_isActive = false;
				m_visible = false;
			}
			return;
		}
		if (m_dyingType == DyingType::BONKED)
		{
			if (m_deathTimer >= GameConfig::Enemies::DEATH_BONK_ANI_TIME) {

				m_isActive = false;
				m_visible = false;
				return;
			}
			SetPosition(GetPosition() + GetVelocity() * dt);
			return;
		}
	}

	// Check if entity is on ground - important for physics

	// Apply gravity only if in the air

	m_animTimer += dt;
	if (m_animTimer >= m_frameTime)
	{
		m_animTimer = 0.0f;
		m_flipFrame = !m_flipFrame;

		// Apply horizontal flip to animator
		if (GetAnimator())
		{
			GetAnimator()->SetFlipHorizontal(m_flipFrame);
		}
	}

	SetPosition(GetPosition() + GetVelocity() * dt);

	Entity::Update(dt);
}

void Goomba::SetupCollisionComponent()
{
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x, curSize.y);
	m_collisionComponent->SetSize(newSize);

	m_collisionComponent->SetPosition(GetPosition() + Vector2(newSize.x / 2, newSize.y / 2));
}
