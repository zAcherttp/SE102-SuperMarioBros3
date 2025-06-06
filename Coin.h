#pragma once
#include "Block.h"

class Coin : public Block
{
public:
    Coin(Vector2 position, Vector2 size, bool isSolid, SpriteSheet* spriteSheet);

	//static block so no update needed
	void Update(float dt) override;

	//here we use m_size to render too
    void Render(DirectX::SpriteBatch* spriteBatch) override;

	void OnCollision(const CollisionResult& event) override;

private:
	int m_tileXcount = 1;
	int m_tileYcount = 1;
};
