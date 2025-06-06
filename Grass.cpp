#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "Grass.h"

Grass::Grass(Vector2 position, Vector2 size, int countX, int countY, bool isSolid, SpriteSheet* spriteSheet)
	: Block(position, size, spriteSheet)
{
	m_tileXcount = countX;
	m_tileYcount = countY;
	m_isSolid = isSolid;
	m_isStatic = true;

	// update the collision box to match the size of the grass
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x * countX, curSize.y * countY);
	m_collisionComponent->SetSize(newSize);
	// update the position as size is the center of the entity
	m_collisionComponent->SetPosition(position + Vector2(newSize.x / 2 , newSize.y / 2));
}

void Grass::Render(DirectX::SpriteBatch* spriteBatch)
{
    Vector2 size = m_collisionComponent->GetSize();
    Vector2 tileSize = Vector2(size.x / m_tileXcount, size.y / m_tileYcount);
    Vector2 pos = m_collisionComponent->GetPosition() - Vector2(size.x / 2 - tileSize.x / 2, size.y / 2 - tileSize.y / 2);

    // draw top left corner of the tile, then the middle, then the top right corner
    m_animator->Draw(spriteBatch, ID_SPRITE_GRASS_TOP_LEFT, pos, 1.0f);
    for (int i = 1; i < m_tileXcount - 1; i++)
    {
        m_animator->Draw(spriteBatch, ID_SPRITE_GRASS_TOP_MID, pos + Vector2(i * tileSize.x, 0), 1.0f);
    }
    m_animator->Draw(spriteBatch, ID_SPRITE_GRASS_TOP_RIGHT, pos + Vector2((m_tileXcount - 1) * tileSize.x, 0), 1.0f);
    // draw the middle of the tile
    for (int j = 1; j < m_tileYcount; j++)
    {
        m_animator->Draw(spriteBatch, ID_SPRITE_GRASS_BOT_LEFT, pos + Vector2(0, j * tileSize.y), 1.0f);
        for (int i = 1; i < m_tileXcount - 1; i++)
        {
            m_animator->Draw(spriteBatch, ID_SPRITE_GRASS_BOT_MID, pos + Vector2(i * tileSize.x, j * tileSize.y), 1.0f);
        }
        m_animator->Draw(spriteBatch, ID_SPRITE_GRASS_BOT_RIGHT, pos + Vector2((m_tileXcount - 1) * tileSize.x, j * tileSize.y), 1.0f);
    }
}


void Grass::Update(float dt)
{
	//do nothing
    dt;
}
