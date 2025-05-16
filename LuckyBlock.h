#pragma once
#include "Block.h"

enum class PowerUpType
{
    MUSHROOM,
    LEAF
};
class LuckyBlock : public Block
{
public:
    LuckyBlock(Vector2 position, Vector2 size, bool isSolid, SpriteSheet* spriteSheet, bool isSpecial);

	//static block so no update needed
	void Update(float dt) override;

	//here we use m_size to render too
    void Render(DirectX::SpriteBatch* spriteBatch) override;

	void Bump() override;
	void OnCollision(const CollisionResult& event) override;

	void SpawnReward();
	void SpawnCoin();
	void SpawnPowerUp();
private:
	int m_tileXcount = 1;
	int m_tileYcount = 1;

	bool m_isSpecial = false;

	Vector2 m_origin;
	bool m_isClaiming;
};
