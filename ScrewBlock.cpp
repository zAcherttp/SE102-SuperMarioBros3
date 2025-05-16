#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "ScrewBlock.h"
#include "Mario.h"

ScrewBlock::ScrewBlock(Vector2 position, Vector2 size, int countX, int countY, bool isSolid, SpriteSheet* spriteSheet, float depth, int color, bool isFloating)
	: Block(position, size, spriteSheet)
{
	m_tileXcount = countX;
	m_tileYcount = countY;
	m_isSolid = isSolid;
	m_isStatic = true;
    m_isCollidable = true;

    // Set the color and depth
	m_depth = depth;
	m_color = color;
	m_isFloating = isFloating;

	// update the collision box to match the size of the screw block
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x * countX, curSize.y * countY);
	m_collisionComponent->SetSize(newSize);
	// update the position as size is the center of the entity
	m_collisionComponent->SetPosition(position + Vector2(newSize.x / 2, newSize.y / 2));

    m_collisionGroup = CollisionGroup::NONSOLID;
}

int ScrewBlock::GetSpriteID(int color, ScrewBlockPosition position) {
	// Using new convention: 25[color][position]
	// color: 0=white, 1=pink, 2=blue, 3=green
	// position: 0-8 representing position in 3x3 grid

	// Map ScrewBlockPosition enum to grid positions
	int positionValue;
	switch (position) {
	case TOP_LEFT:     positionValue = 0; break;
	case TOP_MID:      positionValue = 1; break;
	case TOP_RIGHT:    positionValue = 2; break;
	case MID_LEFT:     positionValue = 3; break;
	case MID_MID:      positionValue = 4; break;
	case MID_RIGHT:    positionValue = 5; break;
	case BOT_LEFT:     positionValue = 6; break;
	case BOT_MID:      positionValue = 7; break;
	case BOT_RIGHT:    positionValue = 8; break;
	default:           positionValue = 0; break;
	}

	return 2500 + (color * 10) + positionValue;
}

void ScrewBlock::Render(DirectX::SpriteBatch* spriteBatch)
{
	Vector2 size = m_collisionComponent->GetSize();
	Vector2 tileSize = Vector2(size.x / m_tileXcount, size.y / m_tileYcount);
	Vector2 pos = m_collisionComponent->GetPosition() - Vector2(size.x / 2 - tileSize.x / 2, size.y / 2 - tileSize.y / 2);

	// Top row - always draw
	m_animator->Draw(spriteBatch, GetSpriteID(m_color, TOP_LEFT), pos, m_depth);
	for (int i = 1; i < m_tileXcount - 1; i++)
	{
		m_animator->Draw(spriteBatch, GetSpriteID(m_color, TOP_MID), pos + Vector2(i * tileSize.x, 0), m_depth);
	}
	m_animator->Draw(spriteBatch, GetSpriteID(m_color, TOP_RIGHT), pos + Vector2((m_tileXcount - 1) * tileSize.x, 0), m_depth);

	// Middle rows - only if there are at least 3 rows
	if (m_tileYcount > 2) {
		for (int j = 1; j < m_tileYcount - 1; j++)
		{
			m_animator->Draw(spriteBatch, GetSpriteID(m_color, MID_LEFT), pos + Vector2(0, j * tileSize.y), m_depth);
			for (int i = 1; i < m_tileXcount - 1; i++)
			{
				m_animator->Draw(spriteBatch, GetSpriteID(m_color, MID_MID), pos + Vector2(i * tileSize.x, j * tileSize.y), m_depth);
			}
			m_animator->Draw(spriteBatch, GetSpriteID(m_color, MID_RIGHT), pos + Vector2((m_tileXcount - 1) * tileSize.x, j * tileSize.y), m_depth);
		}
	}

	// Bottom row - draw if there's more than 1 row
	if (m_tileYcount > 1) {
		m_animator->Draw(spriteBatch, GetSpriteID(m_color, BOT_LEFT), pos + Vector2(0, (m_tileYcount - 1) * tileSize.y), m_depth);
		for (int i = 1; i < m_tileXcount - 1; i++)
		{
			m_animator->Draw(spriteBatch, GetSpriteID(m_color, BOT_MID), pos + Vector2(i * tileSize.x, (m_tileYcount - 1) * tileSize.y), m_depth);
		}
		m_animator->Draw(spriteBatch, GetSpriteID(m_color, BOT_RIGHT), pos + Vector2((m_tileXcount - 1) * tileSize.x, (m_tileYcount - 1) * tileSize.y), m_depth);
	}

	// Side shade - always draw
	// Draw shade_bot_left at the highest position (y=0)
	m_animator->Draw(spriteBatch, ID_SPRITE_SCREW_BLOCK_SHADE_BOT_LEFT,
		pos + Vector2(m_tileXcount * tileSize.x, 0), m_depth);

	// Draw shade_left for all rows below the top
	for (int j = 1; j < m_tileYcount; j++) {
		m_animator->Draw(spriteBatch, ID_SPRITE_SCREW_BLOCK_SHADE_LEFT,
			pos + Vector2(m_tileXcount * tileSize.x, j * tileSize.y), m_depth);
	}

	// If floating, add bottom shade
	if (m_isFloating) {
		// Draw shade_top_right at the left corner of bottom shade
		m_animator->Draw(spriteBatch, ID_SPRITE_SCREW_BLOCK_SHADE_TOP_RIGHT,
			pos + Vector2(0, m_tileYcount * tileSize.y), m_depth);

		// Draw shade_top across the bottom
		for (int i = 1; i < m_tileXcount; i++) {
			m_animator->Draw(spriteBatch, ID_SPRITE_SCREW_BLOCK_SHADE_TOP,
				pos + Vector2(i * tileSize.x, m_tileYcount * tileSize.y), m_depth);
		}

		// Draw shade_top_left at the right corner of bottom shade
		m_animator->Draw(spriteBatch, ID_SPRITE_SCREW_BLOCK_SHADE_TOP_LEFT,
			pos + Vector2(m_tileXcount * tileSize.x, m_tileYcount * tileSize.y), m_depth);
	}
}

void ScrewBlock::Update(float dt)
{
	//do nothing
	dt;
	SetPosition(GetPosition() + GetVelocity() * dt);
}
