#pragma once
#include "Block.h"

class BlackBackground : public Block
{
public:
    BlackBackground(Vector2 position, Vector2 size ,int tileXcount, int tileYcount, SpriteSheet* spriteSheet = nullptr, int type = 0, float depth = 1.0f);

	//static block so no update needed
	void Update(float dt) override;

	//here we use m_size to render too
    void Render(DirectX::SpriteBatch* spriteBatch) override;

private:
	int m_tileXcount;
	int m_tileYcount;

    int m_type; // 0 = Solid, 1 = Ziczag
    float m_depth;
};

