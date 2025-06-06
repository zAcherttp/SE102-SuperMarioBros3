#pragma once
#include "Enemy.h"
enum TroopaState
{
	WALKING,
	SHELL_IDLE,
	SHELL_SLIDE,
	DEAD
};

class Troopa : public Enemy
{
public:
    Troopa(Vector2 position, Vector2 size, SpriteSheet* spriteSheet);

	void SetState(TroopaState state);
	TroopaState GetState() const;

	virtual void HandleBounceCollision();
	virtual void HandleSweepCollision(float x_force, float y_force, bool spawnEffect);
protected:
	TroopaState m_state;

	bool m_isBouncing = false;
	int m_bounceCount = 0;
	float m_bounceForce = 100.0f;
	float m_bounceDamping = 0.6f;

};
