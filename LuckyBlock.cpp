#include "pch.h"
#include "AssetIDs.h"
#include "Block.h"
#include "Collision.h"
#include "Debug.h"
#include "EffectManager.h"
#include "FireLeaf.h"
#include "Game.h"
#include "LuckyBlock.h"
#include "Mario.h"
#include "MarioPowerUpStates.h"
#include "Mushroom.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteSheet.h"
#include "World.h"
#include <string>

LuckyBlock::LuckyBlock(Vector2 position, Vector2 size, bool isSolid, SpriteSheet* spriteSheet, bool isSpecial)
	: Block(position, size, spriteSheet)
{
	m_tileXcount = 1;
	m_tileYcount = 1; // LuckyBlock is always 1 tile high
	m_isSolid = isSolid;
	m_isStatic = true;
	m_isClaiming = false;
	m_isClaimed = false;
	m_isCollidable = true;
	m_isActive = true;
	m_isCollidable = true;
	m_isSpecial = isSpecial;
	m_claimCoinTimer = 0.0f;
	m_collectedCoin = false;

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

	m_animator->Draw(spriteBatch, pos, 0.0f);

}

void LuckyBlock::Update(float dt)
{
	if (!m_collectedCoin && m_isClaimed) m_claimCoinTimer += dt;
	if (m_claimCoinTimer > 0.7f && !m_collectedCoin) {
		m_collectedCoin = true;

		Game::GetInstance()->AddScore(100);

		Log(LOG_INFO, "Collected Coin from LuckyBlock at: " + std::to_string(m_collisionComponent->GetPosition().x) + ", " + std::to_string(m_collisionComponent->GetPosition().y));

	}
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
			SpawnReward();

		}
		else {
			SetVelocity(vel);
			SetPosition(pos);
		}
	}

	m_animator->Update(dt);
}

void LuckyBlock::Bump() {
	if (m_isClaimed || m_isClaiming) return;
	m_isClaiming = true;
	m_isClaimed = false;
	SetAnimId(ID_ANIM_LUCKY_BLOCK_CLAIMED);
	SetVelocity(Vector2(0, -240.f));



	//Log(LOG_INFO, "Bumped LuckyBlock at: " + std::to_string(m_collisionComponent->GetPosition().x) + ", " + std::to_string(m_collisionComponent->GetPosition().y));
}

void LuckyBlock::OnCollision(const CollisionResult& event)
{
	Mario* mario = dynamic_cast<Mario*>(event.collidedWith);

	//Log(LOG_INFO, "Collision with LuckyBlock at: " + std::to_string(m_collisionComponent->GetPosition().x) + ", " + std::to_string(m_collisionComponent->GetPosition().y) + " with: " + std::to_string(event.collidedWith->GetPosition().x) + ", " + std::to_string(event.collidedWith->GetPosition().y));

	if (event.contactNormal.y < 0 && mario && !m_isClaimed)
	{
		Vector2 vel = mario->GetVelocity();
		vel.y = -vel.y;
		mario->SetVelocity(vel);
	}
}

void LuckyBlock::SpawnReward()
{
	if (m_isSpecial) {
		SpawnPowerUp();
	}
	else {
		SpawnCoin();
	}
}

void LuckyBlock::SpawnCoin()
{
	EffectManager::GetInstance()->CreateCoinEffect(GetPosition());
}

void LuckyBlock::SpawnPowerUp()
{
	Mario* mario = dynamic_cast<Mario*>(World::GetInstance()->GetPlayer());
	ItemType powerupType;

	if (mario->GetPowerUpState() == PowerUpType::SMALL) {
		powerupType = ItemType::MUSHROOM;
	}
	else {
		powerupType = ItemType::LEAF;
	}

	Vector2 spawnPosition = GetPosition();

	if (powerupType == ItemType::MUSHROOM) {
		World::GetInstance()->AddEntity(
			new Mushroom(spawnPosition, Vector2(16, 16), Game::GetInstance()->GetSpriteSheet())
		);
	}
	else if (powerupType == ItemType::LEAF) {
		World::GetInstance()->AddEntity(
			new Leaf(spawnPosition, Vector2(16, 16), Game::GetInstance()->GetSpriteSheet())
		);
	}
}
