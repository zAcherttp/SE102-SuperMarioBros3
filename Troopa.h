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
	
protected:
	TroopaState m_state;
};
