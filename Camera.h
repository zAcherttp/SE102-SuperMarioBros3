#pragma once

class Camera {
public:
    Camera(int gameWidth, int gameHeight);

    void UpdateWindowSizeDependentMatrices(int windowWidth, int windowHeight);

    // Update based on camera movement/zoom
    void UpdateViewMatrix();

    void SetPosition(const DirectX::SimpleMath::Vector2& position, bool oneAxis);
    void Move(const DirectX::SimpleMath::Vector2& delta);

    const DirectX::SimpleMath::Matrix& GetGameViewMatrix() const { return m_gameViewMatrix; }
    const DirectX::SimpleMath::Matrix& GetScreenTransformMatrix() const { return m_screenTransformMatrix; }
    const DirectX::SimpleMath::Matrix& GetDebugViewMatrix() const { return m_debugViewMatrix; }
    const DirectX::SimpleMath::Matrix& GetDebugProjectionMatrix() const { return m_debugProjectionMatrix; }
    const RECT& GetGameViewRect() const { return m_gameViewRect; }

private:
    int m_gameWidth;
    int m_gameHeight;
    int m_windowWidth;
    int m_windowHeight;

    DirectX::SimpleMath::Vector2 m_position;

    DirectX::SimpleMath::Matrix m_gameViewMatrix;           // World -> Game View (for Pass 1 Sprites)
    DirectX::SimpleMath::Matrix m_screenTransformMatrix;    // Game View -> Screen (Letterbox/Pillarbox scale/translate)
    DirectX::SimpleMath::Matrix m_debugViewMatrix;          // World -> Screen (GameView * ScreenTransform) (for Pass 2 Primitives)
    DirectX::SimpleMath::Matrix m_debugProjectionMatrix;    // Screen -> Clip (for Pass 2 Primitives)

    RECT m_gameViewRect;

    // Recalculate combined matrix
    void UpdateDebugMatrix();
};