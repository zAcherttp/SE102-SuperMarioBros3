#include "pch.h"
#include  "BitePiranha.h"
#include "Debug.h"
#include "AssetIDs.h"
#include "Mario.h"
#include "Block.h"
#include "Game.h"
#include "World.h"
#include "RedTroopas.h"


BitePiranha::BitePiranha(Vector2 position, Vector2 size, SpriteSheet* spriteSheet)
	: Enemy(position, size, spriteSheet)
	, m_state(b_HIDDEN)
	, m_world(nullptr)
	, m_initialPosition(position) // Store the initial position
    , m_timer(0)
    , m_isDying(false)


{
	SetupCollisionComponent();
	Log(__FUNCTION__, "Collision component initialized");
	m_mario = dynamic_cast<Mario*>(World::GetInstance()->GetPlayer());
	m_isCollidable = true;
    m_isActive = true;
    m_visible = false;
    SetAnimation(ID_ANIM_BITE_PIRANHA, true);
}


void BitePiranha::Update(float dt)
{

	if (m_isDying && m_isActive)
	{
		if (m_deathTimer > m_flipTime * 15)
		{
			m_isActive = false;
			return;
		}
		m_deathTimer += dt;

		m_flipTimer += dt;
		if (m_flipTimer > m_flipTime) {
			m_isFlipped = !m_isFlipped;
			m_animator->SetFlipVertical(m_isFlipped);
			m_flipTimer = 0.0f;
		}
		return;
	}

	Vector2 m_pos = GetPosition();
	Vector2 mario_pos = m_mario ? m_mario->GetPosition() : Vector2(0, 0);


	// Debug position periodically
	static float debugTimer = 0;
	debugTimer += dt;


    Vector2 vel = GetVelocity();

	m_timer += dt;

	switch (m_state)
	{
	case b_HIDDEN:
		if (abs(m_pos.x - mario_pos.x) < 120.0f && abs(m_pos.x - mario_pos.x) >= 24.0f)
		{
			m_state = b_EMERGING;
			m_timer = 0.0f; // Reset timer for emerging state
		}
		break;

	case b_EMERGING:
        if(m_timer >= 0.3f) m_visible = true;

		if (m_pos.y > m_initialPosition.y - 24.0f) // If above the initial position
		{
			vel.x = 0.0f;
			vel.y = -50.0f;
			SetVelocity(vel);
			SetPosition(GetPosition() + GetVelocity() * dt);
		}
		if (m_pos.y <= m_initialPosition.y - 24.0f) // If at or below the initial position
		{
			vel.x = 0.0f;   // No horizontal movement
			vel.y = 0.0f;   // Stop vertical movement
			SetVelocity(vel);
			m_timer = 0.0f; // Reset timer for firing state
			m_state = b_BITING; // Transition to firing state
		}

		break;

    case b_BITING:

        if(m_timer >= 2.0f)
        {
            m_state = b_RECEDING;
            m_timer = 0;
        }
        break;

	case b_RECEDING:

		if (m_pos.y < m_initialPosition.y)
		{
			vel.y = 50.0f;
			vel.x = 0.0f;
			SetVelocity(vel);
			SetPosition(GetPosition() + GetVelocity() * dt);

		}
		if (m_timer >= 2.0f)
		{
			m_state = b_HIDDEN;
			m_timer = 0.0f;
		}
		break;
	
    }
    Entity::Update(dt);
}

void BitePiranha::Render(DirectX::SpriteBatch* spriteBatch)
{
	Entity::Render(spriteBatch);
}

void BitePiranha::OnCollision(const CollisionResult& event)
{
	Block* block = dynamic_cast<Block*>(event.collidedWith);

	Mario* mario = dynamic_cast<Mario*>(event.collidedWith);

	if (block) {
		if (block->IsSolid()) {}
	}

	if (mario)
	{
		mario->Damage();
		return;
	}
}

void BitePiranha::SetupCollisionComponent()
{
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x, curSize.y); // Make collision box slightly smaller
	m_collisionComponent->SetSize(newSize);

	m_collisionComponent->SetPosition(GetPosition() + Vector2(newSize.x / 2, newSize.y / 2));

	m_collisionComponent->SetSize(newSize); // Make collision box smaller than visual size
	Log(__FUNCTION__, "FirePiranha collision component initialized - collision disabled");
}


void BitePiranha::Die(DyingType type)
{
	if (!m_isDying) {
		m_dyingType = type;
		m_isDying = true;
		m_isCollidable = false;
		m_deathTimer = 0.0f;
		SetAnimation(ID_ANIM_EFFECT_PIRANHA_DIE, true);
		return;
	}
}
