#pragma once
#include "Block.h"

class Ground : public Block
{
public:
    Ground(Vector2 position, Vector2 size, SpriteSheet* spriteSheet);

	//static block so no update needed
	void Update(float dt) override = 0;

	//here we use m_size to render too
    void Render(DirectX::SpriteBatch* spriteBatch) override;

};

