#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "FloatingPlatform.h"
#include "GameConfig.h"

using namespace GameConstants::Blocks::FloatingPlatform;

FloatingPlatform::FloatingPlatform(Vector2 position, Vector2 size, int countX, bool isSolid, SpriteSheet* spriteSheet, Vector2 patrolOffset, float roundTime)
	: Block(position, size, spriteSheet)
{
	m_tileXcount = countX;
	m_tileYcount = 1;
	m_isSolid = isSolid;
	m_isStatic = true;

	m_patrolOffset = patrolOffset;
	m_roundTime = roundTime;
	m_startPosition = position;

	// update the collision box to match the size of the ground
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x * m_tileXcount, curSize.y);
	m_collisionComponent->SetSize(newSize);
	// update the position as size is the center of the entity
	m_collisionComponent->SetPosition(position + Vector2(newSize.x / 2, newSize.y / 2));
}

void FloatingPlatform::Fall()
{
	if (m_isFalling) return;
	m_isFalling = true;
	SetVelocity(Vector2(0.f, 0.f));
}

bool FloatingPlatform::GetIsFalling() const
{
	return m_isFalling;
}

void FloatingPlatform::Render(DirectX::SpriteBatch* spriteBatch)
{
	Vector2 size = m_collisionComponent->GetSize();
	Vector2 tileSize = Vector2(size.x / m_tileXcount, size.y / m_tileYcount);
	Vector2 pos = m_collisionComponent->GetPosition() - Vector2(size.x / 2 - tileSize.x / 2, size.y / 2 - tileSize.y / 2);

	// draw the first tile
	m_animator->Draw(spriteBatch, ID_SPRITE_MOVING_PLATFORM_LEFT, pos, 0.5f);
	for (int i = 1; i < m_tileXcount - 1; ++i)  // draw the mid tiles
	{
		m_animator->Draw(spriteBatch, ID_SPRITE_MOVING_PLATFORM_MID, pos + Vector2(i * tileSize.x, 0), 0.5f);
	}
	// draw the last tile
	m_animator->Draw(spriteBatch, ID_SPRITE_MOVING_PLATFORM_RIGHT, pos + Vector2((m_tileXcount - 1) * tileSize.x, 0), 0.5f);
}


void FloatingPlatform::Update(float dt)
{
	if (!m_isFalling) {
		m_roundTimer += dt;

		// update position based on patrol offset and round time
		// move linearly between the two points
		if (m_roundTimer >= m_roundTime) {
			m_roundTimer = 0.0f;
		}

		// [0, 1] range for the timer -> posFactor 1 -> 0 -> 1
		float posFactor = 0.0f;
		if (m_roundTimer >= m_roundTime / 2.0f) {
			posFactor = 1.0f - (m_roundTimer - m_roundTime / 2.0f) / (m_roundTime / 2.0f);
		}
		else {
			posFactor = m_roundTimer / (m_roundTime / 2.0f);
		}

		// calculate the appropriate velocity based on the movement direction
		Vector2 velocity = m_patrolOffset / m_roundTime * 2.f;
		m_collisionComponent->SetVelocity(velocity * (m_roundTimer > m_roundTime / 2.0f ? -1.0f : 1.0f));
		Vector2 newPosition = m_startPosition + m_patrolOffset * posFactor;
		m_collisionComponent->SetPosition(newPosition);
	}
	else {
		Vector2 velocity = GetVelocity();
		velocity.y += ACCEL_FALL * dt;

		SetVelocity(velocity);
		SetPosition(GetPosition() + velocity * dt);
	}

	if (GetPosition().y > 600.f) m_isActive = false;
}