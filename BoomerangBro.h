#pragma once
#include "Enemy.h"
#include "Boomerang.h"
#define FIREPIRANHA_WIDTH 16
#define FIREPIRANHA_HEIGHT 32

class Mario;
class World;

enum class BoomerangBroState
{
    WALKLEFT,
    WALKRIGHT,
    WAIT
};

class BoomerangBro : public Enemy
{
public:
	BoomerangBro(Vector2 position, Vector2 size, SpriteSheet* spriteSheet);

	void Update(float dt) override;
	void Render(DirectX::SpriteBatch* spriteBatch) override;

	void OnCollision(const CollisionResult& event) override;
	void SetupCollisionComponent();


	// Create and fire a bullet in the specified direction
	void Hold();
	void Throw();

    void Jump();

	// Set the reference to Mario for aiming
	void SetMario(Mario* mario) { m_mario = mario; }

	// Set the reference to World for adding bullets
	void SetWorld(World* world) { m_world = world; }

	// Update the bro's orientation based on Mario's position
	void UpdateBroOrientation(Mario* mario);

	void CreateBoomerangs();

	void OnBoomerangCaught(Boomerang* boomerang);

	bool CheckAABBCollision(const Entity* entityA, const Entity* entityB);

private:
	int m_leftX;
    int m_rightX;

    BoomerangBroState m_state;
	bool m_isThrowingPhase;
    float m_phaseTimer;
    float m_waitTimer;
	float m_jumpTimer;
	float m_jumpTime;


	bool m_isBommerangCreated = false;
	bool m_isBoomerangThrown = false;

	// Boomerang objects for throwing
	Boomerang* m_boomerang1; // Boomerang object for throwing
    Boomerang* m_boomerang2; // Second boomerang for dual-wielding

	// References needed for gameplay functionality
	Mario* m_mario;
	World* m_world;
	// Position tracking

	// Plant orientation state
	bool m_isFlipped;   // true: looking right, false: looking left


	float m_flipTime;
	float m_flipTimer;

	float m_offset;

};
