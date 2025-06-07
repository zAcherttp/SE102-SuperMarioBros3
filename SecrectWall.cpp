#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "SecretWall.h"

SecretWall::SecretWall(Vector2 position, Vector2 size, int countX, int countY, SpriteSheet* spriteSheet)
	: Block(position, size, spriteSheet)
{
	m_tileXcount = countX;
	m_tileYcount = countY;
	m_isSolid = true;
	m_isStatic = true;

	// update the collision box to match the size of the ground
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x * countX, curSize.y * countY);
	m_collisionComponent->SetSize(newSize);
	// update the position as size is the center of the entity
	m_collisionComponent->SetPosition(position + Vector2(newSize.x / 2 , newSize.y / 2));
}

void SecretWall::Render(DirectX::SpriteBatch* spriteBatch)
{
    Vector2 size = m_collisionComponent->GetSize();
    Vector2 tileSize = Vector2(size.x / m_tileXcount, size.y / m_tileYcount);
    Vector2 pos = m_collisionComponent->GetPosition() - Vector2(size.x / 2 - tileSize.x / 2, size.y / 2 - tileSize.y / 2);

	for(int i = 0; i < m_tileXcount; i++)
    {
        for(int j = 0; j < m_tileYcount; j++)
        {
            m_animator->Draw(spriteBatch, ID_SPRITE_SECRET_WALL, pos + Vector2(i * tileSize.x, j * tileSize.y), 1.0f);
        }
    }
}


void SecretWall::Update(float dt)
{
	//do nothing
    dt;
}
