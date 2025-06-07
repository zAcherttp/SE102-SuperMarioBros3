#pragma once
#include "Block.h"

class HitBox : public Block
{
public:
    HitBox(Vector2 position, Vector2 size, bool isSolid, SpriteSheet* spriteSheet);

	//static block so no update needed
	void Update(float dt) override;

	//here we use m_size to render too
    void Render(DirectX::SpriteBatch* spriteBatch) override;

    void OnCollision(const CollisionResult& event) override;
};

