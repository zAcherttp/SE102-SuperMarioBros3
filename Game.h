//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "SpriteSheet.h"
#include "World.h"
#include "GameLoader.h"

#include "StepTimer.h"


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
    friend class GameLoader;
public:

    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

private:

    void Update(DX::StepTimer const& timer);
    void Render();
    void HandleInput();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources> m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer m_timer;
                            
    // Game view
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_gameRenderTarget;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_gameRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_gameShaderResource;

    // Textures
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

	// SpriteBatch for rendering sprites
    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
    std::unique_ptr<DirectX::CommonStates> m_states;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::DX11::VertexPositionColor>> m_primitiveBatch;
    std::unique_ptr<DirectX::BasicEffect> m_effect;
    
    DirectX::SimpleMath::Vector2 m_screenPos;
    DirectX::SimpleMath::Vector2 m_origin;
    D3D11_VIEWPORT m_gameView;
    RECT m_gameViewRect;

    std::unique_ptr<DirectX::SpriteFont> m_font;
    DirectX::SimpleMath::Vector2 m_fontPos;

	std::unique_ptr<SpriteSheet> m_spriteSheet;

    std::unique_ptr<DirectX::Keyboard> m_keyboard;
    DirectX::Keyboard::KeyboardStateTracker m_keys;

	std::unique_ptr<World> m_gameWorld;
};
