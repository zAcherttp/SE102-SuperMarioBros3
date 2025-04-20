//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "AnimatedTexture.h"
#include "StepTimer.h"


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
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

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>                m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                                        m_timer;
                            
    // Texture resource
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>     m_texture;

	// SpriteBatch for rendering sprites
    std::unique_ptr<DirectX::SpriteBatch>                m_spriteBatch;
    std::unique_ptr<DirectX::CommonStates>               m_states;
    DirectX::SimpleMath::Vector2                         m_screenPos;
    DirectX::SimpleMath::Vector2                         m_origin;

    std::unique_ptr<AnimatedTexture>                     m_ship;
    DirectX::SimpleMath::Vector2                         m_shipPos;

    std::unique_ptr<DirectX::SpriteFont>                 m_font;
    DirectX::SimpleMath::Vector2                         m_fontPos;
};
