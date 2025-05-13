#pragma once
#include "Entity.h"
#define BULLET_WIDTH 12
#define BULLET_HEIGHT 12

// Define directions for the bullet
enum BulletDirection {
    // Right side angles (clockwise from top)
    TOP_RIGHT_45 = 0,      // 45° angle (1, -1)
    TOP_RIGHT_60 = 1,      // 60° angle (calculate vector)
    TOP_RIGHT_120 = 2,     // 120° angle (calculate vector)
    TOP_RIGHT_135 = 3,     // 135° angle (1, 1)
    
    // Left side angles (clockwise from top)
    TOP_LEFT_45 = 4,       // 45° angle (-1, -1)
    TOP_LEFT_60 = 5,       // 60° angle (calculate vector)
    TOP_LEFT_120 = 6,      // 120° angle (calculate vector) 
    TOP_LEFT_135 = 7       // 135° angle (-1, 1)
};

class Bullet : public Entity
{
public:
    Bullet(Vector2 position, Vector2 size, SpriteSheet* spriteSheet, BulletDirection direction = TOP_LEFT_45);
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


