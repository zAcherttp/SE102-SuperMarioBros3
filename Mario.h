#pragma once
#include "Entity.h"

using namespace DirectX::SimpleMath;

class Mario : protected Entity
{
public:
	Mario(Vector2 position, int lives, int score, int coins) : Entity(position), m_lives(lives), m_score(score), m_coins(coins) {}
	~Mario() = default;

	void Update(float dt) override;
	void Render() override;

	bool IsGrounded() const override;

	int GetLives() const { return m_lives; }
	void SetLives(int lives) { m_lives = lives; }
	int GetScore() const { return m_score; }
	void SetScore(int score) { m_score = score; }
	int GetCoins() const { return m_coins; }
	void SetCoins(int coins) { m_coins = coins; }

	/// <summary>
	/// Smart pointer for mario's movement state machine
	/// </summary>
	std::unique_ptr<State> m_movementSM;
	/// <summary>
	/// Smart pointer for mario's powerup state machine
	/// </summary>
	std::unique_ptr<State> m_powerupSM;

private:
	int m_lives;
	int m_score;
	int m_coins;
};

