#include "pch.h"
#include "FireLeaf.h"
#include "Debug.h"
#include "AssetIDs.h"
#include "Mario.h"
#include "Block.h"
#include "GameConfig.h"

FireLeaf::FireLeaf(Vector2 position, Vector2 size, SpriteSheet* spriteSheet)
    : Entity(position, size, spriteSheet)
    
    {
        DefineAnimation(ID_ANIM_FIRE_LEAF, { L"leaf" }, false, 0.1f);
        SetAnimation(ID_ANIM_FIRE_LEAF, false);
        SetupCollisionComponent();
        SetDirection(-1);
        // Initialize movement properties
        SetVelocity(Vector2(0, -LEAF_LAUNCH_SPEED));
        m_state = LEAF_STATE_RISING;
        m_direction = 1;
        m_fallTimer = 0.0f;
        // m_acceleration = Vector2(120, 0);
    }

void FireLeaf::Update(float dt)
{
    m_velocity = GetVelocity();
    switch (m_state)
    {
        case LEAF_STATE_RISING:
        {
            // Rising phase - move upward until reaching maximum height
            if (m_velocity.y < 0 && abs(m_velocity.y) < LEAF_MIN_SPEED)
            {
                m_state = LEAF_STATE_FALLING;
                m_velocity.y = 0;
                m_velocity.x = LEAF_HORIZONTAL_SPEED * m_direction;
                SetDirection(m_direction);
            }
            else
            {
                // Add gravity to slow the rising
                m_velocity.y += LEAF_GRAVITY * dt;
                
            }
            SetVelocity(m_velocity);
            break;
        }   
        case LEAF_STATE_FALLING:
        {
            m_fallTimer += dt;
            
            // Apply gravity for falling
            m_velocity.y += LEAF_FALLING_GRAVITY * dt;
            if (m_velocity.y > LEAF_MAX_FALL_SPEED)
                m_velocity.y = LEAF_MAX_FALL_SPEED;
             

            if (m_fallTimer >= LEAF_DIRECTION_CHANGE_TIME)
            {   
                FlipAnimation();
                m_direction *= -1;
                m_velocity.x = LEAF_HORIZONTAL_SPEED * m_direction;

                m_fallTimer = 0.0f;
            }
            SetVelocity(m_velocity);
            break;
        }
    }    

    SetPosition(GetPosition() + GetVelocity() * dt);
    Entity::Update(dt);
}
    
void FireLeaf::Render(DirectX::SpriteBatch* spriteBatch)
{
    Entity::Render(spriteBatch);
}

void FireLeaf::OnCollision(const CollisionResult& event)
{
    Mario* mario = dynamic_cast<Mario*>(event.collidedWith);
    if (mario)
    {
        // Handle the interaction with Mario
        EffectManager::GetInstance()->CreatePointEffect(GetPosition(), 100);
        EffectManager::GetInstance()->CreateCoinEffect(GetPosition());

        m_isActive = false;
    }
 
}

void FireLeaf::SetupCollisionComponent()
{
    Vector2 curSize = m_collisionComponent->GetSize();
    Vector2 newSize = Vector2(curSize.x, curSize.y);
    m_collisionComponent->SetSize(newSize);
    m_collisionComponent->SetPosition(GetPosition());
}

void FireLeaf::FlipAnimation()
{
    if (GetAnimator())
    {
        SetDirection(GetVelocity().x < 0 ? 1 : -1);
    }
}