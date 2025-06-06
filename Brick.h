#pragma once
#include "Block.h"
#include "effects.h"

enum class BrickState
{
	BRICK,
	COIN
};

class Brick : public Block
{
public:
    Brick(Vector2 position, Vector2 size, bool isSolid, SpriteSheet* spriteSheet);

	void Break();

	//static block so no update needed
	void Update(float dt) override;

	//here we use m_size to render too
    void Render(DirectX::SpriteBatch* spriteBatch) override;

	void TransformToCoin();
	void TransformToBrick();

	void OnCollision(const CollisionResult& event) override;

private:
	int m_tileXcount = 1;
	int m_tileYcount = 1;

	float m_timer = 0.0f;

	BrickState m_state = BrickState::BRICK;
};
