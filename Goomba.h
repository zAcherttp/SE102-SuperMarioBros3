#pragma once
#include "Entity.h"
#define GOOMBA_WIDTH 16
#define GOOMBA_HEIGHT 16

class Goomba : public Entity
{
public:
    Goomba(Vector2 position, Vector2 size, SpriteSheet* spriteSheet);
    ~Goomba() = default;

    void Update(float dt) override;
    void Render(DirectX::SpriteBatch* spriteBatch) override;

    void OnCollision(const CollisionResult& event) override;
    void SetupCollisionComponent();
    void Die(DyingType type) override;
private:
    float m_animTimer;
    float m_frameTime;
    bool m_flipFrame;
    float m_deathTimer;
    bool m_isDying;
};

