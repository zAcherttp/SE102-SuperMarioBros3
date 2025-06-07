#pragma once

class Camera {
public:
	Camera(int gameWidth, int gameHeight);

	void UpdateWindowSizeDependentMatrices(int windowWidth, int windowHeight);

	// Update based on camera movement/zoom
	void UpdateViewMatrix();

	void Update(float dt);

	void SetPosition(const DirectX::SimpleMath::Vector2& position, bool oneAxis);
	DirectX::SimpleMath::Vector2 GetPosition() const;
	void SetWorldSize(int width, int height);
	void Move(const DirectX::SimpleMath::Vector2& delta);

	const DirectX::SimpleMath::Matrix& GetGameViewMatrix() const;
	const DirectX::SimpleMath::Matrix& GetScreenTransformMatrix() const;
	const DirectX::SimpleMath::Matrix& GetDebugViewMatrix() const;
	const DirectX::SimpleMath::Matrix& GetDebugProjectionMatrix() const;
	const RECT& GetGameViewRect() const;

	void ClampPointInView(DirectX::SimpleMath::Vector2& position) const;

	const bool IsInGameView(const DirectX::SimpleMath::Vector2& position, const int padding = 0) const;

	void Shake(const DirectX::SimpleMath::Vector2& shakeOffset, const int amount = 4, const float duration = 0.5f);

	int GetGameWidth() const;
	int GetGameHeight() const;
	int GetWindowWidth() const;
	int GetWindowHeight() const;

private:
	int m_gameWidth;
	int m_gameHeight;
	int m_windowWidth;
	int m_windowHeight;

	int m_gameWorldWidth;
	int m_gameWorldHeight;

	bool m_isShaking = false;
	DirectX::SimpleMath::Vector2 m_shakeOffset;
	float m_shakeDuration = 0.0f;
	float m_shakeTimer = 0.0f;
	int m_shakeAmount = 0;

	DirectX::SimpleMath::Vector2 m_position;
	DirectX::SimpleMath::Vector2 m_targetPosition;

	DirectX::SimpleMath::Matrix m_gameViewMatrix;           // World -> Game View (for Pass 1 Sprites)
	DirectX::SimpleMath::Matrix m_screenTransformMatrix;    // Game View -> Screen (Letterbox/Pillarbox scale/translate)
	DirectX::SimpleMath::Matrix m_debugViewMatrix;          // World -> Screen (GameView * ScreenTransform) (for Pass 2 Primitives)
	DirectX::SimpleMath::Matrix m_debugProjectionMatrix;    // Screen -> Clip (for Pass 2 Primitives)

	RECT m_gameViewRect;

	// Recalculate combined matrix
	void UpdateDebugMatrix();
};