﻿//
// Game.cpp
//

#include "pch.h"
#include "Debug.h"
#include "DebugOverlay.h"
#include "Game.h"
#include "Helpers.h"
#include "Mario.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;
Game* Game::s_instance = nullptr;

Game::Game() noexcept(false)
{
	s_instance = this;
	m_deviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_FORMAT_UNKNOWN);
	// TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
	//   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
	//   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
	m_deviceResources->RegisterDeviceNotify(this);
	m_gameView = {};
	m_currentWorldId = m_nextWorldId = -1;
	m_gameTitle = L"";
	m_gameWidth = m_gameHeight = m_wndHeight = m_wndWidth = 0;
	m_isLoading = true;
	m_requestReset = false;
}

Game* Game::GetInstance() { return s_instance; }

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height) {
	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	// Renders only 2D, so no need for a depth buffer.
	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	m_keyboard = std::make_unique<Keyboard>();
	m_keys = std::make_unique<Keyboard::KeyboardStateTracker>();
	m_hud = std::make_unique<HeadUpDisplay>(m_spriteSheet.get());

	m_gameView.TopLeftX = 0.0f;
	m_gameView.TopLeftY = 0.0f;
	m_gameView.Width = static_cast<float>(m_gameWidth);
	m_gameView.Height = static_cast<float>(m_gameHeight);
	m_gameView.MinDepth = 0.0f;
	m_gameView.MaxDepth = 1.0f;

	// uncomment to use the fixed timestep
	//m_timer.SetFixedTimeStep(true);
	//m_timer.SetTargetElapsedSeconds(1.0 / 60);
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

	if (m_isLoading) return;

	float elapsedTime = float(timer.GetElapsedSeconds());

	HandleInput();

	if (m_worlds[m_currentWorldId]) {
		m_worlds[m_currentWorldId]->Update(elapsedTime);
		m_camera->Update(elapsedTime);
	}

	if (m_nextWorldId != m_currentWorldId || m_requestReset) {
		SwitchWorld();
		m_requestReset = false;
	}
	m_effectManager->Update(elapsedTime);
}

#pragma endregion

void Game::HandleInput() {
	auto kbs = m_keyboard->GetState();
	m_keys->Update(kbs);
	DebugOverlay::UpdateInput(&kbs);
	if (m_keys->IsKeyPressed(Keyboard::Escape)) {
		ExitGame();
	}
	if (m_keys->IsKeyPressed(Keyboard::F3)) {
		DebugOverlay::ToggleFPSCounter();
	}
	if (m_keys->IsKeyPressed(Keyboard::F4)) {
		DebugOverlay::ToggleCollisionBox();
	}
	if (m_keys->IsKeyPressed(Keyboard::F5)) {
		//function testing key
		m_camera->Shake(Vector2(0.f, 4.f), 8, .25f);
	}
	if (m_worlds[m_currentWorldId]) {
		m_worlds[m_currentWorldId]->HandleInput(&kbs, m_keys.get());
		DebugOverlay::UpdateMarioState((Mario*)m_worlds[m_currentWorldId]->GetPlayer());
	}
}

void Game::UpdateHUD(float dt)
{
	m_hud->Update(dt);
}

void Game::AddScore(const int& score)
{
	m_hud->AddScore(score);
}

void Game::AddCoin(const int& coin)
{
	m_hud->AddCoins(coin);
}

void Game::SetEnterPosition(const Vector2& pos)
{
	m_worldStartPosition = pos;
}

void Game::RestartWorld()
{
	m_requestReset = true;

	m_hud->SetLives(m_hud->GetLives() - 1);

	Log(LOG_INFO, "World restarting");
}

#pragma region Frame Render
// Draws the scene.
void Game::Render() {
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0 || m_isLoading) {
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
			GetCurrentWorld()->GetBackgroundColor());

		// Render the game world to the render target
		m_spriteBatch->Begin(SpriteSortMode_BackToFront, m_states->NonPremultiplied(),
			m_states->PointClamp(), nullptr, nullptr, nullptr, m_camera->GetGameViewMatrix());

		GetCurrentWorld()->Render(m_spriteBatch.get());

		m_effectManager->Render(m_spriteBatch.get());

		m_spriteBatch->End();

		// Restore the original render target and viewport
		context->OMSetRenderTargets(1, oldRenderTarget.GetAddressOf(),
			oldDepthStencil.Get());
		context->RSSetViewports(1, &oldViewport);

		m_deviceResources->PIXEndEvent();
	}
	// Pass 2
	{
		ClearBackBuffer();
		m_deviceResources->PIXBeginEvent(L"Render to screen");

		m_spriteBatch->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied(),
			m_states->PointClamp());

		// Draw the game render target to the screen
		const RECT& gameRect = m_camera->GetGameViewRect();
		m_spriteBatch->Draw(m_gameShaderResource.Get(), gameRect);
		//Log(LOG_INFO, "HUD position: " + std::to_string(m_gameView.TopLeftX) + ", " + std::to_string(m_gameView.TopLeftY));
		// Draw HUD
		m_hud->Render(m_spriteBatch.get(), m_font.get(), gameRect);

		m_spriteBatch->End();

		m_effect->SetView(m_camera->GetDebugViewMatrix());
		m_effect->SetProjection(m_camera->GetDebugProjectionMatrix());

		m_effect->Apply(context);
		context->IASetInputLayout(m_inputLayout.Get());

		m_primitiveBatch->Begin();

		GetCurrentWorld()->RenderDebug(m_primitiveBatch.get());

		m_primitiveBatch->End();

		m_spriteBatch->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied(),
			m_states->PointClamp());

		DebugOverlay::DrawFPSCounter(m_spriteBatch.get(), m_font.get(),
			m_timer.GetFramesPerSecond());
		DebugOverlay::DrawInput(m_spriteBatch.get(), m_font.get());
		DebugOverlay::DrawMarioState(m_spriteBatch.get(), m_font.get());

		m_spriteBatch->End();


		m_deviceResources->PIXEndEvent();
	}

	m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::ClearBackBuffer() {
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
#pragma endregion

#pragma region Game Properties
// Properties
void Game::GetDefaultWndSize(int& width, int& height) const noexcept {
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = m_wndWidth;
	height = m_wndHeight;
}

void Game::GetDefaultGameSize(int& width, int& height) const noexcept {
	width = m_gameWidth;
	height = m_gameHeight;
}

void Game::GetDefaultGameTitle(LPCWSTR& title) const noexcept {
	title = m_gameTitle.c_str();
}

#pragma endregion

#pragma region Game Loading
World* Game::GetCurrentWorld()
{
	return m_worlds[m_currentWorldId];
}

bool Game::LoadGame(const std::string& filePath)
{
	try {
		std::ifstream gameFile(filePath);
		if (!gameFile.is_open()) {
			Log(__FUNCTION__, "Failed to open file: " + filePath);
			return false;
		}

		json gameData{};
		gameFile >> gameData;

		LoadGameConfig(gameData["config"]);
		LoadWorldConfig(gameData["worlds"]);
	}
	catch (const std::exception& e) {
		Log(__FUNCTION__, "Exception occurred: " + std::string(e.what()));
	}
	m_isLoading = false;
	return true;
}

void Game::LoadGameConfig(const json& config)
{
	m_gameWidth = config["game"]["width"];
	m_gameHeight = config["game"]["height"];

	m_wndWidth = config["window"]["width"];
	m_wndHeight = config["window"]["height"];
	Log(__FUNCTION__, "Window size set to: " + std::to_string(m_wndWidth) + "x" + std::to_string(m_wndHeight));

	m_startWorldId = m_nextWorldId = config["game"]["world"]["startWorldId"];
	Log(__FUNCTION__, "Start world id: " + std::to_string(m_nextWorldId));



	std::string spritePath = config["game"]["sprites"]["path"];
	m_spritePath = std::wstring(spritePath.begin(), spritePath.end());

	std::string spriteDataPath = config["game"]["sprites"]["data"];
	m_spriteDataPath = std::wstring(spriteDataPath.begin(), spriteDataPath.end());

	std::string fontPath = config["game"]["sprites"]["font"];
	m_spriteFontPath = std::wstring(fontPath.begin(), fontPath.end());

	std::string title(config["game"]["title"]);
	m_gameTitle = std::wstring(title.begin(), title.end());

	Log(__FUNCTION__, "Window title set to: " + title);
}

void Game::LoadWorldConfig(const json& config) {
	if (!config.is_array()) {
		throw std::runtime_error("Worlds config must be an array");
	}

	for (const auto& w : config) {

		int id = w["id"].get<int>();
		std::string path = w["path"].get<std::string>();
		std::string name = w["name"].get<std::string>();

		m_worlds[id] = new World(path, name);
		Log(__FUNCTION__, "World loaded: " + name);
	}
}

void Game::SetNextWorldId(int id)
{
	m_nextWorldId = id;
}

void Game::SetWorldSize(int width, int height)
{
	m_camera->SetWorldSize(width, height);
}

void Game::SetCameraPosition(const Vector2& pos, bool oneAxis)
{
	m_camera->SetPosition(pos, oneAxis);
}

void Game::MoveCamera(const DirectX::SimpleMath::Vector2& delta)
{
	m_camera->Move(delta);
}

SpriteSheet* Game::GetSpriteSheet() const
{
	return m_spriteSheet.get();
}

SpriteBatch* Game::GetSpriteBatch() const
{
	return m_spriteBatch.get();
}

void Game::SwitchWorld()
{
	if (m_nextWorldId < 0) return;
	m_isLoading = true;

	if (m_worlds[m_currentWorldId] != NULL)
		m_worlds[m_currentWorldId]->Unload();

	m_currentWorldId = m_nextWorldId;

	//TODO: clean up sprites/anims

	// Load world
	World* world = m_worlds[m_currentWorldId];
	Log(__FUNCTION__, "Loading into world: " + world->GetName());
	world->Load(m_spriteSheet.get());

	if (m_worldStartPosition.x > 0 && m_worldStartPosition.y > 0) {
		world->GetPlayer()->SetPosition(m_worldStartPosition);
		m_worldStartPosition = Vector2(-1, -1);
	}

	m_camera->SetPosition(Vector2(0.f, 0.f), false);

	m_isLoading = false;

	m_hud->SetLives(3);
	m_hud->SetScore(0);
	m_hud->SetCoins(0);
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources() {
	auto device = m_deviceResources->GetD3DDevice();
	auto context = m_deviceResources->GetD3DDeviceContext();

	m_camera = std::make_unique<Camera>(m_gameWidth, m_gameHeight);

	D3D11_TEXTURE2D_DESC rtDesc = {};
	rtDesc.Width = m_gameWidth;
	rtDesc.Height = m_gameHeight;
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
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	m_states = std::make_unique<CommonStates>(device);

	m_effect = std::make_unique<BasicEffect>(device);
	m_effect->SetVertexColorEnabled(true);
	m_effect->SetWorld(Matrix::Identity);

	m_effectManager = std::make_unique<EffectManager>(m_spriteSheet.get());

	DX::ThrowIfFailed(CreateInputLayoutFromEffect<VertexPositionColor>(
		device, m_effect.get(), m_inputLayout.ReleaseAndGetAddressOf()));

	m_font = std::make_unique<SpriteFont>(device, m_spriteFontPath.c_str());
	m_spriteSheet = std::make_unique<SpriteSheet>();

	// uncomment to see the defined symbols, might not cover all symbols
	// Helpers::DisplayDefinedSymbols(m_font.get());

	ComPtr<ID3D11Resource> resource;

	DX::ThrowIfFailed(CreateWICTextureFromFile(
		device, m_spritePath.c_str(), resource.GetAddressOf(),
		m_texture.ReleaseAndGetAddressOf()));
	m_spriteSheet->Load(m_texture.Get(), m_spriteDataPath.c_str());

	ComPtr<ID3D11Texture2D> tiles;
	DX::ThrowIfFailed(resource.As(&tiles));

	CD3D11_TEXTURE2D_DESC tilesDesc;
	tiles->GetDesc(&tilesDesc);

	m_origin.x = float(tilesDesc.Width / 2);
	m_origin.y = float(tilesDesc.Height / 2);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources() {
	// Get current window size
	auto size = m_deviceResources->GetOutputSize();
	float windowWidth = static_cast<float>(size.right);
	float windowHeight = static_cast<float>(size.bottom);

	if (m_camera) {
		m_camera->UpdateWindowSizeDependentMatrices((int)windowWidth, (int)windowHeight);
	}

	m_effect->SetView(m_camera->GetDebugViewMatrix());
	m_effect->SetProjection(m_camera->GetDebugProjectionMatrix());
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
