#include "pch.h"
#include "Mushroom.h"
#include "Debug.h"
#include "AssetIDs.h"
#include "Mario.h"
#include "Block.h"
#include "GameConfig.h"

Mushroom::Mushroom(Vector2 position, Vector2 size, SpriteSheet* spriteSheet)
    : Entity(position, size, spriteSheet)
    , m_phase(MushroomPhase::GROWING)
    , m_initialPosition(position)
    {
        DefineAnimation(ID_ANIM_MUSHROOM, { L"mushroom" }, false, 0.1f);
        SetAnimation(ID_ANIM_MUSHROOM, false);
 
    }

void Mushroom::Update(float dt)
{
    Vector2 m_pos = GetPosition();
    Vector2 vel = GetVelocity();
    
    switch(m_phase)
    {
        case MushroomPhase::GROWING:
        {
            if(m_pos.y > m_initialPosition.y - 17.0f) // If above the initial position
            {
                vel.x = 0.0f;   
                vel.y = -25.0f; 
                SetVelocity(vel);
            }
            if(m_pos.y <= m_initialPosition.y - 17.0f) // If at or below the initial position
            {
                vel.x = 0.0f;   
                vel.y = 0.0f; 
                SetVelocity(vel);
                SetupCollisionComponent();

                m_phase = MushroomPhase::WALKING;
            }
            break;
        }
        case MushroomPhase::WALKING:
        {
            if(!m_isVelocitySet)
            {
                Mario* mario = dynamic_cast<Mario*>(World::GetInstance()->GetPlayer());

                int dir = (mario->GetPosition().x - m_pos.x) >= 0 ? -1 : 1;

                Log(LOG_INFO, "Mario x: " + std::to_string(mario->GetPosition().x) + ", Mushroom x: " + std::to_string(m_pos.x) + ", dir: " + std::to_string(dir));
                Vector2 vel = GetVelocity();
                vel.x = 60.0f * dir; 
                SetVelocity(vel);
                m_isVelocitySet = true;
            }
            break;
        }
    }
    
    m_isGrounded = Collision::GetInstance()->GroundCheck(this, dt);

    if (!m_isGrounded && m_hasInitializedCollisionComponent) {
        Vector2 vel = GetVelocity();
        vel.y += GameConfig::Physics::GRAVITY * dt;
        SetVelocity(vel);
    }

    SetPosition(GetPosition() + GetVelocity() * dt);
    Entity::Update(dt);
}

void Mushroom::Render(DirectX::SpriteBatch* spriteBatch)
{
    Entity::Render(spriteBatch);
}

void Mushroom::OnCollision(const CollisionResult& event)
{
    Mario* mario = dynamic_cast<Mario*>(event.collidedWith);
    Block* block = dynamic_cast<Block*>(event.collidedWith);


    if (event.collidedWith->GetCollisionGroup() == CollisionGroup::NONSOLID && event.contactNormal.x != 0) {
        return;  // Ignore collisions with non-solid entities
    }

    if (event.collidedWith->GetCollisionGroup() == CollisionGroup::NONSOLID && event.contactNormal.x != 0) {
		return;
	}

	if (event.contactNormal.x != 0 && block)
	{
		if (block->IsSolid())
		{
			float targetSpeed = GameConfig::Enemies::Goomba::WALK_SPEED;
			Vector2 vel = GetVelocity();
			vel.x = -vel.x;
			SetVelocity(vel);
		}

	}

	if (event.contactNormal.y < 0 && block)
	{
		Vector2 vel = GetVelocity();
		vel += event.contactNormal * Vector2(std::abs(vel.x), std::abs(vel.y)) * (1.0f - event.contactTime);
		SetVelocity(vel);
		return;
	}

    if (mario)
    {
        // Handle the interaction with Mario
        EffectManager::GetInstance()->CreatePointEffect(GetPosition(), 100);
        EffectManager::GetInstance()->CreateCoinEffect(GetPosition());

        m_isActive = false;
    }
 
}

void Mushroom::SetupCollisionComponent()
{
    Vector2 curSize = m_collisionComponent->GetSize();
    Vector2 newSize = Vector2(curSize.x, curSize.y);
    m_collisionComponent->SetSize(newSize);
    m_collisionComponent->SetPosition(GetPosition());
    m_hasInitializedCollisionComponent = true;
}