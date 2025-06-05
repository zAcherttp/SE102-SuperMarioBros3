#include "pch.h"
#include "Camera.h"
#include "Debug.h"

using namespace DirectX::SimpleMath;

Camera::Camera(int gameWidth, int gameHeight) :
	m_gameWidth(gameWidth),
	m_gameHeight(gameHeight),
	m_windowWidth(gameWidth),
	m_windowHeight(gameHeight),
	m_position(Vector2::Zero),
	m_screenTransformMatrix(Matrix::Identity),
	m_debugViewMatrix(Matrix::Identity),
	m_debugProjectionMatrix(Matrix::Identity),
	m_gameViewRect{ 0, 0, gameWidth, gameHeight }
{
	UpdateViewMatrix();
	UpdateWindowSizeDependentMatrices(m_windowWidth, m_windowHeight);
}

void Camera::UpdateWindowSizeDependentMatrices(int windowWidth, int windowHeight) {
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	float fWindowWidth = static_cast<float>(windowWidth);
	float fWindowHeight = static_cast<float>(windowHeight);

	const float gameAspectRatio = static_cast<float>(m_gameWidth) / static_cast<float>(m_gameHeight);
	const float windowAspectRatio = fWindowWidth / fWindowHeight;

	float viewWidth, viewHeight;
	float viewX, viewY;

	if (windowAspectRatio > gameAspectRatio) {
		viewHeight = fWindowHeight;
		viewWidth = viewHeight * gameAspectRatio;
		viewY = 0.0f;
		viewX = (fWindowWidth - viewWidth) / 2.0f;
	}
	else {
		viewWidth = fWindowWidth;
		viewHeight = viewWidth / gameAspectRatio;
		viewX = 0.0f;
		viewY = (fWindowHeight - viewHeight) / 2.0f;
	}

	m_gameViewRect = {
		static_cast<LONG>(viewX), static_cast<LONG>(viewY),
		static_cast<LONG>(viewX + viewWidth), static_cast<LONG>(viewY + viewHeight)
	};

	float scale = viewWidth / static_cast<float>(m_gameWidth);

	m_screenTransformMatrix =
		Matrix::CreateScale(scale, scale, 1.0f) *
		Matrix::CreateTranslation(viewX, viewY, 0.0f);

	m_debugProjectionMatrix = Matrix::CreateOrthographicOffCenter(0.0f, fWindowWidth, fWindowHeight, 0.0f, 0.0f, 1.0f); // Use RH if that's your convention

	UpdateDebugMatrix();

	OutputDebugStringA(("[Camera] Scale: " + std::to_string(scale) +
		", Translation X: " + std::to_string(viewX) +
		", Translation Y: " + std::to_string(viewY) + "\n").c_str());
}

void Camera::UpdateViewMatrix() {
	// Game View: Transforms World coordinates to Game View coordinates (origin at top-left)
	// It needs to center the view on the camera position and apply zoom.
	// The translation should move the world so the camera's position becomes the center of the view.
	// Since SpriteBatch/BasicEffect origins are often top-left, we translate by -(position - halfGameView)
	// But a simpler way is often to translate by -position and then maybe adjust if needed,
	// or translate so the camera position maps to the *center* of the game view.
	// Let's assume camera position (0,0) means top-left of world is at top-left of view.
	Matrix translation = Matrix::CreateTranslation(-m_position.x, -m_position.y, 0.f);
	// If origin should be center:
	// Matrix centerOffset = Matrix::CreateTranslation(m_gameWidth * 0.5f, m_gameHeight * 0.5f, 0.f);
	// Matrix invCenterOffset = Matrix::CreateTranslation(-m_gameWidth * 0.5f, -m_gameHeight * 0.5f, 0.f);
	// m_gameViewMatrix = translation * invCenterOffset * scale * centerOffset; // Order matters! T*C-*S*C

	// Simpler: Translate world by negative camera pos, then scale around origin (0,0).
	m_gameViewMatrix = translation;

	// Recalculate the combined debug view matrix
	UpdateDebugMatrix();
}

void Camera::UpdateDebugMatrix() {
	m_debugViewMatrix = m_gameViewMatrix * m_screenTransformMatrix;
}

void Camera::Update(float dt) {
	if (m_isShaking) {
		m_shakeTimer += dt;
		if (m_shakeTimer >= m_shakeDuration) {
			// Shake finished - reset to target position
			m_isShaking = false;
			m_shakeTimer = 0.0f;
			m_shakeAmount = 0;
			m_shakeOffset = DirectX::SimpleMath::Vector2::Zero;
			m_position = m_targetPosition; // Use target position instead of original
			return;
		}

		if (m_shakeAmount > 0) {
			// Spread the shake over m_shakeDuration and m_shakeAmount
			float interval = m_shakeDuration / static_cast<float>(m_shakeAmount);
			int currentShake = static_cast<int>(m_shakeTimer / interval);
			if (currentShake < m_shakeAmount) {
				// Alternate shake direction for each interval
				float shakeFactor = (currentShake % 2 == 0) ? -1.0f : 0.0f;
				m_position = m_targetPosition + m_shakeOffset * shakeFactor;

				/*Log(__FUNCTION__, "Shaking camera: currentShake = " + std::to_string(currentShake) +
					", m_position = (" + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ")");*/
			}
			else {
				m_position = m_targetPosition;
			}
		}
	}
	else {
		// Not shaking - set position to target position
		m_position = m_targetPosition;
	}

	// Update view matrix after position changes
	UpdateViewMatrix();
}

void Camera::SetWorldSize(int width, int height) {
	m_gameWorldWidth = width;
	m_gameWorldHeight = height;
}

void Camera::SetPosition(const Vector2& position, bool oneAxis = false) {
	Vector2 clampedPosition = position;
	if (oneAxis) {
		if (position.x == 0) {
			clampedPosition.y = std::max(0.0f, std::min(clampedPosition.y, static_cast<float>(m_gameWorldHeight - m_gameHeight)));
		}
		else {
			clampedPosition.x = std::max(0.0f, std::min(clampedPosition.x, static_cast<float>(m_gameWorldWidth - m_gameWidth)));
		}
	}
	else {
		clampedPosition.x = std::max(0.0f, std::min(clampedPosition.x, static_cast<float>(m_gameWorldWidth - m_gameWidth)));
		clampedPosition.y = std::max(0.0f, std::min(clampedPosition.y, static_cast<float>(m_gameWorldHeight - m_gameHeight)));
	}
	m_targetPosition = clampedPosition;
	UpdateViewMatrix();
}

void Camera::Move(const Vector2& delta) {
	m_targetPosition = m_position + delta;
	UpdateViewMatrix();
}