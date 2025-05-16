#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "Pipe.h"
#include "Mario.h"

Pipe::Pipe(Vector2 position, Vector2 size, int countX, int countY, bool isSolid, SpriteSheet* spriteSheet, bool hasHead)
	: Block(position, size, spriteSheet)
{
	m_tileXcount = countX;
	m_tileYcount = countY;
	m_isSolid = isSolid;
	m_isStatic = true;
    m_hasHead = hasHead;

	// update the collision box to match the size of the screw block
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x * countX, curSize.y * countY);
	m_collisionComponent->SetSize(newSize);
	// update the position as size is the center of the entity
	m_collisionComponent->SetPosition(position + Vector2(newSize.x / 2 , newSize.y / 2));
}

void Pipe::Render(DirectX::SpriteBatch* spriteBatch)
{
    Vector2 size = m_collisionComponent->GetSize();
    Vector2 tileSize = Vector2(size.x / m_tileXcount, size.y / m_tileYcount);
    Vector2 pos = m_collisionComponent->GetPosition() - Vector2(size.x / 2 - tileSize.x / 2, size.y / 2 - tileSize.y / 2);

    if(m_hasHead)
    {
        m_animator->Draw(spriteBatch, ID_SPRITE_PIPE_TOP_LEFT, pos + Vector2(0, 0), 0.0f); // Draw the head above the pipe
        m_animator->Draw(spriteBatch, ID_SPRITE_PIPE_TOP_RIGHT, pos + Vector2(16, 0), 0.0f); // Draw the head above the pipe
    }
    for(int j = m_hasHead ? 1 : 0; j < m_tileYcount; j++)
    {
        m_animator->Draw(spriteBatch, ID_SPRITE_PIPE_BOT_LEFT, pos + Vector2(0, j * tileSize.y), 0.0f);
        m_animator->Draw(spriteBatch, ID_SPRITE_PIPE_BOT_RIGHT, pos + Vector2(16, j * tileSize.y), 0.0f);
    }
    
}

void Pipe::Update(float dt)
{
    //do nothing
    dt;
}
