#include "pch.h"
#include "DebugOverlay.h"

bool DebugOverlay::m_drawFPSCounter = true;
bool DebugOverlay::m_drawCollisionBox = true;
bool DebugOverlay::m_keyState[7] = { false };

/// <summary>
/// Draws an FPS (frames per second) counter on the screen using a sprite batch and font.
/// </summary>
/// <param name="spriteBatch">A pointer to the DirectX::SpriteBatch object used for rendering the FPS counter.</param>
/// <param name="font">A pointer to the DirectX::SpriteFont object used to render the text of the FPS counter.</param>
/// <param name="fps">The current frames per second value to display.</param>
void DebugOverlay::DrawFPSCounter(DirectX::SpriteBatch* spriteBatch, DirectX::SpriteFont* font, uint32_t fps)
{
	if (!m_drawFPSCounter) return;
	wchar_t fpsOutput[16]{};
	swprintf_s(fpsOutput, L"fps %d", fps);
	font->DrawString(spriteBatch, fpsOutput,
		Vector2(10, 10), Colors::White, 0.f, Vector2::Zero, 2.f);
}

/// <summary>
/// Renders a debug overlay displaying the state of specific input keys.
/// </summary>
/// <param name="spriteBatch">A pointer to the SpriteBatch object used for rendering the text.</param>
/// <param name="font">A pointer to the SpriteFont object used to render the text strings.</param>
void DebugOverlay::DrawInput(SpriteBatch* spriteBatch, SpriteFont* font)
{
	if (!m_drawFPSCounter) return;
	wchar_t string[128]{};
	if (m_keyState[0]) 
		wcsncat_s(string, L"w down\n", 8);
	else
		wcsncat_s(string, L"w up\n", 7);
	if (m_keyState[1])
		wcsncat_s(string, L"a down\n", 8);
	else
		wcsncat_s(string, L"a up\n", 7);
	if (m_keyState[2])
		wcsncat_s(string, L"s down\n", 8);
	else
		wcsncat_s(string, L"s up\n", 7);
	if (m_keyState[3])
		wcsncat_s(string, L"d down\n", 8);
	else
		wcsncat_s(string, L"d up\n", 7);
	if (m_keyState[4])
		wcsncat_s(string, L"j down\n", 8);
	else
		wcsncat_s(string, L"j up\n", 7);
	if(m_keyState[5])
		wcsncat_s(string, L"k down\n", 8);
	else
		wcsncat_s(string, L"k up\n", 7);
	if (m_keyState[6])
		wcsncat_s(string, L"i down\n", 8);
	else
		wcsncat_s(string, L"i up\n", 7);
	font->DrawString(spriteBatch, string, Vector2(10, 30), Colors::White, 0.f, Vector2::Zero, 2.f);
}

void DebugOverlay::DrawLine(PrimitiveBatch<VertexPositionColor>* primitiveBatch, Vector2 start, Vector2 end, GXMVECTOR color)
{
	if (!m_drawCollisionBox) return;
	VertexPositionColor v1(start, color);
	VertexPositionColor v2(end, color);

	primitiveBatch->DrawLine(v1, v2);
}

void DebugOverlay::UpdateInput(Keyboard::State* kbState)
{
	m_keyState[0] = kbState->W;
	m_keyState[1] = kbState->A;
	m_keyState[2] = kbState->S;
	m_keyState[3] = kbState->D;
	m_keyState[4] = kbState->J;
	m_keyState[5] = kbState->K;
	m_keyState[6] = kbState->I;
}

/// <summary>
/// Draws a collision box on the debug overlay using the specified position, size, and color.
/// </summary>
/// <param name="primitiveBatch">A pointer to the PrimitiveBatch object used for rendering the collision box.</param>
/// <param name="pos">The center position of the collision box, represented as a 2D vector.</param>
/// <param name="size">The size of the collision box, represented as a 2D vector (width and height).</param>
/// <param name="color">The color of the collision box, represented as a GXMVECTOR.</param>
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
	VertexPositionColor topLeft(Vector2((float)rect.left, (float)rect.top), color);
	VertexPositionColor topRight(Vector2((float)rect.right, (float)rect.top), color);
	VertexPositionColor bottomLeft(Vector2((float)rect.left, (float)rect.bottom), color);
	VertexPositionColor bottomRight(Vector2((float)rect.right, (float)rect.bottom), color);
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