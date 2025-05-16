#include "pch.h"
#include "Effect.h"
#include "AssetIDs.h"
#include "Debug.h"

Effect::Effect(Vector2 position, Vector2 size, SpriteSheet* spriteSheet, EffectType type)
    : Entity(position, size, spriteSheet)
    , m_animTimer(0.0f)
    , m_type(type)
    , m_isFlipped(false)
    , m_flipTimer(0.0f)
    , m_flipTime(0.2f) // Time between flip frames
    , m_velocity(Vector2::Zero)
    , m_gravity(750.0f)        
    , m_initialJumpSpeed(-300.0f) 

{
    std::vector<const wchar_t*> frames;

    switch (type)
    {
        case EffectType::POINT:
        {
            break;
        }
        case EffectType::BONK:
        { 
            frames = { L"spark1", L"spark2"};
            DefineAnimation(ID_ANIM_EFFECT_BONK, frames, true, 0.1f);
            SetAnimation(ID_ANIM_EFFECT_BONK, true);

            break;
        }
        case EffectType::COIN:
        {
            frames = { L"coin-collect1", L"coin-collect2", L"coin-collect3"};
            DefineAnimation(ID_ANIM_EFFECT_COIN, frames, true, 0.07f);
            SetAnimation(ID_ANIM_EFFECT_COIN, true);
            m_velocity.y = m_initialJumpSpeed;
            break;
        }
        
    }
}

void Effect::Render(DirectX::SpriteBatch* spriteBatch)
{    
    Entity::Render(spriteBatch);
}

void Effect::Update(float dt) 
{
    if(!m_isActive) return;

    switch(m_type)
    {
        case EffectType::BONK:
        {
            m_animTimer += dt;
            m_flipTimer += dt;
            if (m_animTimer > 0.5f)
            {
                Deactivate();
                return;
            }
            if(m_flipTimer > m_flipTime)
            {
                m_isFlipped = !m_isFlipped;
                m_animator->SetFlipHorizontal(m_isFlipped);
                m_flipTimer = 0.0f;
            }
            break;
        }
        case EffectType::COIN:
        {
            m_animTimer += dt;
            
            m_velocity.y += m_gravity * dt;
            
            // Update position based on velocity
            Vector2 position = GetPosition();
            position += m_velocity * dt;
            SetPosition(position);
            
            if (m_animTimer > 0.7f)
            {
                Deactivate();
                return;
            }
            break;
        }
    }

    Entity::Update(dt);
}

void Effect::Deactivate()
{
    m_isActive = false;
    m_visible = false;
}
