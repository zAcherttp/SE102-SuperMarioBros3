#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "Brick.h"
#include "Mario.h"

Brick::Brick(Vector2 position, Vector2 size, bool isSolid, SpriteSheet* spriteSheet)
	: Block(position, size, spriteSheet)
    , m_state(BrickState::BRICK)
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
    if(m_state == BrickState::COIN)
    {
        m_timer += dt;
        if (m_timer > 11.0f)
        {
            TransformToBrick();
        }
    }
    Entity::Update(dt);
}

void Brick::Break()
{
    EffectManager::GetInstance()->CreateBrickEffect(GetPosition());
    m_isActive = false;
    m_visible = false;
}

void Brick::TransformToCoin()
{
    m_state = BrickState::COIN;
    m_animator->SetAnimation(ID_ANIM_BRICK_COIN, false);
    m_isCollectible = true;
    m_isSolid = false;
}

void Brick::TransformToBrick()
{
    m_state = BrickState::BRICK;
    m_animator->SetAnimation(ID_ANIM_BRICK, true);
    m_isCollectible = false;
    m_isSolid = true;
}

void Brick::OnCollision(const CollisionResult& event)
{
    Troopa* troopa = dynamic_cast<Troopa*>(event.collidedWith);
    Mario* mario = dynamic_cast<Mario*>(event.collidedWith);
    if (m_state == BrickState::COIN && mario)
    {
        m_isActive = false;
        m_visible = false;
        return;
    }

    if(m_state == BrickState::BRICK && troopa && troopa->GetState() == TroopaState::SHELL_SLIDE)
    {
        // If a Troopa in shell state collides with the brick, it should break
        Break();
        return;
    }
}