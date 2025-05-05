#pragma once
#include "Entity.h"
#include "MarioMovementStates.h"

using namespace DirectX::SimpleMath;

struct InputState {
	bool isLeftDown = false;
	bool isRightDown = false;
	bool isUpDown = false;
	bool isDownDown = false;
	bool isADown = false;
	bool isBDown = false;

	bool isLeftPressed = false;
	bool isRightPressed = false;
	bool isUpPressed = false;
	bool isDownPressed = false;
	bool isAPressed = false;
	bool isBPressed = false;
	bool isStartPressed = false;
};
	

class Mario : public Entity
{
public:
	Mario(Vector2 position, int lives, int score, int coins, SpriteSheet* spriteSheet);
	~Mario() = default;

	std::vector<std::pair<InteractionPointType, Vector2>> GetInteractionPoints() const override;
	bool UsesInteractionPoints() const override;

	void ItsAMe();

	void Update(float dt) override;
	void Render(DirectX::SpriteBatch* spriteBatch) override;
	void HandleInput(DirectX::Keyboard::State* kbState, DirectX::Keyboard::KeyboardStateTracker* kbsTracker);
	const InputState* GetInput() const { return m_inputState; }

	int GetLives() const { return m_lives; }
	void SetLives(int lives) { m_lives = lives; }
	int GetScore() const { return m_score; }
	void SetScore(int score) { m_score = score; }
	int GetCoins() const { return m_coins; }
	void SetCoins(int coins) { m_coins = coins; }

	std::string GetCurrentStateName() const { return m_movementSM->GetStateName(); }

	void OnCollision(const CollisionEvent& event) override;
	void OnNoCollision() override;
	void OnTopHeadCollision(Entity* other, const Vector2& normal) override;
	void OnFootCollision(Entity* other, const Vector2& normal) override;
	void OnLeftSideCollision(Entity* other, const Vector2& normal) override;
	void OnRightSideCollision(Entity* other, const Vector2& normal) override;

private:
	int m_lives;
	int m_score;
	int m_coins;

	InputState* m_inputState;

	MarioMovementState* m_movementSM;
	/*MarioPowerUpState* m_powerupSM;*/

	std::vector<std::pair<InteractionPointType, Vector2>> GetSmallMarioInteractionPoints() const;
	std::vector<std::pair<InteractionPointType, Vector2>> GetBigMarioInteractionPoints() const;
	std::vector<std::pair<InteractionPointType, Vector2>> GetBigMarioSitInteractionPoints() const;
};
