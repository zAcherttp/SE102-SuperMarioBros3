#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "Brick.h"

Brick::Brick(Vector2 position, Vector2 size, bool isSolid, SpriteSheet* spriteSheet)
	: Block(position, size, spriteSheet)
{
	m_tileXcount = 1;
	m_tileYcount = 1; // Brick is always 1 tile high
	m_isSolid = isSolid;
	m_isStatic = true;

	// update the collision box to match the size of the ground
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x * m_tileXcount, curSize.y);
	m_collisionComponent->SetSize(newSize);
	// update the position as size is the center of the entity
	m_collisionComponent->SetPosition(position + Vector2(newSize.x / 2 , newSize.y / 2));
     SetAnimation(ID_ANIM_BRICK, true);
    // Log(LOG_INFO, "Setted Animation for: " + std::to_string(position.x) + ", " + std::to_string(position.y));


}

void Brick::Render(DirectX::SpriteBatch* spriteBatch)
{
    Vector2 size = m_collisionComponent->GetSize();
    Vector2 tileSize = Vector2(size.x / m_tileXcount, size.y / m_tileYcount);
    Vector2 pos = m_collisionComponent->GetPosition() - Vector2(size.x / 2 - tileSize.x / 2, size.y / 2 - tileSize.y / 2);

    m_animator->Draw(spriteBatch, pos, 0.5f); 

}


void Brick::Update(float dt)
{
    m_animator->Update(dt);

        if (m_animator->HasAnimation(ID_ANIM_BRICK)) {
        // Log(LOG_INFO,"Animation ID_ANIM_BRICK exists");
    } else {
        // Log(LOG_WARN,"Animation ID_ANIM_BRICK does NOT exist");

    }


}
