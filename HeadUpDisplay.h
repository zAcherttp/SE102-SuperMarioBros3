#pragma once
#include "pch.h"
#include "Animator.h"
#include <string>

using namespace DirectX::SimpleMath;

class HeadUpDisplay
{
public:
	HeadUpDisplay(SpriteSheet* spriteSheet);
	~HeadUpDisplay() = default;
	HeadUpDisplay(const HeadUpDisplay&) = delete;
	HeadUpDisplay& operator=(const HeadUpDisplay&) = delete;

	void Update(float dt);
	void Render(DirectX::SpriteBatch* spriteBatch, DirectX::SpriteFont* spriteFont, const RECT& gameRect);

	static HeadUpDisplay* GetInstance();

	void UpdatePMeter(float speed, bool isOnGround, bool isBDown, bool isDirectionDown);
	int GetPMeterValue() const;
	bool IsPMeterFull() const;

	void SetLives(int lives);
	int GetLives() const;
	void SetScore(int score);
	int GetScore() const;
	void SetCoins(int coins);
	int GetCoins() const;
	void AddCoins(int amount);
	void AddScore(int amount);

	void SetWorldNumber(int world);
	int GetWorldNumber() const;

	void SetTime(int time);
	int GetTime() const;
	void StartTimer();
	void StopTimer();

	// P-Meter flight timer management
	void StartFlightTimer();
	void StopFlightTimer();
	bool IsFlightActive() const;
	float GetFlightTimeRemaining() const;

	// Mario powerup type
	void SetMarioPowerupType(int powerupType);
	int GetMarioPowerupType() const;

	void Reset();

private:
	static HeadUpDisplay* s_instance;

	std::unique_ptr<Animator> m_animator;

	// Game stats
	int m_lives;
	int m_score;
	int m_coins;
	int m_worldNumber;
	int m_timeRemaining;
	bool m_isTimerRunning;
	float m_timeCounter;

	const char* GetLivesChar() const;
	const char* GetScoreChar() const;
	const char* GetCoinsChar() const;
	const char* GetWorldChar() const;
	const char* GetRTimeChar() const;

	// P-Meter related
	int m_pMeterArrows;
	int m_maxPMeterArrows;
	float m_pMeterTimer;
	float m_pMeterTimerThreshold;
	bool m_pMeterTimerActive;
	int m_pMeterTimerType; // 0 = none, 1 = 8-frame, 2 = 24-frame, 3 = 16-frame
	float m_pMeterBadgeBlinkTimer;

	// Mario's speed and state
	float m_currentSpeed;
	bool m_isOnGround;
	bool m_isBDown;
	bool m_isDirectionDown;
	int m_currentPowerupType; // 0 = small, 1 = super, 2 = raccoon

	// Flight timer (255 frames)
	bool m_flightTimerActive;
	float m_flightTimer;
	float m_maxFlightTime;

	// Utility functions
	void UpdatePMeterTimer(float dt);
	void UpdateFlightTimer(float dt);
	void UpdateGameTimer(float dt);

	// load sprites
	void LoadSprites();
};