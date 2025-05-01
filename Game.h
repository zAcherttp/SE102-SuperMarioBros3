//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "SpriteSheet.h"
#include "Animator.h"
#include "World.h"
#include "StepTimer.h"
#include "json.hpp"

using json = nlohmann::json;

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

    static Game* GetInstance();

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
    void GetDefaultWndSize( int& width, int& height ) const noexcept;
    void GetDefaultGameSize( int& width, int& height ) const noexcept;
    void GetDefaultGameTitle(LPCWSTR& title) const noexcept;

    World* GetCurrentWorld();
    bool LoadGame(const std::string& filePath);
    void SwitchWorld();
    void SetNextWorldId(int id);

    SpriteSheet* GetSpriteSheet() const;
    DirectX::SpriteBatch* GetSpriteBatch() const;
private:

    static Game* s_instance;

    void Update(DX::StepTimer const& timer);
    void Render();
    void HandleInput();

    void ClearBackBuffer();
    void LoadGameConfig(const json& config);
    void LoadWorldConfig(const json& config);

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

    // PrimitiveBatch for rendering debug primitives
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::DX11::VertexPositionColor>> m_primitiveBatch;
    std::unique_ptr<DirectX::BasicEffect> m_effect;
    
    std::unique_ptr<DirectX::SpriteFont> m_font;
    DirectX::SimpleMath::Vector2 m_fontPos;
    std::wstring m_spriteFontPath;

	std::unique_ptr<SpriteSheet> m_spriteSheet;
    std::string m_spritePath;
    std::string m_spriteDataPath;

    std::unique_ptr<DirectX::Keyboard> m_keyboard;
    std::unique_ptr<DirectX::Keyboard::KeyboardStateTracker> m_keys;

    DirectX::SimpleMath::Vector2 m_screenPos;
    DirectX::SimpleMath::Vector2 m_origin;
    D3D11_VIEWPORT m_gameView;
    RECT m_gameViewRect;
    LPCWSTR m_gameTitle;
	int m_gameWidth;
	int m_gameHeight;
	int m_wndWidth;
	int m_wndHeight;

    std::unordered_map<int, World*> m_worlds;
    int m_currentWorldId;
    int m_nextWorldId;
};
