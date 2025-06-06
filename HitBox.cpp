#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "HitBox.h"
#include "Collision.h"
#include "Mario.h"
#include "RedTroopas.h"

HitBox::HitBox(Vector2 position, Vector2 size, bool isSolid, SpriteSheet* spriteSheet)
    : Block(position, size, spriteSheet)
{
    m_isSolid = isSolid;
    m_isStatic = true;

    // update the collision box to match the size of the hitbox
    Vector2 curSize = m_collisionComponent->GetSize();
    Vector2 newSize = Vector2(curSize.x, curSize.y * size.y);
    m_collisionComponent->SetSize(newSize);
    // update the position as size is the center of the entity
    m_collisionComponent->SetPosition(position + Vector2(newSize.x / 2 , newSize.y / 2));
}

void HitBox::Update(float dt)
{
    // HitBox is static, no update needed
    dt; // Avoid unused parameter warning
}
void HitBox::Render(DirectX::SpriteBatch* spriteBatch)
{
    Vector2 size = m_collisionComponent->GetSize();
    Vector2 pos = m_collisionComponent->GetPosition() - Vector2(size.x / 2, size.y / 2);

    m_animator->Draw(spriteBatch, pos, 0.0f);
}

void HitBox::OnCollision(const CollisionResult& event)
{
    Mario* mario = dynamic_cast<Mario*>(event.collidedWith);
    RedTroopas* redTroopas = dynamic_cast<RedTroopas*>(event.collidedWith);
    if(mario) return;
    if(redTroopas && event.contactNormal.x <= 0) return;
}