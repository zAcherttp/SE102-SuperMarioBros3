#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "Bush.h"

Bush::Bush(Vector2 position, int tileXcount, SpriteSheet* spriteSheet, int type)
	: Block(position, spriteSheet), m_type(type)
{
    m_tileXcount = tileXcount;
    m_tileYcount = 1;
	m_isStatic = true;
    m_isCollidable = false;
    m_isSolid = false;

    m_type = type;

	// update the collision box to match the size of the bush
	Vector2 curSize = m_collisionComponent->GetSize();

    Vector2 newSize;

    switch(type){
        case 0:
            m_tileXcount = tileXcount;
            m_tileYcount = 1;
            newSize = Vector2(16.f * tileXcount, 16.f);
            break;
        case 1: //small
            m_tileXcount = 4;
            m_tileYcount = 3;
            newSize = Vector2(64.f,48.f);
            break;
        case 2: //mid
            m_tileXcount = 5;
            m_tileYcount = 4;
            newSize = Vector2(80.f,64.f);
            break;
        case 3: //big
            m_tileXcount = 6;
            m_tileYcount = 8;
            newSize = Vector2(96.f,128.f);
            break;
        case 4: // big black bush
            m_tileXcount = 5;
            m_tileYcount = 6;
            newSize = Vector2(80.f,96.f);
            break;
        case 5: //small black bush
            m_tileXcount = 3;
            m_tileYcount = 3;
            newSize = Vector2(48.f,48.f);
            break;
        default:
            Log(__FUNCTION__, "Unknown bush type: " + std::to_string(m_type));
            newSize = Vector2(16.f, 16.f);
            break;
}

	m_collisionComponent->SetSize(newSize);
    // update the position as size is the center of the entity
	m_collisionComponent->SetPosition(position + Vector2(newSize.x / 2 , newSize.y / 2));
}


void Bush::Render(DirectX::SpriteBatch* spriteBatch)
{
    Vector2 size = m_collisionComponent->GetSize();
    Vector2 tileSize = Vector2(size.x / m_tileXcount, size.y / m_tileYcount);
    Vector2 pos = m_collisionComponent->GetPosition() - Vector2(size.x / 2 - tileSize.x / 2, size.y / 2 - tileSize.y / 2);

    switch(m_type) {
        case 0:
            for(int i = 0; i < m_tileXcount; i++)
            {
                    m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_LITTLE, pos + Vector2(i * tileSize.x, 0), 0.9f);
            }
            break;
        case 1: //small
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_OUTER_TOP_LEFT, pos + Vector2(16, 0), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_OUTER_TOP_RIGHT, pos + Vector2(32, 0), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_LEFT_SIDE	, pos + Vector2(16, 16), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_INNER_TOP_LEFT	, pos + Vector2(32, 16), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_OUTER_TOP_RIGHT, pos + Vector2(48, 16), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_OUTER_TOP_LEFT	, pos + Vector2(0, 32), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_INNER_TOP_RIGHT, pos + Vector2(16, 32), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(32, 32), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_RIGHT_SIDE	, pos + Vector2(48, 32), 0.9f);
            break;

        case 2: //mid
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_OUTER_TOP_LEFT, pos + Vector2(16, 0), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_OUTER_TOP_RIGHT, pos + Vector2(32, 0), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_OUTER_TOP_LEFT, pos + Vector2(0, 16), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_INNER_TOP_RIGHT, pos + Vector2(16, 16), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_RIGHT_SIDE, pos + Vector2(32, 16), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_OUTER_TOP_LEFT, pos + Vector2(48, 16), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_OUTER_TOP_RIGHT, pos + Vector2(64, 16), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_LEFT_SIDE, pos + Vector2(0, 32), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_INNER_TOP_LEFT, pos + Vector2(16, 32), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_INNER_TOP_RIGHT, pos + Vector2(32, 32), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(48, 32), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_RIGHT_SIDE, pos + Vector2(64, 32), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_LEFT_SIDE, pos + Vector2(0, 48), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(16, 48), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(32, 48), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(48, 48), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_RIGHT_SIDE, pos + Vector2(64, 48), 0.9f);

            break;
        case 3: 
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_OUTER_TOP_LEFT, pos + Vector2(32, 0),0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_OUTER_TOP_RIGHT, pos + Vector2(48, 0),0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_LEFT_SIDE, pos + Vector2(32, 16),0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_INNER_TOP_LEFT, pos + Vector2(48, 16), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_OUTER_TOP_RIGHT, pos + Vector2(64, 16), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_LEFT_SIDE, pos + Vector2(32, 32), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(48, 32), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_RIGHT_SIDE, pos + Vector2(64, 32), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_OUTER_TOP_LEFT, pos + Vector2(16, 48), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_INNER_TOP_RIGHT, pos + Vector2(32, 48), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(48, 48), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_RIGHT_SIDE, pos + Vector2(64, 48), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_OUTER_TOP_LEFT, pos + Vector2(0, 64), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_INNER_TOP_RIGHT, pos + Vector2(16, 64), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(32, 64), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(48, 64), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_RIGHT_SIDE, pos + Vector2(64, 64), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_LEFT_SIDE, pos + Vector2(0, 80), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(16, 80), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(32, 80), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(48, 80), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_RIGHT_SIDE, pos + Vector2(64, 80), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_LEFT_SIDE, pos + Vector2(0, 96), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(16, 96), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(32, 96), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(48, 96), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_INNER_TOP_LEFT, pos + Vector2(64, 96), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_OUTER_TOP_RIGHT, pos + Vector2(80, 96), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_LEFT_SIDE, pos + Vector2(0, 112), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(16, 112), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(32, 112), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(48, 112), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_MID, pos + Vector2(64, 112), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_BUSH_RIGHT_SIDE, pos + Vector2(80, 112), 0.9f);
            break;
        case 4: // big black
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_TOP_LEFT, pos + Vector2(16, 0),0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_TOP_RIGHT, pos + Vector2(32, 0),0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_LEFT_SIDE, pos + Vector2(16, 16),0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_RIGHT_SIDE, pos + Vector2(32, 16),0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_LEFT_SIDE, pos + Vector2(16, 32),0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_TOP_LEFT, pos + Vector2(32, 32), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_TOP_RIGHT, pos + Vector2(48, 32), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_LEFT_SIDE, pos + Vector2(16, 48), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_RIGHT_SIDE, pos + Vector2(48, 48), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_LEFT_SIDE, pos + Vector2(16, 64), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_TOP_LEFT, pos + Vector2(48, 64), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_TOP_RIGHT, pos + Vector2(64, 64), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_TOP_LEFT, pos + Vector2(0, 80), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_TOP_RIGHT, pos + Vector2(16, 80), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_RIGHT_SIDE, pos + Vector2(64, 80), 0.9f);
            break;
        case 5: //small black bush
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_TOP_LEFT, pos + Vector2(16, 0), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_TOP_RIGHT, pos + Vector2(32, 0), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_LEFT_SIDE, pos + Vector2(16, 16), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_RIGHT_SIDE, pos + Vector2(32, 16), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_TOP_LEFT, pos + Vector2(0, 32), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_TOP_RIGHT, pos + Vector2(16, 32), 0.9f);
            m_animator->Draw(spriteBatch, ID_SPRITE_END_BUSH_RIGHT_SIDE, pos + Vector2(32, 32), 0.9f);
            break;
        default:
            Log(__FUNCTION__, "Unknown bush type: " + std::to_string(m_type));
            break;
    }
}


void Bush::Update(float dt)
{
	//do nothing
    dt;
}
