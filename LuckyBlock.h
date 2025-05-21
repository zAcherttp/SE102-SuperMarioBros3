#pragma once
#include "Block.h"
#include "HiddenButton.h"

enum class ItemType
{
	MUSHROOM,
	LEAF
};
class LuckyBlock : public Block
{
public:
	LuckyBlock(Vector2 position, Vector2 size, bool isSolid, SpriteSheet* spriteSheet, bool isSpecial, bool isMimic = false);

	//static block so no update needed
	void Update(float dt) override;

	//here we use m_size to render too
	void Render(DirectX::SpriteBatch* spriteBatch) override;

	void Bump() override;
	void OnCollision(const CollisionResult& event) override;

	void SpawnReward();
	void SpawnCoin();
	void SpawnPowerUp();
	void SpawnButton();

private:
	int m_tileXcount = 1;
	int m_tileYcount = 1;

	HiddenButton* m_hiddenButton = nullptr;

	bool m_isSpecial = false;
	bool m_isMimic = false;

	Vector2 m_origin;
	bool m_isClaiming;

	float m_claimCoinTimer;
	bool m_collectedCoin;
};
