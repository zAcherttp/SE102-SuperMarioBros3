#pragma once
#include "Entity.h"

enum class EffectType
{
    POINT,
    BONK,
    COIN
};

class Effect : public Entity
{
public:
    Effect(Vector2 position, Vector2 size, SpriteSheet* spriteSheet, EffectType type);
    ~Effect() = default;

    // Implementation of Entity's pure virtual function
    void Update(float dt) override;
    
    void Render(DirectX::SpriteBatch* spriteBatch) override;

    void Deactivate();
    bool IsActive() const { return m_isActive; }
    
private:
    float m_animTimer;
    EffectType m_type;

    bool m_isFlipped;
    float m_flipTimer;
    float m_flipTime;

    Vector2 m_velocity;         
    float m_gravity;          
    float m_initialJumpSpeed; 

};