#pragma once  

#include <SpriteBatch.h>  
#include <SpriteFont.h>  
#include "StepTimer.h"  

using namespace DirectX;  
using namespace DirectX::SimpleMath;  

class DebugOverlay  
{  
private:
	static bool m_drawFPSCounter;
	static bool m_drawCollisionBox;
	//W, A, S, D, J, K, I
    static bool m_keyState[7];

public:
    DebugOverlay() = delete;  
    DebugOverlay(const DebugOverlay&) = delete;  
    DebugOverlay& operator=(const DebugOverlay&) = delete;  

    static void DrawFPSCounter(SpriteBatch* spriteBatch,  
        SpriteFont* font,  
        uint32_t fps);
    static void DrawInput(SpriteBatch* spriteBatch,  
        SpriteFont* font);
    static void DrawLine(PrimitiveBatch<VertexPositionColor>* primitiveBatch,  
        Vector2 start,  
        Vector2 end,  
        GXMVECTOR color = Colors::White);
	static void UpdateInput(Keyboard::State* kbState);
	// TODO: change the param to array of positions and size from gameWorld to draw
    static void DrawCollisionBox(PrimitiveBatch<VertexPositionColor>* primitiveBatch,  
        Vector2 position,  
        Vector2 size,  
        GXMVECTOR color = Colors::White);  
    static void DrawCollisionBox(PrimitiveBatch<VertexPositionColor>* primitiveBatch,  
        RECT rect,  
        GXMVECTOR color = Colors::White);  
    static void ToggleFPSCounter();
	static void ToggleCollisionBox();
};
