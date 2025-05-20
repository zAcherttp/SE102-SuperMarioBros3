#pragma once
#include "Enemy.h"

#define BITEPIRANHA_WIDTH 16
#define BITEPIRANHA_HEIGHT 32

class Mario;
class World;

enum BitePlantState { b_HIDDEN, b_EMERGING, b_BITING, b_RECEDING };

class BitePiranha : public Enemy
{
public:
	BitePiranha(Vector2 position, Vector2 size, SpriteSheet* spriteSheet);

	void Update(float dt) override;
	void Render(DirectX::SpriteBatch* spriteBatch) override;

	void OnCollision(const CollisionResult& event) override;
	void SetupCollisionComponent();
	void Die(DyingType type) override;

	// Set the reference to Mario for aiming
	void SetMario(Mario* mario) { m_mario = mario; }

	// Set the reference to World for adding bullets
	void SetWorld(World* world) { m_world = world; }


private:

	BitePlantState m_state;

    float m_timer;
    float m_flipTime;
    float m_deathTimer;
    float m_flipTimer;
    bool m_isFlipped;

    bool m_isDying;

	// References needed for gameplay functionality
	Mario* m_mario;
	World* m_world;

	// Position tracking
	Vector2 m_initialPosition; // Initial position when created

};
