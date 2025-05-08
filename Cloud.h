#pragma once
#include "Block.h"

class Cloud : public Block
{
public:
    Cloud(Vector2 position, int tileXcount = 0 , SpriteSheet* spriteSheet = nullptr, int type = 0);

	//static block so no update needed
	void Update(float dt) override;

	//here we use m_size to render too
    void Render(DirectX::SpriteBatch* spriteBatch) override;

private:
	int m_tileXcount;
	int m_tileYcount;

    int m_type; // 0 = little, 1 = small, 2 = mid, 3 = big
};

