//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "Helpers.h"
#include "DebugOverlay.h"
#include "Mario.h"

constexpr auto GAME_WIDTH = 256;
constexpr auto GAME_HEIGHT = 240;
constexpr auto WND_WIDTH = 512;
constexpr auto WND_HEIGHT = 512;

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
	m_deviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_FORMAT_UNKNOWN);
	// TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
	//   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
	//   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
	m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height) {
	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	// Renders only 2D, so no need for a depth buffer.
	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	m_keyboard = std::make_unique<Keyboard>();

	m_gameView.TopLeftX = 0.0f;
	m_gameView.TopLeftY = 0.0f;
	m_gameView.Width = static_cast<float>(GAME_WIDTH);
	m_gameView.Height = static_cast<float>(GAME_HEIGHT);
	m_gameView.MinDepth = 0.0f;
	m_gameView.MaxDepth = 1.0f;

	// uncomment to use the fixed timestep
	/*m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);*/
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	m_timer.Tick([&]()
		{
			Update(m_timer);
		});

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer) {
	float elapsedTime = float(timer.GetElapsedSeconds());

	if (m_gameWorld) {
		/*m_gameWorld->Update(elapsedTime);*/
	}

	HandleInput();
	elapsedTime;
}
#pragma endregion

void Game::HandleInput() {
	auto kb = m_keyboard->GetState();
	m_keys.Update(kb);
	DebugOverlay::UpdateInput(&kb);
	if (m_keys.IsKeyPressed(Keyboard::Escape)) {
		ExitGame();
	}
	if (m_keys.IsKeyPressed(Keyboard::F3)) {
		DebugOverlay::ToggleFPSCounter();
	}
	if (m_keys.IsKeyPressed(Keyboard::F4)) {
		DebugOverlay::ToggleCollisionBox();
	}
	if (m_gameWorld) {
		m_gameWorld->HandleInput(&m_keys);
	}
}

#pragma region Frame Render
// Draws the scene.
void Game::Render() {
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0) {
		return;
	}
	auto context = m_deviceResources->GetD3DDeviceContext();

	// Pass 1
	{
		m_deviceResources->PIXBeginEvent(L"Render game view");

		// Save the current render target and viewport
		ComPtr<ID3D11RenderTargetView> oldRenderTarget;
		ComPtr<ID3D11DepthStencilView> oldDepthStencil;
		context->OMGetRenderTargets(1, oldRenderTarget.GetAddressOf(),
			oldDepthStencil.GetAddressOf());

		D3D11_VIEWPORT oldViewport;
		UINT viewportCount = 1;
		context->RSGetViewports(&viewportCount, &oldViewport);

		ID3D11ShaderResourceView* const nullSRVs[1] = { nullptr };
        context->PSSetShaderResources(0, 1, nullSRVs);

		context->OMSetRenderTargets(1, m_gameRenderTargetView.GetAddressOf(),
			nullptr);
		context->RSSetViewports(1, &m_gameView);

		context->ClearRenderTargetView(m_gameRenderTargetView.Get(),
			Colors::CornflowerBlue);

		// Render the game world to the render target
		m_spriteBatch->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied(),
			m_states->PointClamp());

		// block TODO: move this block into World rendering
		auto frame = m_spriteSheet->Find(L"walk2");
		if (frame != 0)
			m_spriteSheet->Draw(m_spriteBatch.get(), *frame,
				Vector2(GAME_WIDTH / 2, GAME_HEIGHT / 2));

		auto frame1 = m_spriteSheet->Find(L"jump");
		if (frame1 != 0)
			m_spriteSheet->Draw(m_spriteBatch.get(), *frame1,
				Vector2(GAME_WIDTH / 2, GAME_HEIGHT / 2 + 32));
		// endblock

		m_spriteBatch->End();

		// Restore the original render target and viewport
		context->OMSetRenderTargets(1, oldRenderTarget.GetAddressOf(),
			oldDepthStencil.Get());
		context->RSSetViewports(1, &oldViewport);

		m_deviceResources->PIXEndEvent();
	}
	// Pass 2
	{
		Clear();
		m_deviceResources->PIXBeginEvent(L"Render to screen");

		m_spriteBatch->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied(),
			m_states->PointClamp());

		// Draw the game render target to the screen
		m_spriteBatch->Draw(m_gameShaderResource.Get(), m_gameViewRect);


		m_spriteBatch->End();

		m_effect->Apply(context);
		context->IASetInputLayout(m_inputLayout.Get());

		m_primitiveBatch->Begin();

		 DebugOverlay::DrawCollisionBox(m_primitiveBatch.get(), Vector2(GAME_WIDTH
		 / 2, GAME_HEIGHT / 2), Vector2(16, 16), Colors::Lime);
		DebugOverlay::DrawCollisionBox(m_primitiveBatch.get(), { 0, 0, GAME_WIDTH, GAME_HEIGHT }, Colors::Lime);
		m_primitiveBatch->End();

		m_spriteBatch->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied(),
			m_states->PointClamp());

		DebugOverlay::DrawFPSCounter(m_spriteBatch.get(), m_font.get(),
			m_timer.GetFramesPerSecond());
		DebugOverlay::DrawInput(m_spriteBatch.get(), m_font.get());

		m_spriteBatch->End();


		m_deviceResources->PIXEndEvent();
	}

	m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear() {
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();

	context->ClearRenderTargetView(renderTarget, Colors::Black);
	context->OMSetRenderTargets(1, &renderTarget, nullptr);

	// Set the viewport.
	auto const viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated() {
	// TODO: Game is becoming active window.
}

void Game::OnDeactivated() {
	// TODO: Game is becoming background window.
}

void Game::OnSuspending() {
	// TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming() {
	m_timer.ResetElapsedTime();

	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved() {
	auto const r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange() { m_deviceResources->UpdateColorSpace(); }

void Game::OnWindowSizeChanged(int width, int height) {
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	CreateWindowSizeDependentResources();
	// TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept {
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = WND_WIDTH;
	height = WND_HEIGHT;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources() {
	auto device = m_deviceResources->GetD3DDevice();
	auto context = m_deviceResources->GetD3DDeviceContext();

	D3D11_TEXTURE2D_DESC rtDesc = {};
	rtDesc.Width = GAME_WIDTH;
	rtDesc.Height = GAME_HEIGHT;
	rtDesc.MipLevels = 1;
	rtDesc.ArraySize = 1;
	rtDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	rtDesc.SampleDesc.Count = 1;
	rtDesc.SampleDesc.Quality = 0;
	rtDesc.Usage = D3D11_USAGE_DEFAULT;
	rtDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	rtDesc.CPUAccessFlags = 0;
	rtDesc.MiscFlags = 0;

	DX::ThrowIfFailed(device->CreateTexture2D(
		&rtDesc, nullptr, m_gameRenderTarget.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(device->CreateRenderTargetView(
		m_gameRenderTarget.Get(), nullptr,
		m_gameRenderTargetView.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(device->CreateShaderResourceView(
		m_gameRenderTarget.Get(), nullptr,
		m_gameShaderResource.ReleaseAndGetAddressOf()));

	m_spriteBatch = std::make_unique<SpriteBatch>(context);
	m_primitiveBatch =
		std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);
	m_states = std::make_unique<CommonStates>(device);
	m_effect = std::make_unique<BasicEffect>(device);

	auto size = m_deviceResources->GetOutputSize();
	m_effect->SetProjection(XMMatrixOrthographicOffCenterRH(
		0.0f, static_cast<float>(size.right),
		static_cast<float>(size.bottom), 0.0f,
		0.0f, 1.0f));
	m_effect->SetVertexColorEnabled(true);

	DX::ThrowIfFailed(CreateInputLayoutFromEffect<VertexPositionColor>(
		device, m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf()));

	m_font = std::make_unique<SpriteFont>(device, L"textures/mario.spritefont");
	m_spriteSheet = std::make_unique<SpriteSheet>();

	// uncomment to see the defined symbols, might not cover all symbols
	// Helpers::DisplayDefinedSymbols(m_font.get());

	ComPtr<ID3D11Resource> resource;

	/*DX::ThrowIfFailed(
			CreateDDSTextureFromFile(device, L"textures/tiles.dds",
					resource.GetAddressOf(),
					m_texture.ReleaseAndGetAddressOf()));*/

	DX::ThrowIfFailed(CreateWICTextureFromFile(
		device, L"textures/mario/mario.png", resource.GetAddressOf(),
		m_texture.ReleaseAndGetAddressOf()));
	m_spriteSheet->Load(m_texture.Get(), L"textures/mario/mario.txt");

	ComPtr<ID3D11Texture2D> tiles;
	DX::ThrowIfFailed(resource.As(&tiles));

	CD3D11_TEXTURE2D_DESC tilesDesc;
	tiles->GetDesc(&tilesDesc);

	/*DX::ThrowIfFailed(CreateWICTextureFromFile(device,
	L"textures/shipanimated.png", nullptr, m_texture.ReleaseAndGetAddressOf()));

	m_ship = std::make_unique<AnimatedTexture>();
	m_ship->Load(m_texture.Get(), 4,60)*/
	;

	m_origin.x = float(tilesDesc.Width / 2);
	m_origin.y = float(tilesDesc.Height / 2);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources() {
	// Get current window size
	auto size = m_deviceResources->GetOutputSize();
	float windowWidth = static_cast<float>(size.right);
	float windowHeight = static_cast<float>(size.bottom);

	// Calculate aspect ratios
	const float gameAspectRatio = static_cast<float>(GAME_WIDTH) / static_cast<float>(GAME_HEIGHT);
	const float windowAspectRatio = windowWidth / windowHeight;

	// Center positions for UI elements
	m_screenPos = { windowWidth / 2.f, windowHeight / 2.f };
	m_fontPos = m_screenPos;

	// Calculate letterbox/pillarbox dimensions
	float viewWidth, viewHeight;
	float viewX, viewY;

	if (windowAspectRatio > gameAspectRatio) {
		// Window is wider than game - pillarboxing (black bars on sides)
		viewHeight = windowHeight;
		viewWidth = viewHeight * gameAspectRatio;
		viewY = 0.0f;
		viewX = (windowWidth - viewWidth) / 2.0f;
	}
	else {
		// Window is taller than game - letterboxing (black bars on top/bottom)
		viewWidth = windowWidth;
		viewHeight = viewWidth / gameAspectRatio;
		viewX = 0.0f;
		viewY = (windowHeight - viewHeight) / 2.0f;
	}

	// Set the game view rectangle
	m_gameViewRect = {
		static_cast<LONG>(viewX),
		static_cast<LONG>(viewY),
		static_cast<LONG>(viewX + viewWidth),
		static_cast<LONG>(viewY + viewHeight)
	};

	// Calculate scale factor to maintain proper aspect ratio
	float scale = std::min(viewWidth / GAME_WIDTH, viewHeight / GAME_HEIGHT);

	// Create transformation matrices
	XMMATRIX projectionMatrix = XMMatrixOrthographicOffCenterRH(
		0.0f, windowWidth, windowHeight, 0.0f, 0.0f, 1.0f);

	XMMATRIX transformMatrix = XMMatrixScaling(scale, scale, 1.0f) *
		XMMatrixTranslation(viewX, viewY, 0.0f);

	// Apply matrices to effect
	m_effect->SetView(transformMatrix);
	m_effect->SetProjection(projectionMatrix);

	OutputDebugStringA(("[GameView] Scale: " + std::to_string(scale) +
		", Translation X: " + std::to_string(viewX) +
		", Translation Y: " + std::to_string(viewY) + "\n").c_str());
}

void Game::OnDeviceLost() {
	// Add Direct3D resource cleanup here.
	m_gameRenderTarget.Reset();
	m_gameRenderTargetView.Reset();
	m_gameShaderResource.Reset();
	m_spriteBatch.reset();
	m_spriteSheet.reset();
	m_states.reset();
	m_font.reset();
	m_effect.reset();
	m_primitiveBatch.reset();
	m_inputLayout.Reset();
}

void Game::OnDeviceRestored() {
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}

#pragma endregion
