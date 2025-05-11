#pragma once
#include "Block.h"

class LuckyBlock : public Block
{
public:
    LuckyBlock(Vector2 position, Vector2 size, bool isSolid, SpriteSheet* spriteSheet);

	//static block so no update needed
	void Update(float dt) override;

	//here we use m_size to render too
    void Render(DirectX::SpriteBatch* spriteBatch) override;

	void Bump() override;
	void OnCollision(const CollisionResult& event) override;
private:
	int m_tileXcount = 1;
	int m_tileYcount = 1;

	Vector2 m_origin;
	bool m_isClaiming;
};
