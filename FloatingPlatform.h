#pragma once
#include "Block.h"
class FloatingPlatform :
	public Block
{
public:
	FloatingPlatform(Vector2 position, Vector2 size, int countX, bool isSolid, SpriteSheet* spriteSheet, Vector2 patrolOffset = Vector2::Zero, float roundTime = 0.0f);

	void Fall();
	bool GetIsFalling() const;

	//static block so no update needed
	void Update(float dt) override;
	void OnCollision(const CollisionResult& event) override;
	void OnNoCollision(float dt, Axis axis) override;

	bool IsEntityOnPlatform(Entity* entity) const;
	void UpdateEntitiesOnPlatform(float dt);

	//here we use m_size to render too
	void Render(DirectX::SpriteBatch* spriteBatch) override;

private:
	int m_tileXcount;
	int m_tileYcount = 1;

	std::vector<Entity*> m_entitiesOnPlaform;

	Vector2 m_patrolOffset;
	float m_roundTime;
	float m_roundTimer = 0.0f;

	bool m_isFalling = false;

	Vector2 m_startPosition;

	Vector2 m_previousPosition;
};


