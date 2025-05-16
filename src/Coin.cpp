#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "Coin.h"

Coin::Coin(Vector2 position, Vector2 size, bool isSolid, SpriteSheet* spriteSheet)
	: Block(position, size, spriteSheet)
{
	m_tileXcount = 1;
	m_tileYcount = 1; // Coin is always 1 tile high
	m_isSolid = isSolid;
	m_isStatic = true;

	// update the collision box to match the size of the ground
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x * m_tileXcount, curSize.y * m_tileYcount);
	m_collisionComponent->SetSize(newSize);
	// update the position as size is the center of the entity
	m_collisionComponent->SetPosition(position + Vector2(newSize.x / 2 , newSize.y / 2));
     SetAnimation(ID_ANIM_COIN, true);
    // Log(LOG_INFO, "Set Animation for: " + std::to_string(position.x) + ", " + std::to_string(position.y));


}

void Coin::Render(DirectX::SpriteBatch* spriteBatch)
{
    Vector2 size = m_collisionComponent->GetSize();
    Vector2 tileSize = Vector2(size.x / m_tileXcount, size.y / m_tileYcount);
    Vector2 pos = m_collisionComponent->GetPosition() - Vector2(size.x / 2 - tileSize.x / 2, size.y / 2 - tileSize.y / 2);

    m_animator->Draw(spriteBatch, pos , 0.5f); 
    
}


void Coin::Update(float dt)
{
    m_animator->Update(dt);

}
