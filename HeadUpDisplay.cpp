#include "pch.h"
#include "HeadUpDisplay.h"
#include "Debug.h"
#include "Entity.h"

constexpr auto RUNSPEED_THRESHOLD = 2.5f * 60.0f;
constexpr auto MAX_FLIGHT_TIME = 255.f / 60.f;

HeadUpDisplay* HeadUpDisplay::s_instance = nullptr;

HeadUpDisplay::HeadUpDisplay()
	: m_lives(4)
	, m_score(0)
	, m_coins(0)
	, m_worldNumber(1)
	, m_timeRemaining(300)
	, m_isTimerRunning(false)
	, m_timeCounter(0.0f)
	, m_pMeterArrows(0)
	, m_maxPMeterArrows(6)
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
{
	s_instance = this;
}

void HeadUpDisplay::Update(float dt)
{
	UpdatePMeterTimer(dt);
	UpdateFlightTimer(dt);
	UpdateGameTimer(dt);
}

void HeadUpDisplay::Render(DirectX::SpriteBatch* spriteBatch, DirectX::SpriteFont* spriteFont, const RECT& gameRect)
{
	int gameWidth, gameHeight;
	Game::GetInstance()->GetDefaultGameSize(gameWidth, gameHeight);
	float scale = ((float)gameRect.right - (float)gameRect.left) / (float)gameWidth;
	Vector2 topLeft(gameRect.left, gameRect.top);

	Vector2 scorePos(64, 211);
	Vector2 timerPos(136, 211);

	Vector2 coinTextSize = ((Vector2)spriteFont->MeasureString(GetCoins()));
	Vector2 coinPos = Vector2(160 - coinTextSize.x, 203);

	Vector2 worldNumPos(48, 203);
	Vector2 livesPos(48, 211);

	spriteFont->DrawString(spriteBatch, GetScore(), topLeft + scorePos * scale, Colors::White, 0.f, Vector2::Zero, scale);
	spriteFont->DrawString(spriteBatch, GetRTime(), topLeft + timerPos * scale, Colors::White, 0.f, Vector2::Zero, scale);
	spriteFont->DrawString(spriteBatch, GetCoins(), topLeft + coinPos * scale, Colors::White, 0.f, Vector2::Zero, scale);
	spriteFont->DrawString(spriteBatch, GetWorld(), topLeft + worldNumPos * scale, Colors::White, 0.f, Vector2::Zero, scale);
	spriteFont->DrawString(spriteBatch, GetLives(), topLeft + livesPos * scale, Colors::White, 0.f, Vector2::Zero, scale);
	
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

	// Handle P-Meter full state
	if (IsPMeterFull()) {
		// When P-Meter is full, Mario's speed accelerates to 0x0380
		if (m_isBDown && m_isDirectionDown) {
			// 16-frame timer is handled in the update function
			if (!m_pMeterTimerActive || m_pMeterTimerType != 3) {
				m_pMeterTimerActive = true;
				m_pMeterTimerType = 3; // 16-frame timer
				m_pMeterTimer = 16.0f; // 16 frames (converted to seconds in UpdatePMeterTimer)
				//Log(LOG_INFO, "Started 16-frame timer for max speed");
			}
		}
		else {
			// If B or direction is released, timer starts counting down
			if (m_pMeterTimerType == 3 && m_pMeterTimerActive) {
				// Let the timer continue counting but don't lock it anymore
			}
		}
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
						Log(LOG_INFO, "Added P-Meter arrow. Current arrows: " + std::to_string(m_pMeterArrows));
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
						Log(LOG_INFO, "Removed P-Meter arrow. Current arrows: " + std::to_string(m_pMeterArrows));
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
			// 16-frame timer for max speed
		case 3:
			if (m_isBDown && m_isDirectionDown && m_currentSpeed >= RUNSPEED_THRESHOLD) {
				// Lock timer at maximum
				m_pMeterTimer = 16.0f;
			}
			else if (m_pMeterTimer <= 0) {
				// Timer expired, deactivate
				m_pMeterTimerActive = false;
			}
			break;
		}
	}
}

void HeadUpDisplay::UpdateFlightTimer(float dt)
{
	// Convert to frame-based timing (assuming 60fps)
	float frameTime = dt * 60.0f;

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

void HeadUpDisplay::SetLives(int lives) { m_lives = lives; }

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

const char* HeadUpDisplay::GetLives()
{
	static char lives[3];
	sprintf_s(lives, "%d", m_lives);
	return lives;
}

const char* HeadUpDisplay::GetScore()
{
	static char score[8];
	sprintf_s(score, "%07d", m_score);
	return score;
}

const char* HeadUpDisplay::GetCoins()
{
	static char coinsBuffer[3];
	sprintf_s(coinsBuffer, "%d", m_coins);
	return coinsBuffer;
}

const char* HeadUpDisplay::GetWorld() const
{
	static char world[2];
	sprintf_s(world, "%d", m_worldNumber);
	return world;
}
const char* HeadUpDisplay::GetRTime() const
{
	static char timeBuffer[4];
	sprintf_s(timeBuffer, "%03d", m_timeRemaining);
	return timeBuffer;
}
