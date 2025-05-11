#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "LuckyBlock.h"
#include "GameConfig.h"
#include "Mario.h"

LuckyBlock::LuckyBlock(Vector2 position, Vector2 size, bool isSolid, SpriteSheet* spriteSheet)
	: Block(position, size, spriteSheet)
{
	m_tileXcount = 1;
	m_tileYcount = 1; // LuckyBlock is always 1 tile high
	m_isSolid = true;
	m_isStatic = false;
	m_isClaiming = false;
	m_isClaimed = false;


	// update the collision box to match the size of the ground
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x * m_tileXcount, curSize.y);
	m_collisionComponent->SetSize(newSize);
	// update the position as size is the center of the entity
	Vector2 newPos = position + Vector2(newSize.x / 2, newSize.y / 2);
	m_collisionComponent->SetPosition(newPos);
	m_origin = newPos;
    SetAnimation(ID_ANIM_LUCKY_BLOCK, true);
    // Log(LOG_INFO, "Set Animation for: " + std::to_string(position.x) + ", " + std::to_string(position.y));


}

void LuckyBlock::Render(DirectX::SpriteBatch* spriteBatch)
{
    Vector2 size = m_collisionComponent->GetSize();
    Vector2 tileSize = Vector2(size.x / m_tileXcount, size.y / m_tileYcount);
    Vector2 pos = m_collisionComponent->GetPosition() - Vector2(size.x / 2 - tileSize.x / 2, size.y / 2 - tileSize.y / 2);

    m_animator->Draw(spriteBatch, pos , 0.5f); 
    
}

void LuckyBlock::Update(float dt)
{
	if (m_isClaiming) {
		// set anim id to opened state
		//SetAnimId()

		float gravity = 3200.f;

		Vector2 vel = GetVelocity();
		Vector2 pos = GetPosition();
		vel.y += gravity * dt;
		pos.y += vel.y * dt;

		// if vel.y > 0 and pos is close to origin, stop the block and set it to claimed
		if (vel.y > 0 && m_origin.y - GetPosition().y < 0.1f) {
			vel.y = 0;
			m_isClaiming = false;
			m_isClaimed = true;
			SetVelocity(vel);
			SetPosition(m_origin);

		} else {
			SetVelocity(vel);
			SetPosition(pos);
		}
	}

    m_animator->Update(dt);
}

void LuckyBlock::Bump() {
	if(m_isClaimed || m_isClaiming) return;
	m_isClaiming = true;
	m_isClaimed = false;
	SetVelocity(Vector2(0, -240.f));

	//Log(LOG_INFO, "Bumped LuckyBlock at: " + std::to_string(m_collisionComponent->GetPosition().x) + ", " + std::to_string(m_collisionComponent->GetPosition().y));
}

void LuckyBlock::OnCollision(const CollisionResult& event)
{
	Mario* mario = dynamic_cast<Mario*>(event.collidedWith);

	//Log(LOG_INFO, "Collision with LuckyBlock at: " + std::to_string(m_collisionComponent->GetPosition().x) + ", " + std::to_string(m_collisionComponent->GetPosition().y) + " with: " + std::to_string(event.collidedWith->GetPosition().x) + ", " + std::to_string(event.collidedWith->GetPosition().y));

	if (event.contactNormal.y < 0 && mario)
	{
		Vector2 vel = mario->GetVelocity();
		vel.y = -vel.y;
		mario->SetVelocity(vel);
	}
}
