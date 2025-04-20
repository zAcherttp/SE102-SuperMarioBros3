#include "pch.h"
#include "DebugOverlay.h"

bool DebugOverlay::m_drawFPSCounter = true;
bool DebugOverlay::m_drawCollisionBox = true;
bool DebugOverlay::m_keyState[7] = { false };

void DebugOverlay::DrawFPSCounter(DirectX::SpriteBatch* spriteBatch, DirectX::SpriteFont* font, uint32_t fps)
{
	if (!m_drawFPSCounter) return;
	wchar_t fpsOutput[16]{};
	swprintf_s(fpsOutput, L"fps %d", fps);
	font->DrawString(spriteBatch, fpsOutput,
		Vector2(10, 10), Colors::White, 0.f, Vector2::Zero, 2.f);
}

void DebugOverlay::DrawInput(SpriteBatch* spriteBatch, SpriteFont* font)
{
	if (!m_drawFPSCounter) return;
	wchar_t string[128]{};
	if (m_keyState[0]) 
		wcsncat(string, L"w down\n", 8);
	else
		wcsncat(string, L"w up\n", 7);
	if (m_keyState[1])
		wcsncat(string, L"a down\n", 8);
	else
		wcsncat(string, L"a up\n", 7);
	if (m_keyState[2])
		wcsncat(string, L"s down\n", 8);
	else
		wcsncat(string, L"s up\n", 7);
	if (m_keyState[3])
		wcsncat(string, L"d down\n", 8);
	else
		wcsncat(string, L"d up\n", 7);
	if (m_keyState[4])
		wcsncat(string, L"j down\n", 8);
	else
		wcsncat(string, L"j up\n", 7);
	if(m_keyState[5])
		wcsncat(string, L"k down\n", 8);
	else
		wcsncat(string, L"k up\n", 7);
	if (m_keyState[6])
		wcsncat(string, L"i down\n", 8);
	else
		wcsncat(string, L"i up\n", 7);
	font->DrawString(spriteBatch, string, Vector2(10, 30), Colors::White, 0.f, Vector2::Zero, 2.f);
}

void DebugOverlay::DrawLine(PrimitiveBatch<VertexPositionColor>* primitiveBatch, Vector2 start, Vector2 end, GXMVECTOR color)
{
	if (!m_drawCollisionBox) return;
	VertexPositionColor v1(start, color);
	VertexPositionColor v2(end, color);

	primitiveBatch->DrawLine(v1, v2);
}

void DebugOverlay::UpdateInput(Keyboard::State kbState)
{
	m_keyState[0] = kbState.W;
	m_keyState[1] = kbState.A;
	m_keyState[2] = kbState.S;
	m_keyState[3] = kbState.D;
	m_keyState[4] = kbState.J;
	m_keyState[5] = kbState.K;
	m_keyState[6] = kbState.I;
}

void DebugOverlay::DrawCollisionBox(PrimitiveBatch<VertexPositionColor>* primitiveBatch, Vector2 pos, Vector2 size, GXMVECTOR color)
{
	if (!m_drawCollisionBox) return;
	VertexPositionColor topLeft(pos - Vector2(size.x / 2, size.y / 2), color);
	VertexPositionColor topRight(pos + Vector2(size.x / 2, -size.y / 2), color);
	VertexPositionColor bottomLeft(pos + Vector2(-size.x / 2, size.y / 2), color);
	VertexPositionColor bottomRight(pos + Vector2(size.x / 2, size.y / 2), color);
	primitiveBatch->DrawLine(topLeft, topRight);
	primitiveBatch->DrawLine(topRight, bottomRight);
	primitiveBatch->DrawLine(bottomRight, bottomLeft);
	primitiveBatch->DrawLine(bottomLeft, topLeft);
}

void DebugOverlay::DrawCollisionBox(PrimitiveBatch<VertexPositionColor>* primitiveBatch, RECT rect, GXMVECTOR color)
{
	if (!m_drawCollisionBox) return;
	VertexPositionColor topLeft(Vector2(rect.left, rect.top), color);
	VertexPositionColor topRight(Vector2(rect.right, rect.top), color);
	VertexPositionColor bottomLeft(Vector2(rect.left, rect.bottom), color);
	VertexPositionColor bottomRight(Vector2(rect.right, rect.bottom), color);
	primitiveBatch->DrawLine(topLeft, topRight);
	primitiveBatch->DrawLine(topRight, bottomRight);
	primitiveBatch->DrawLine(bottomRight, bottomLeft);
	primitiveBatch->DrawLine(bottomLeft, topLeft);
}

void DebugOverlay::ToggleCollisionBox()
{
	m_drawCollisionBox = !m_drawCollisionBox;
}

void DebugOverlay::ToggleFPSCounter()
{
	m_drawFPSCounter = !m_drawFPSCounter;
}