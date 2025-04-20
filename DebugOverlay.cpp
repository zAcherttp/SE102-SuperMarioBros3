#include "pch.h"
#include "DebugOverlay.h"

bool DebugOverlay::m_draw = true;

void DebugOverlay::DrawFPSCounter(DirectX::SpriteBatch* spriteBatch, DirectX::SpriteFont* font, uint32_t fps)
{
	if (!m_draw) return;
	wchar_t fpsOutput[16]{};
	swprintf_s(fpsOutput, L"fps %d", fps);
	font->DrawString(spriteBatch, fpsOutput,
		Vector2(10, 10), Colors::White, 0.f, Vector2::Zero, 3.f);
}

void DebugOverlay::DrawLine(PrimitiveBatch<VertexPositionColor>* primitiveBatch, Vector2 start, Vector2 end, GXMVECTOR color)
{
	if (!m_draw) return;
	VertexPositionColor v1(start, color);
	VertexPositionColor v2(end, color);

	primitiveBatch->DrawLine(v1, v2);
}

void DebugOverlay::DrawCollisionBox(PrimitiveBatch<VertexPositionColor>* primitiveBatch, Vector2 position, Vector2 size, GXMVECTOR color)
{
	if (!m_draw) return;
	Vector2 halfSize = size / 2.f;
	Vector2 topLeft = position - halfSize;
	Vector2 topRight = position + Vector2(halfSize.x, -halfSize.y);
	Vector2 bottomLeft = position + Vector2(-halfSize.x, halfSize.y);
	Vector2 bottomRight = position + halfSize;
	primitiveBatch->DrawQuad(
		VertexPositionColor(topLeft, color),
		VertexPositionColor(topRight, color),
		VertexPositionColor(bottomRight, color),
		VertexPositionColor(bottomLeft, color)
	);
}

void DebugOverlay::DrawCollisionBox(PrimitiveBatch<VertexPositionColor>* primitiveBatch, RECT rect, GXMVECTOR color)
{
	if (!m_draw) return;
	Vector2 topLeft = Vector2(static_cast<float>(rect.left), static_cast<float>(rect.top));
	Vector2 topRight = Vector2(static_cast<float>(rect.right), static_cast<float>(rect.top));
	Vector2 bottomLeft = Vector2(static_cast<float>(rect.left), static_cast<float>(rect.bottom));
	Vector2 bottomRight = Vector2(static_cast<float>(rect.right), static_cast<float>(rect.bottom));
	primitiveBatch->DrawQuad(
		VertexPositionColor(topLeft, color),
		VertexPositionColor(topRight, color),
		VertexPositionColor(bottomRight, color),
		VertexPositionColor(bottomLeft, color)
	);
}

void DebugOverlay::Toggle()
{
	m_draw = !m_draw;
}
