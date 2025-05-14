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

{
    std::vector<const wchar_t*> frames;

    if (type == EffectType::POINT) {

    } else if (type == EffectType::BONK) {
        frames = { L"spark1", L"spark2"};
        DefineAnimation(ID_ANIM_EFFECT_BONK, frames, true, 0.1f);
        SetAnimation(ID_ANIM_EFFECT_BONK, true);
    }
}

void Effect::Render(DirectX::SpriteBatch* spriteBatch)
{    
    Entity::Render(spriteBatch);
}

void Effect::Update(float dt) 
{
    if(!m_isActive) return;

    if(m_type == EffectType::BONK) {
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

    }
    Entity::Update(dt);
}

void Effect::Deactivate()
{
    m_isActive = false;
    m_visible = false;
}
