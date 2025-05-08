#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "Cloud.h"

Cloud::Cloud(Vector2 position, int count, SpriteSheet* spriteSheet, int type)
	: Block(position, spriteSheet), m_type(type)
{

	m_isStatic = true;
    m_isCollidable = false;
    m_isSolid = false;

    m_type = type;

	// update the collision box to match the size of the bush
	Vector2 curSize = m_collisionComponent->GetSize();

    Vector2 newSize;

    switch(type){
        case 0:
                m_tileXcount = count + 2;
                m_tileYcount = 2;
                newSize = Vector2(16 * m_tileXcount , 16 * m_tileYcount);
            break;
        case 1:
        case 2: 
        case 3: 
            m_tileXcount = 1;
            m_tileYcount = 1;
            newSize = Vector2(16,16);
            break;
        default:
            Log(__FUNCTION__, "Unknown cloud type: " + std::to_string(m_type));
            newSize = Vector2(16, 16);
            break;
}

	m_collisionComponent->SetSize(newSize);
    // update the position as size is the center of the entity
	m_collisionComponent->SetPosition(position + Vector2(newSize.x / 2 , newSize.y / 2));
}


void Cloud::Render(DirectX::SpriteBatch* spriteBatch)
{
    Vector2 size = m_collisionComponent->GetSize();
    Vector2 tileSize = Vector2(size.x / m_tileXcount, size.y / m_tileYcount);
    Vector2 pos = m_collisionComponent->GetPosition() - Vector2(size.x / 2 - tileSize.x / 2, size.y / 2 - tileSize.y / 2);

    switch(m_type) {
        case 0:
            m_animator->Draw(spriteBatch, ID_SPRITE_CLOUD_TOP_LEFT, pos + Vector2(0, 0));
            m_animator->Draw(spriteBatch, ID_SPRITE_CLOUD_BOT_LEFT, pos + Vector2(0, 16)); 
            for(int i = 1; i < m_tileXcount -1; i++)
            {     
                    m_animator->Draw(spriteBatch, ID_SPRITE_CLOUD_TOP_MID, pos + Vector2(i * tileSize.x, 0)); 
                    m_animator->Draw(spriteBatch, ID_SPRITE_CLOUD_BOT_MID, pos + Vector2(i * tileSize.x, 16));
            }
            m_animator->Draw(spriteBatch, ID_SPRITE_CLOUD_TOP_RIGHT, pos + Vector2((m_tileXcount -1) * tileSize.x, 0));
            m_animator->Draw(spriteBatch, ID_SPRITE_CLOUD_BOT_RIGHT, pos + Vector2((m_tileXcount -1) * tileSize.x, 16));
            break;
        case 1: //small
          m_animator->Draw(spriteBatch, ID_SPRITE_CLOUD_STAR, pos);
            break;
        case 2: //mid
          m_animator->Draw(spriteBatch, ID_SPRITE_CLOUD_MUSHROOM, pos);
            break;
        case 3: 
            m_animator->Draw(spriteBatch, ID_SPRITE_CLOUD_FLOWER, pos);
            break;

        default:
            Log(__FUNCTION__, "Unknown cloud type: " + std::to_string(m_type));
            break;
    }
}


void Cloud::Update(float dt)
{
	//do nothing
}
