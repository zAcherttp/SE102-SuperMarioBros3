#include "pch.h"
#include "Animator.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "Game.h"
#include "HeadUpDisplay.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "SpriteSheet.h"
#include <cmath>
#include <cstdio>
#include <DirectXColors.h>
#include <memory>
#include <string>
#include <Windows.h>

constexpr auto RUNSPEED_THRESHOLD = 2.5f * 60.0f;
constexpr auto MAX_FLIGHT_TIME = 255.f / 60.f;

HeadUpDisplay* HeadUpDisplay::s_instance = nullptr;

HeadUpDisplay::HeadUpDisplay(SpriteSheet* spriteSheet)
	: m_lives(4)
	, m_score(0)
	, m_coins(0)
	, m_worldNumber(1)
	, m_timeRemaining(300)
	, m_isTimerRunning(false)
	, m_timeCounter(0.0f)
	, m_pMeterArrows(0)
	, m_maxPMeterArrows(7)
	, m_pMeterTimer(0.0f)
	, m_pMeterTimerThreshold(0.0f)
	, m_pMeterTimerActive(false)
	, m_pMeterTimerType(0)
	, m_currentSpeed(0.0f)
	, m_isOnGround(false)
	, m_currentPowerupType(0)
	, m_isBDown(false)
	, m_isDirectionDown(false)
	, m_flightTimerActive(false)
	, m_flightTimer(0.0f)
	, m_maxFlightTime(MAX_FLIGHT_TIME)
	, m_pMeterBadgeBlinkTimer(0.0f)
{
	s_instance = this;

	m_animator = std::make_unique<Animator>();
	m_animator->SetSpriteSheet(spriteSheet);

	LoadSprites();
}

void HeadUpDisplay::Update(float dt)
{
	UpdatePMeterTimer(dt);
	UpdateFlightTimer(dt);
	UpdateGameTimer(dt);

	// Update badge blink timer
	m_pMeterBadgeBlinkTimer += dt;
	if (m_pMeterBadgeBlinkTimer >= 1.0f) // reset every 1s to avoid float drift
		m_pMeterBadgeBlinkTimer -= 1.0f;
}

void HeadUpDisplay::Render(DirectX::SpriteBatch* spriteBatch, DirectX::SpriteFont* spriteFont, const RECT& gameRect)
{
	int gameWidth, gameHeight;
	Game::GetInstance()->GetDefaultGameSize(gameWidth, gameHeight);
	float scale = ((float)gameRect.right - (float)gameRect.left) / (float)gameWidth;
	Vector2 topLeft((float)gameRect.left, (float)gameRect.top);

	Vector2 scorePos(64, 211);
	Vector2 timerPos(136, 211);

	Vector2 coinTextSize = ((Vector2)spriteFont->MeasureString(GetCoinsChar()));
	Vector2 coinPos = Vector2(160 - coinTextSize.x, 203);

	Vector2 worldNumPos(48, 203);
	Vector2 livesPos(48, 211);

	// Draw hud board and powerup slots
	// 			draw with center pos							  top left pos		 offset to top left	
	m_animator->Draw(spriteBatch, ID_SPRITE_HUD_BOARD, topLeft + (Vector2(13.f, 197.f) + Vector2(75.f, 13.f)) * scale, 0.0f, scale);

	for (int i = 0; i < 3; i++) {
		m_animator->Draw(spriteBatch, ID_SPRITE_HUD_POWERUP_SLOT, topLeft + (Vector2(172.f + 24.f * i, 197.f) + Vector2(11.f, 13.f)) * scale, 0.0f, scale);
	}

	// Draw Mario badge
	m_animator->Draw(spriteBatch, ID_SPRITE_HUD_M_BADGE, topLeft + (Vector2(16.f, 211.f) + Vector2(8.f, 3.5f)) * scale, 0.0f, scale);

	// Draw P-Meter arrows
	for (int i = 0; i < 6; i++) {
		m_animator->Draw(spriteBatch, ID_SPRITE_HUD_PMETER_ARROW_FULL, topLeft + (Vector2(64.f + 8.f * i, 203.f) + Vector2(4.f, 3.5f)) * scale, 0.0f, scale);
	}
	for (int i = m_pMeterArrows; i < 6; i++) {
		m_animator->Draw(spriteBatch, ID_SPRITE_HUD_PMETER_ARROW_EMPTY, topLeft + (Vector2(64.f + 8.f * i, 203.f) + Vector2(4.f, 3.5f)) * scale, 0.0f, scale);
	}

	// Draw P-Meter badge with blinking effect
	if (m_pMeterArrows >= m_maxPMeterArrows)
	{
		// Alternate every 0.5s between FULL and EMPTY
		bool showFull = (fmodf(m_pMeterBadgeBlinkTimer, 1.0f) < 0.5f);
		int badgeSprite = showFull ? ID_SPRITE_HUD_PMETER_BADGE_FULL : ID_SPRITE_HUD_PMETER_BADGE_EMPTY;
		m_animator->Draw(spriteBatch, badgeSprite, topLeft + (Vector2(113.f, 203.f) + Vector2(7.5f, 3.5f)) * scale, 0.0f, scale);
	}
	else
	{
		m_animator->Draw(spriteBatch, ID_SPRITE_HUD_PMETER_BADGE_EMPTY, topLeft + (Vector2(113.f, 203.f) + Vector2(7.5f, 3.5f)) * scale, 0.0f, scale);
	}

	spriteFont->DrawString(spriteBatch, GetScoreChar(), topLeft + scorePos * scale, DirectX::Colors::White, 0.f, Vector2::Zero, scale);
	spriteFont->DrawString(spriteBatch, GetRTimeChar(), topLeft + timerPos * scale, DirectX::Colors::White, 0.f, Vector2::Zero, scale);
	spriteFont->DrawString(spriteBatch, GetCoinsChar(), topLeft + coinPos * scale, DirectX::Colors::White, 0.f, Vector2::Zero, scale);
	spriteFont->DrawString(spriteBatch, GetWorldChar(), topLeft + worldNumPos * scale, DirectX::Colors::White, 0.f, Vector2::Zero, scale);
	spriteFont->DrawString(spriteBatch, GetLivesChar(), topLeft + livesPos * scale, DirectX::Colors::White, 0.f, Vector2::Zero, scale);

}

void HeadUpDisplay::UpdatePMeter(float speed, bool isOnGround, bool isBDown, bool isDirectionDown)
{
	// Store the current state
	m_currentSpeed = speed;
	m_isOnGround = isOnGround;
	m_isBDown = isBDown;
	m_isDirectionDown = isDirectionDown;

	// If not on ground, don't process P-Meter updates
	// (except for flight timer which is handled separately)
	if (!isOnGround) {
		return;
	}

	// Initialize a timer if needed
	if (speed >= RUNSPEED_THRESHOLD && !m_pMeterTimerActive) {
		// Start an 8-frame timer for adding an arrow
		m_pMeterTimerActive = true;
		m_pMeterTimerType = 1; // 8-frame timer
		m_pMeterTimer = 8.0f;  // 8 frames (converted to seconds in UpdatePMeterTimer)
		//Log(LOG_INFO, "Started 8-frame P-Meter timer for adding arrow");
	}
	else if (speed < RUNSPEED_THRESHOLD && m_pMeterArrows > 0 && !m_pMeterTimerActive) {
		// Start a 24-frame timer for removing an arrow
		m_pMeterTimerActive = true;
		m_pMeterTimerType = 2; // 24-frame timer
		m_pMeterTimer = 24.0f; // 24 frames (converted to seconds in UpdatePMeterTimer)
		//Log(LOG_INFO, "Started 24-frame P-Meter timer for removing arrow");
	}
}

void HeadUpDisplay::UpdatePMeterTimer(float dt)
{
	float frameTime = dt * 60.0f;

	if (m_pMeterTimerActive) {
		// Decrease the timer
		m_pMeterTimer -= frameTime;

		switch (m_pMeterTimerType) {
			// 8-frame timer for adding arrow
		case 1:
			if (m_pMeterTimer <= 0) {
				// Timer has elapsed, check if criteria are still met
				if (m_isOnGround && m_currentSpeed >= RUNSPEED_THRESHOLD) {
					// Add an arrow to P-Meter
					if (m_pMeterArrows < m_maxPMeterArrows) {
						m_pMeterArrows++;
						//Log(LOG_INFO, "Added P-Meter arrow. Current arrows: " + std::to_string(m_pMeterArrows));
					}

					// Restart 8-frame timer
					m_pMeterTimer = 8.0f;
				}
				else {
					// Criteria not met, deactivate timer
					m_pMeterTimerActive = false;
				}
			}
			break;
			// 24-frame timer for removing arrow
		case 2:
			if (m_pMeterTimer <= 0) {
				// Timer has elapsed, check if criteria are still not met
				if (!m_isOnGround || m_currentSpeed < RUNSPEED_THRESHOLD) {
					// Remove an arrow from P-Meter
					if (m_pMeterArrows > 0) {
						m_pMeterArrows--;
						//Log(LOG_INFO, "Removed P-Meter arrow. Current arrows: " + std::to_string(m_pMeterArrows));
					}

					// Restart 24-frame timer if there are still arrows
					if (m_pMeterArrows > 0) {
						m_pMeterTimer = 24.0f;
					}
					else {
						// No more arrows, deactivate timer
						m_pMeterTimerActive = false;
					}
				}
				else {
					// Criteria now met, deactivate timer
					m_pMeterTimerActive = false;
				}
			}
			break;
		}
	}
}

void HeadUpDisplay::UpdateFlightTimer(float dt)
{
	// Convert to frame-based timing (assuming 60fps)
	float frameTime = dt * 200.0f;

	if (m_flightTimerActive) {
		// Decrease the timer
		m_flightTimer -= frameTime;

		// Check if the timer has expired
		if (m_flightTimer <= 0) {
			if (m_currentPowerupType == 3) { // Raccoon
				// P-Meter instantly empties regardless of speed/position
				m_pMeterArrows = 0;
				Log(LOG_INFO, "Flight timer expired. P-Meter emptied.");
			}

			// Flight timer is now inactive
			m_flightTimerActive = false;
		}

		// Exception for Raccoon/Tanooki: if they land and accelerate to 0x0280 before flight timer ends
		if (m_currentPowerupType == 3 && m_isOnGround && m_currentSpeed >= RUNSPEED_THRESHOLD) {
			// If they meet the criteria before flight timer ends, they can prevent P-Meter from emptying
			Log(LOG_INFO, "Raccoon landed and accelerated. Preventing P-Meter emptying.");
			StopFlightTimer();
		}
	}
}

void HeadUpDisplay::UpdateGameTimer(float dt)
{
	if (m_isTimerRunning) {
		m_timeCounter += dt;

		// Decrease time every second
		if (m_timeCounter >= 1.0f) {
			m_timeCounter -= 1.0f;
			if (m_timeRemaining > 0) {
				m_timeRemaining--;
			}
		}
	}
}

void HeadUpDisplay::LoadSprites()
{
	m_animator->DefineAnimation(ID_SPRITE_HUD_BOARD, { L"hud" }, false);
	m_animator->DefineAnimation(ID_SPRITE_HUD_BOARD_BACKGROUND, { L"hud-background" }, false);
	m_animator->DefineAnimation(ID_SPRITE_HUD_PMETER_ARROW_FULL, { L"arrow1" }, false);
	m_animator->DefineAnimation(ID_SPRITE_HUD_PMETER_ARROW_EMPTY, { L"arrow2" }, false);
	m_animator->DefineAnimation(ID_SPRITE_HUD_M_BADGE, { L"mbadge" }, false);
	m_animator->DefineAnimation(ID_SPRITE_HUD_L_BADGE, { L"lbadge" }, false);
	m_animator->DefineAnimation(ID_SPRITE_HUD_PAUSE, { L"pause" }, false);
	m_animator->DefineAnimation(ID_SPRITE_HUD_PMETER_BADGE_FULL, { L"pbadge1" }, false);
	m_animator->DefineAnimation(ID_SPRITE_HUD_PMETER_BADGE_EMPTY, { L"pbadge2" }, false);
	m_animator->DefineAnimation(ID_SPRITE_HUD_FIREFLOWER, { L"powerup-fireflower" }, false);
	m_animator->DefineAnimation(ID_SPRITE_HUD_MUSHROOM, { L"powerup-mushroom" }, false);
	m_animator->DefineAnimation(ID_SPRITE_HUD_STAR, { L"powerup-star" }, false);
	m_animator->DefineAnimation(ID_SPRITE_HUD_POWERUP_SLOT, { L"powerupslot" }, false);
}

void HeadUpDisplay::AddCoins(int amount)
{
	m_coins += amount;

	// Check for 100 coins = 1 extra life
	if (m_coins >= 100) {
		m_lives++;
		m_coins -= 100;
		Log(LOG_INFO, "Got 100 coins! Extra life earned. Lives: " + std::to_string(m_lives));
	}
}

HeadUpDisplay* HeadUpDisplay::GetInstance() { return s_instance; }

int HeadUpDisplay::GetPMeterValue() const { return m_pMeterArrows; }

bool HeadUpDisplay::IsPMeterFull() const { return m_pMeterArrows >= m_maxPMeterArrows; }

void HeadUpDisplay::SetLives(int lives) {
	m_lives = lives;
	if (m_lives <= 0) m_lives = 4;
}

int HeadUpDisplay::GetLives() const { return m_lives; }

void HeadUpDisplay::SetScore(int score) { m_score = score; }

int HeadUpDisplay::GetScore() const { return m_score; }

void HeadUpDisplay::SetCoins(int coins) { m_coins = coins; }

int HeadUpDisplay::GetCoins() const { return m_coins; }

void HeadUpDisplay::AddScore(int amount) { m_score += amount; }

void HeadUpDisplay::SetWorldNumber(int world) { m_worldNumber = world; }

int HeadUpDisplay::GetWorldNumber() const { return m_worldNumber; }

void HeadUpDisplay::SetTime(int time) { m_timeRemaining = time; }

int HeadUpDisplay::GetTime() const { return m_timeRemaining; }

void HeadUpDisplay::StartTimer() { m_isTimerRunning = true; }

void HeadUpDisplay::StopTimer() { m_isTimerRunning = false; }

// P-Meter flight timer management
void HeadUpDisplay::StartFlightTimer() { m_flightTimerActive = true; m_flightTimer = m_maxFlightTime; }

void HeadUpDisplay::StopFlightTimer() { m_flightTimerActive = false; m_flightTimer = 0; }

bool HeadUpDisplay::IsFlightActive() const { return m_flightTimerActive && m_flightTimer > 0; }

float HeadUpDisplay::GetFlightTimeRemaining() const { return m_flightTimer; }

// Mario powerup type
void HeadUpDisplay::SetMarioPowerupType(int powerupType) { m_currentPowerupType = powerupType; }

int HeadUpDisplay::GetMarioPowerupType() const { return m_currentPowerupType; }

const char* HeadUpDisplay::GetLivesChar() const
{
	static char lives[3];
	sprintf_s(lives, "%d", m_lives);
	return lives;
}

const char* HeadUpDisplay::GetScoreChar() const
{
	static char score[8] = { 0 };
	sprintf_s(score, "%07d", m_score);
	return score;
}

const char* HeadUpDisplay::GetCoinsChar() const
{
	static char coinsBuffer[3];
	sprintf_s(coinsBuffer, "%d", m_coins);
	return coinsBuffer;
}

const char* HeadUpDisplay::GetWorldChar() const
{
	static char world[2];
	sprintf_s(world, "%d", m_worldNumber);
	return world;
}
const char* HeadUpDisplay::GetRTimeChar() const
{
	static char timeBuffer[4];
	sprintf_s(timeBuffer, "%03d", m_timeRemaining);
	return timeBuffer;
}
