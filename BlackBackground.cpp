#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "BlackBackground.h"

BlackBackground::BlackBackground(Vector2 position, Vector2 size, int tileXcount, int tileYcount, SpriteSheet* spriteSheet, int type, float depth)
	: Block(position,size, spriteSheet), m_type(type), m_tileXcount(tileXcount), m_tileYcount(tileYcount)
{

	m_isStatic = true;
    m_isCollidable = false;
    m_isSolid = false;
    m_type = type;
    m_depth = depth;

	// update the collision box to match the size of the bush
	Vector2 curSize = m_collisionComponent->GetSize();

	Vector2 newSize = Vector2(curSize.x * m_tileXcount, curSize.y * m_tileYcount);
	m_collisionComponent->SetSize(newSize);
	// update the position as size is the center of the entity
	m_collisionComponent->SetPosition(position + Vector2(newSize.x / 2 , newSize.y / 2));

}


void BlackBackground::Render(DirectX::SpriteBatch* spriteBatch)
{
    Vector2 size = m_collisionComponent->GetSize();
    Vector2 tileSize = Vector2(size.x / m_tileXcount, size.y / m_tileYcount);
    Vector2 pos = m_collisionComponent->GetPosition() - Vector2(size.x / 2 - tileSize.x / 2, size.y / 2 - tileSize.y / 2);

    switch(m_type)
    {
        case 0:
        for(int i = 0; i < m_tileXcount; i++)
        {
            for (int j = 0; j < m_tileYcount; j++)
            {
              m_animator->Draw(spriteBatch, ID_SPRITE_BLACK_BACKGROUND_SOLID, pos + Vector2(i * tileSize.x, j * tileSize.y), m_depth);
            }
        }
        break;
        case 1:
        for(int i = 0; i < m_tileXcount; i++)
        {
            for (int j = 0; j < m_tileYcount; j++)
            {
              m_animator->Draw(spriteBatch, ID_SPRITE_BLACK_BACKGROUND_BARRIER, pos + Vector2(i * tileSize.x, j * tileSize.y), m_depth  );
            }
        }
    }
}


void BlackBackground::Update(float dt)
{
	//do nothing
    dt;
}
