#pragma once
#include "Entity.h"
#include "EffectManager.h"

// Leaf states
#define LEAF_STATE_RISING  0
#define LEAF_STATE_FALLING 1

// Leaf physics constants
#define LEAF_LAUNCH_SPEED      300.0f
#define LEAF_MIN_SPEED         10.0f
#define LEAF_GRAVITY           900.0f
#define LEAF_FALLING_GRAVITY   10.0f
#define LEAF_HORIZONTAL_SPEED  60.0f
#define LEAF_MAX_FALL_SPEED    40.0f
#define LEAF_DIRECTION_CHANGE_TIME 0.8f

class Leaf : public Entity
{
public:
	Leaf(Vector2 position, Vector2 size, SpriteSheet* spriteSheet);

	void Update(float dt) override;
	void Render(DirectX::SpriteBatch* spriteBatch) override;

	void OnCollision(const CollisionResult& event) override;

	void FlipAnimation();

private:
	void SetupCollisionComponent();

	int m_state;         // Current state (rising or falling)
	int m_direction;     // Current horizontal direction (1 = right, -1 = left)
	float m_fallTimer;   // Timer for direction changes during falling

	Vector2 m_velocity;  // Current velocity of the leaf
	Vector2 m_acceleration;
};