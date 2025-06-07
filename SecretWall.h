#pragma once
#include "Block.h"

class SecretWall : public Block
{
public:
    SecretWall(Vector2 position, Vector2 size, int countX, int countY, SpriteSheet* spriteSheet);

	//static block so no update needed
	void Update(float dt) override;

	//here we use m_size to render too
    void Render(DirectX::SpriteBatch* spriteBatch) override;

private:
	int m_tileXcount;
	int m_tileYcount;
};

