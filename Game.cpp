//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "Helpers.h"
#include "DebugOverlay.h"

constexpr auto GAME_WIDTH = 512;
constexpr auto GAME_HEIGHT = 512;

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
void Game::Initialize(HWND window, int width, int height)
{
	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	// Renders only 2D, so no need for a depth buffer.
	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	m_keyboard = std::make_unique<Keyboard>();

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
void Game::Update(DX::StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());

	if (m_gameWorld)
	{
		/*m_gameWorld->Update(elapsedTime);*/
	}

	HandleInput();
	elapsedTime;


}
#pragma endregion

void Game::HandleInput()
{
	auto kb = m_keyboard->GetState();
	m_keys.Update(kb);
	DebugOverlay::UpdateInput(&kb);
	if (m_keys.IsKeyPressed(Keyboard::Escape))
	{
		ExitGame();
	}
	if (m_keys.IsKeyPressed(Keyboard::F3))
	{
		DebugOverlay::ToggleFPSCounter();
	}
	if (m_keys.IsKeyPressed(Keyboard::F4))
	{
		DebugOverlay::ToggleCollisionBox();
	}
	if (m_gameWorld)
	{
		m_gameWorld->HandleInput(&m_keys);
	}

}

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	Clear();

	m_deviceResources->PIXBeginEvent(L"Render");
	auto context = m_deviceResources->GetD3DDeviceContext();

	//m_states->PointClamp(); for nearest neighbor sampling a.k.a pixelated look
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied(), m_states->PointClamp());

	/// todo: put this into World class rendering
	XMVECTOR pos = XMLoadFloat2(&m_screenPos);


	OutputDebugString(L"pos: ");
	OutputDebugStringW(std::to_wstring(XMVectorGetX(pos)).c_str());
	OutputDebugString(L", ");
	OutputDebugStringW(std::to_wstring(XMVectorGetY(pos)).c_str());
	OutputDebugString(L"\n");

	//halve the pos vector
	pos = XMVectorMultiply(pos, XMVectorSet(0.5f, 0.5f, 0.f, 1.f));

	auto frame = m_spriteSheet->Find(L"walk2");
	assert(frame != 0);

	m_spriteSheet->Draw(m_spriteBatch.get(), *frame, pos);

	auto frame1 = m_spriteSheet->Find(L"jump");
	assert(frame1 != 0);

	m_spriteSheet->Draw(m_spriteBatch.get(), *frame1, pos);


	/// end block

	DebugOverlay::DrawFPSCounter(m_spriteBatch.get(), m_font.get(), m_timer.GetFramesPerSecond());
	DebugOverlay::DrawInput(m_spriteBatch.get(), m_font.get());
	m_spriteBatch->End();

	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthNone(), 0);
	context->RSSetState(m_states->CullNone());

	m_effect->Apply(context);

	context->IASetInputLayout(m_inputLayout.Get());

	m_primitiveBatch->Begin();

	RECT viewPort = m_deviceResources->GetOutputSize();
	DebugOverlay::DrawCollisionBox(m_primitiveBatch.get(), pos, Vector2(16, 16), Colors::Lime);
	DebugOverlay::DrawLine(m_primitiveBatch.get(), Vector2(0, 300), Vector2(256, 300), Colors::Lime);
	DebugOverlay::DrawLine(m_primitiveBatch.get(), Vector2(512, 290), Vector2(256, 290), Colors::Lime);
	DebugOverlay::DrawCollisionBox(m_primitiveBatch.get(), Vector2(0, 0), Vector2(viewPort.right, viewPort.bottom), Colors::Lime);

	m_primitiveBatch->End();

	m_deviceResources->PIXEndEvent();

	// Show the new frame.
	m_deviceResources->Present();
}


// Helper method to clear the back buffers.
void Game::Clear()
{
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
void Game::OnActivated()
{
	// TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
	// TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
	// TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
	m_timer.ResetElapsedTime();

	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
	auto const r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
	m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	CreateWindowSizeDependentResources();
	// TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
    width = GAME_WIDTH;
    height = GAME_HEIGHT;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
	auto device = m_deviceResources->GetD3DDevice();
	auto context = m_deviceResources->GetD3DDeviceContext();
	m_spriteBatch = std::make_unique<SpriteBatch>(context);
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);
	m_states = std::make_unique<CommonStates>(device);
	m_effect = std::make_unique<BasicEffect>(device);

	int width, height;
	GetDefaultSize(width, height);

	m_effect->SetProjection(XMMatrixOrthographicOffCenterRH(0,
		(float)width, (float)height, 0, 0, 1));
	m_effect->SetVertexColorEnabled(true);

	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionColor>(device, m_effect.get(),
			m_inputLayout.ReleaseAndGetAddressOf())
	);


	m_font = std::make_unique<SpriteFont>(device, L"textures/mario.spritefont");
	m_spriteSheet = std::make_unique<SpriteSheet>();

	// uncomment to see the defined symbols, might not cover all symbols
	//Helpers::DisplayDefinedSymbols(m_font.get());

	ComPtr<ID3D11Resource> resource;

	/*DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"textures/tiles.dds",
			resource.GetAddressOf(),
			m_texture.ReleaseAndGetAddressOf()));*/

	DX::ThrowIfFailed(
		CreateWICTextureFromFile(device, L"textures/mario/mario.png",
			resource.GetAddressOf(),
			m_texture.ReleaseAndGetAddressOf()));
	m_spriteSheet->Load(m_texture.Get(), L"textures/mario/mario.txt");

	ComPtr<ID3D11Texture2D> tiles;
	DX::ThrowIfFailed(resource.As(&tiles));

	CD3D11_TEXTURE2D_DESC tilesDesc;
	tiles->GetDesc(&tilesDesc);


	/*DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"textures/shipanimated.png",
		nullptr, m_texture.ReleaseAndGetAddressOf()));

	m_ship = std::make_unique<AnimatedTexture>();
	m_ship->Load(m_texture.Get(), 4,60)*/;


	m_origin.x = float(tilesDesc.Width / 2);
	m_origin.y = float(tilesDesc.Height / 2);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	auto size = m_deviceResources->GetOutputSize();

	m_screenPos.x = size.right / 2.f;
	m_screenPos.y = size.bottom / 2.f;
	m_fontPos.x = size.right / 2.f;
	m_fontPos.y = size.bottom / 2.f;

	/*m_shipPos.x = float(size.right / 2);
	m_shipPos.y = float((size.bottom / 2) + (size.bottom / 4));*/
}

void Game::OnDeviceLost()
{
	// Add Direct3D resource cleanup here.
	m_texture.Reset();
	m_spriteBatch.reset();
	m_spriteSheet.reset();
	m_states.reset();
	m_font.reset();
	m_effect.reset();
	m_primitiveBatch.reset();
	m_inputLayout.Reset();
}

void Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}

#pragma endregion
