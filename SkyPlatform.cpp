#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "SkyPlatform.h"

SkyPlatform::SkyPlatform(Vector2 position, Vector2 size, int countX, bool isSolid, SpriteSheet* spriteSheet)
	: Block(position, size, spriteSheet)
{
	m_tileXcount = countX;
	m_tileYcount = 1; 
	m_isSolid = isSolid;
	m_isStatic = true;

	// update the collision box to match the size of the ground
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x * m_tileXcount, curSize.y);
	m_collisionComponent->SetSize(newSize);
	// update the position as size is the center of the entity
	m_collisionComponent->SetPosition(position + Vector2(newSize.x / 2 , newSize.y / 2));
     SetAnimation(ID_ANIM_BRICK, true);
  

}

void SkyPlatform::Render(DirectX::SpriteBatch* spriteBatch)
{
    Vector2 size = m_collisionComponent->GetSize();
    Vector2 tileSize = Vector2(size.x / m_tileXcount, size.y / m_tileYcount);
    Vector2 pos = m_collisionComponent->GetPosition() - Vector2(size.x / 2 - tileSize.x / 2, size.y / 2 - tileSize.y / 2);

    for (int i = 0; i < m_tileXcount; i++)
    {
        m_animator->Draw(spriteBatch, ID_SPRITE_SKY_PLATFORM, pos + Vector2(i * tileSize.x, 0), 0.5f); 
    }

}


void SkyPlatform::Update(float dt)
{
    // do nothing
}
