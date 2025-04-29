#pragma once
#include "Entity.h"
#include "MarioMovementStates.h"

using namespace DirectX::SimpleMath;

class Mario : public Entity
{
public:
	Mario(Vector2 position, int lives, int score, int coins, SpriteSheet* spriteSheet);
	~Mario() = default;

	void ItsAMe();

	void Update(float dt) override;
	void Render(DirectX::SpriteBatch* spriteBatch) override;
	void HandleInput(DirectX::Keyboard::KeyboardStateTracker* kbState);

	bool IsGrounded() const override;

	int GetLives() const { return m_lives; }
	void SetLives(int lives) { m_lives = lives; }
	int GetScore() const { return m_score; }
	void SetScore(int score) { m_score = score; }
	int GetCoins() const { return m_coins; }
	void SetCoins(int coins) { m_coins = coins; }

private:
	int m_lives;
	int m_score;
	int m_coins;

	MarioMovementState* m_movementSM;
	/*MarioPowerUpState* m_powerupSM;*/
};

