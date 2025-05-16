#pragma once
#include "Enemy.h"
#define WINGS_WIDTH 16
#define WINGS_HEIGHT 16

class Wings : public Enemy
{
public:
	Wings(Vector2 position, Vector2 size, SpriteSheet* spriteSheet);
	~Wings() = default;

	// Implementation of Entity's pure virtual function
	void Update(float dt) override;

	// This is a specialized update method for Wings
	void Update(float dt, Vector2 ownerPosition, Vector2 ownerVelocity);

	void Render(DirectX::SpriteBatch* spriteBatch) override;

	void SetOffset(const Vector2& offset) { m_offset = offset; }
	Vector2 GetOffset() const { return m_offset; }

	void UpdatePosition(const Vector2& ownerPosition);
	void Deactivate();
	bool IsActive() const { return m_isActive; }
	void HandleFlapping(float dt, float flapSpeed);
	// Helper to set direction (for wing orientation)
	void SetDirection(int direction);

private:
	float m_animTimer;
	float m_frameTime;
	bool m_flipFrame;
	bool m_isActive;
	bool m_flipped;

	// Offset relative to the owner (ParaGoomba)
	Vector2 m_offset;
};

