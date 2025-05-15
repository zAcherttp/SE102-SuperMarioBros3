#pragma once
#include "Entity.h"
#include "Bullet.h"
#define FIREPIRANHA_WIDTH 16
#define FIREPIRANHA_HEIGHT 32

enum PlantState { HIDDEN, EMERGING, FIRING, RECEDING };

class Mario;
class World;

class FirePiranha : public Entity
{
public:
    FirePiranha(Vector2 position, Vector2 size, SpriteSheet* spriteSheet);
    ~FirePiranha();

    void Update(float dt) override;
    void Render(DirectX::SpriteBatch* spriteBatch) override;

    void OnCollision(const CollisionResult& event) override;
    void SetupCollisionComponent();
    void Die(DyingType type) override;

    // Create and fire a bullet in the specified direction
    void Fire(BulletDirection direction);
    
    // Set the reference to Mario for aiming
    void SetMario(Mario* mario) { m_mario = mario; }
    
    // Set the reference to World for adding bullets
    void SetWorld(World* world) { m_world = world; }
    
    // Clean up any active bullets
    void CleanupBullets();
    
    // Update the plant's orientation based on Mario's position
    void UpdatePlantOrientation(Mario* mario);

private:
    PlantState m_state;
    float m_timer;
    float m_emergeTime; // Time to emerge from pipe
    float m_fireDelay;  // Delay before firing
    float m_fireTime;   // Time in firing state

    
    
    Bullet* m_activeBullet;
    Vector2 BULLET_OFFSET; // Offset for bullet spawn position
    
    // References needed for gameplay functionality
    Mario* m_mario;
    World* m_world;
      // Position tracking
    Vector2 m_initialPosition; // Initial position when created
    
    // Plant orientation state
    bool m_isFlipped;   // true: looking right, false: looking left
    bool m_isLookingUp; // true: looking up, false: looking down
    float m_yThreshold; // Y threshold for looking up/down (352 pixels)

    bool m_isDying = false;
    float m_deathTimer;

    float m_flipTime;
    float m_flipTimer;
    
    
    // Determine which direction to fire based on Mario's position
    BulletDirection GetFireDirection() const;
};
