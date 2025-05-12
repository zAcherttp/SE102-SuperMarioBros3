#pragma once
#include "Entity.h"
#define BULLET_WIDTH 12
#define BULLET_HEIGHT 12

// Define directions for the bullet
enum BulletDirection {
    TOP_RIGHT = 0,
    TOP_LEFT = 1,
    BOTTOM_RIGHT = 2,
    BOTTOM_LEFT = 3
};

class Bullet : public Entity
{
public:
    Bullet(Vector2 position, Vector2 size, SpriteSheet* spriteSheet, BulletDirection direction = BOTTOM_LEFT);
    ~Bullet() = default;

    // Implementation of Entity's pure virtual function
    void Update(float dt) override;
    
    void Render(DirectX::SpriteBatch* spriteBatch) override;

    void Deactivate();
    bool IsActive() const { return m_isActive; }
    
    // Sets the direction of the bullet (RIGHT or LEFT)
    void SetDirection(BulletDirection direction);
    
    // Handle rotation animation
    void UpdateRotation(float dt);

private:
    float m_animTimer;
    float m_rotationTimer;
    float m_frameTime;
    float m_rotationFrameTime;
    int m_currentRotationFrame;
    bool m_isActive;
    BulletDirection m_direction;
    float m_speed;
    
    // Constants for rotation
    static const int ROTATION_FRAMES = 4;  // 0, 90, 180, 270 degrees
};


