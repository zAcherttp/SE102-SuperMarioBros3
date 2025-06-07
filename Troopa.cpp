#include "pch.h"
#include "Troopa.h"

Troopa::Troopa(Vector2 position, Vector2 size, SpriteSheet *spriteSheet)
	: Enemy(position, size, spriteSheet)
{
	
}

void Troopa::SetState(TroopaState state)
{
	if (m_state == DEAD) return;
	m_state = state;
}

TroopaState Troopa::GetState() const
{
	return m_state;
}

void Troopa::HandleBounceCollision()
{
	// Default implementation does nothing, can be overridden by derived classes
}
void Troopa::HandleSweepCollision(float x_force, float y_force, bool spawnEffect)
{
	// Default implementation does nothing, can be overridden by derived classes
}