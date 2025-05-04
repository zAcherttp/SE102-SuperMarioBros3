#include "pch.h"
#include "Camera.h"

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


void Camera::SetPosition(const Vector2& position, bool oneAxis = false) {
    if (oneAxis) {
        if (position.x == 0) {
            m_position.y = position.y;
        }
        else {
            m_position.x = position.x;
        }
    }
    else {
        m_position = position;
    }
    UpdateViewMatrix();
}

void Camera::Move(const Vector2& delta) {
    m_position += delta;
    UpdateViewMatrix();
}