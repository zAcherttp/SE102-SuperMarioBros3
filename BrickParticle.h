#pragma once
#include "Entity.h"

class BrickParticle : public Entity
{
public:
    BrickParticle(Vector2 position, Vector2 size , SpriteSheet* spriteSheet, Vector2 velocity);
    virtual void Update(float dt) override;
    virtual void Render(DirectX::SpriteBatch* spriteBatch) override;
    void UpdateRotation(float dt);
private:
    Vector2 m_velocity;
    float m_gravity;
    float m_lifetime;
    float m_maxLifetime;

    // Rotation variables
    float m_rotationTimer = 0.0f;
    float m_rotationFrameTime = 0.05f;
    int m_currentRotationFrame;
    static const int ROTATION_FRAMES = 4;
};