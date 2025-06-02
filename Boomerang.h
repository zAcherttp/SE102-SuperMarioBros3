#pragma once
#include "Enemy.h"
#define BULLET_WIDTH 12
#define BULLET_HEIGHT 12

enum class BoomerangState
{
	OUTWARD,
	RETURN,
	HOLDING
};	

#define BOOMERANG_HORIZONTAL_SPEED 220.0f  
#define BOOMERANG_MAX_VERTICAL_SPEED 220.0f 
#define BOOMERANG_INITAL_UPWARD_SPEED 80.0f  
#define BOOMERANG_GRAVITY 53.0f             
#define BOOMERANG_VERTICAL_ACCE 20.0f      
#define BOOMERANG_HORIZONTAL_ACCE 180.0f     
#define BOOMERANG_WIDTH 16
#define BOOMERANG_HEIGHT 16

#define BOOMERANG_LIFETIME 10.0f 

class Boomerang : public Enemy
{
public:
	Boomerang(Vector2 position, Vector2 size, SpriteSheet* spriteSheet, bool direction = true);
	~Boomerang() = default;

	// Implementation of Entity's pure virtual function
	void Update(float dt) override;

	void Render(DirectX::SpriteBatch* spriteBatch) override;

	void Deactivate();
	bool IsActive() const { return m_isActive; }

	// Sets the direction of the bullet (RIGHT or LEFT)
	void SetDirection();

	void UpdatePosition(const Vector2& ownerPosition);

	// Handle rotation animation
	void UpdateRotation(float dt);

	void SetState(BoomerangState state);

	BoomerangState GetState() const { return m_state; }

	void SetupCollisionComponent();

	void OnCollision(const CollisionResult& event) override;



	float m_lifeTimer;

private:
	BoomerangState m_state;

	bool m_direction = true; // true for right, false for left

	float BOOMERANG_VERTICAL_LENGTH;


	float m_accelerationX;
	float m_accelerationY;

	float m_startPositionX;
	float m_startPositionY;

	float m_animTimer;
	float m_rotationTimer;
	float m_frameTime;
	float m_rotationFrameTime;
	int m_currentRotationFrame;

	// Constants for rotation
	static const int ROTATION_FRAMES = 4;  // 0, 90, 180, 270 degrees
};


