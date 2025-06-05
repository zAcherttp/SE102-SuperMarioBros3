#pragma once
#include "Troopa.h"
#include "Effect.h"
#include "Wings.h"
#include <vector>

#define FLY_HEIGHT 110.0f // Height at which the Troopa flies
#define MAX_FLYING_SPEED 70.0f // Maximum speed when flying
#define VERTICAL_ACCELERATION 100.0f // Acceleration when flying
class RedTroopas : public Troopa
{
public:
	RedTroopas(Vector2 position, Vector2 size, SpriteSheet* spriteSheet, bool hasWing = false);
	~RedTroopas() = default;

	void Update(float dt) override;
	void Render(DirectX::SpriteBatch* spriteBatch) override;

	// void OnCollision(const CollisionResult& event) override;
	void SetupCollisionComponent();
	void Die(DyingType type) override;

	// New function to update sprite direction based on velocity
	void UpdateSpriteDirection();

	void TransformToShell();
	void TransformToTroopa();

	void InitializeWing();


	// Check for platform edges using raycasts
	bool CheckEdge();

	void OnCollision(const CollisionResult& event) override;

	void StartVibration();
	void UpdateVibration(float dt);

	void HandleBounceCollision();

private:

	bool m_isBouncing = false;
	int m_bounceCount = 0;
	float m_bounceForce = 100.0f;
	float m_bounceDamping = 0.6f;

	float m_animTimer;
	float m_frameTime;
	bool m_flipFrame;
	float m_deathTimer;
	bool m_isDying;

	bool m_isVibrating;
	float m_vibrateTimer;
	float m_vibrateInterval;
	float m_vibrateAmplitude;
	int m_vibrateCount;
	int m_maxVibrateCount;
	Vector2 m_originalPosition;
	bool m_vibrateDirection;
	bool m_firstRevivePhase;
	bool m_secondRevivePhase;

	Wings* m_wing;
	bool m_hasWing = false;
	float m_startPositionY;
	bool m_isGoingUp;

	float m_reviveTimer;
	float m_reviveTime;

	bool m_isColliding;

	float m_lastDirectionX;

	bool m_isFlipped;

	std::vector<Effect*> m_bonkEffects;

	std::map<InteractionPointType, bool> m_pointCollisionState;

	bool m_canRevive = false;

};

