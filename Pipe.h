#pragma once
#include "Block.h"

class Pipe : public Block
{
public:
	Pipe(Vector2 position, Vector2 size, int countX, int countY, bool isSolid, SpriteSheet* spriteSheet, bool hasHead = true, bool isBonus = false, bool isCollidable = true, bool enterable = false, int enterType = -1);

	void Update(float dt) override;

	//here we use m_size to render too
	void Render(DirectX::SpriteBatch* spriteBatch) override;

	void OnCollision(const CollisionResult& event) override {}

	bool GetEnterable() const;
	int GetEnterType() const;

private:
	int m_tileXcount;
	int m_tileYcount;
	bool m_hasHead;
	bool m_isBonus = false;

	bool m_enterable = false;
	int m_enterType;
};

