#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "Ground.h"

Ground::Ground(Vector2 position, Vector2 size, int countX, int countY, bool isSolid, SpriteSheet* spriteSheet)
	: Block(position, size, spriteSheet)
{
	m_tileXcount = countX;
	m_tileYcount = countY;
	m_isSolid = isSolid;
	m_isStatic = true;

	// update the collision box to match the size of the ground
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x * countX, curSize.y * countY);
	m_collisionComponent->SetSize(newSize);
	// update the position as size is the center of the entity
	m_collisionComponent->SetPosition(position + Vector2(newSize.x / 2 - newSize.y / 2, 0));
}

void Ground::Render(DirectX::SpriteBatch* spriteBatch)
{
	Vector2 size = m_collisionComponent->GetSize();
	Vector2 pos = m_collisionComponent->GetPosition() - Vector2(size.x / 2 - size.y / 2, 0);
	Vector2 tileSize = Vector2(size.x / m_tileXcount, size.y / m_tileYcount);
	
	// draw top left corner of the tile, then the middle, then the top right corner
	m_animator->Draw(spriteBatch, ID_SPRITE_GROUND_TOP_LEFT, pos);
	for(int i = 1; i < m_tileXcount - 1; i++)
	{
		m_animator->Draw(spriteBatch, ID_SPRITE_GROUND_TOP_MID, pos + Vector2(i * tileSize.x, 0));
	}
	m_animator->Draw(spriteBatch, ID_SPRITE_GROUND_TOP_RIGHT, pos + Vector2((m_tileXcount - 1) * tileSize.x, 0));
}


void Ground::Update(float dt)
{
	//do nothing
}
