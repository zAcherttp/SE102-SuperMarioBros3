#pragma once
#include "Entity.h"
#include "MarioMovementStates.h"
#include "MarioPowerUpStates.h"

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

	void ResetDirectionals() {
		isLeftDown = isLeftPressed = false;
		isRightDown = isRightPressed = false;
		isUpDown = isUpPressed = false;
		isDownDown = isDownPressed = false;
	}
};

class Mario : public Entity
{
public:
	Mario(Vector2 position, int lives, int score, int coins, SpriteSheet* spriteSheet);
	~Mario() = default;

	std::vector<std::pair<InteractionPointType, Vector2>> GetInteractionPoints() const override;
	bool UsesInteractionPoints() const override;

	void ItsAMe();
	void Damage();
	bool Kick(Direction dir, Entity* ent);

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

	bool IsTransitioning() const;
	bool IsDying() const;

	std::string GetCurrentMStateName() const { return m_movementSM->GetStateName(); }
	std::string GetCurrentPStateName() const { return m_powerupSM->GetStateName(); }

	int GetCurrentMStateAnimValue() const { return m_movementSM->GetStateAnimValue(); }
	int GetCurrentPStateAnimValue() const { return m_powerupSM->GetStateAnimValue(); }

	Vector2 GetCurrentMStateSizeOffset() const { return m_movementSM->GetStateSizeOffset(); }
	Vector2 GetCurrentPStateSizeOffset() const { return m_powerupSM->GetStateSizeOffset(); }

	void OnCollision(const CollisionResult& event) override;
	void OnNoCollision(float dt, Axis axis) override;

	void OnTopHeadCollision(const CollisionResult& event) override;
	void OnFootCollision(const CollisionResult& event) override;
	void OnLeftSideCollision(const CollisionResult& event) override;
	void OnRightSideCollision(const CollisionResult& event) override;

private:
	int m_lives;
	int m_score;
	int m_coins;

	InputState* m_inputState;

	std::unique_ptr<MarioMovementState> m_movementSM;
	std::unique_ptr<MarioPowerUpState> m_powerupSM;

	std::vector<std::pair<InteractionPointType, Vector2>> GetSmallMarioInteractionPoints() const;
	std::vector<std::pair<InteractionPointType, Vector2>> GetBigMarioInteractionPoints() const;
	std::vector<std::pair<InteractionPointType, Vector2>> GetBigMarioSitInteractionPoints() const;
};
