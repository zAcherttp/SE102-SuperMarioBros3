#include "pch.h"
#include "DebugOverlay.h"

bool DebugOverlay::m_drawFPSCounter = true;
bool DebugOverlay::m_drawCollisionBox = true;

void DebugOverlay::DrawFPSCounter(DirectX::SpriteBatch* spriteBatch, DirectX::SpriteFont* font, uint32_t fps)
{
	if (!m_drawFPSCounter) return;
	wchar_t fpsOutput[16]{};
	swprintf_s(fpsOutput, L"fps %d", fps);
	font->DrawString(spriteBatch, fpsOutput,
		Vector2(10, 10), Colors::White, 0.f, Vector2::Zero, 3.f);
}

void DebugOverlay::DrawLine(PrimitiveBatch<VertexPositionColor>* primitiveBatch, Vector2 start, Vector2 end, GXMVECTOR color)
{
	if (!m_drawCollisionBox) return;
	VertexPositionColor v1(start, color);
	VertexPositionColor v2(end, color);

	primitiveBatch->DrawLine(v1, v2);
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