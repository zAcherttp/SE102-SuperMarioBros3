#pragma once
#include "Block.h"

class EndPortal : public Block
{
public:
    EndPortal(Vector2 position, Vector2 size , SpriteSheet* spriteSheet = nullptr);

	//static block so no update needed
	void Update(float dt) override;

	//here we use m_size to render too
    void Render(DirectX::SpriteBatch* spriteBatch) override;

private:
	int m_tileXcount = 1;
	int m_tileYcount = 1;

};

