#pragma once  

#include <SpriteBatch.h>  
#include <SpriteFont.h>  
#include "StepTimer.h" 
#include "Mario.h"
#include "World.h"

using namespace DirectX;  
using namespace DirectX::SimpleMath;  

class DebugOverlay  
{  
private:
	static bool m_drawFPSCounter;
	static bool m_drawCollisionBox;
	//W, A, S, D, J, K, I
    static bool m_keyState[7];

    static std::pair<std::string, bool> m_marioState;

public:
    DebugOverlay() = delete;  
    DebugOverlay(const DebugOverlay&) = delete;  
    DebugOverlay& operator=(const DebugOverlay&) = delete;  

    //BoundingBox rendering + fps counter + key states
    static void DrawFPSCounter(SpriteBatch* spriteBatch,  
        SpriteFont* font,  
        uint32_t fps);
    static void DrawInput(SpriteBatch* spriteBatch,  
        SpriteFont* font);
    static void DrawMarioState(SpriteBatch* spriteBatch,  
        SpriteFont* font);
    static void DrawLine(PrimitiveBatch<VertexPositionColor>* primitiveBatch,  
        Vector2 start,  
        Vector2 end,  
        GXMVECTOR color = Colors::White);
	static void UpdateInput(Keyboard::State* kbState);
	static void UpdateMarioState(Mario* mario);
    static void DrawBoundingBox(PrimitiveBatch<VertexPositionColor>* primitiveBatch,  
        Vector2 position,  
        Vector2 size,  
        GXMVECTOR color = Colors::White);  
    static void DrawBoundingBox(PrimitiveBatch<VertexPositionColor>* primitiveBatch,  
        RECT rect,  
        GXMVECTOR color = Colors::White);
    static void DrawQuad(PrimitiveBatch<VertexPositionColor>* primitiveBatch,
        Vector2 position,
        Vector2 size,
        GXMVECTOR color = Colors::White);
    static void DrawQuad(PrimitiveBatch<VertexPositionColor>* primitiveBatch,
        RECT rect,
        GXMVECTOR color = Colors::White);
    static void ToggleFPSCounter();
	static void ToggleCollisionBox();
};
