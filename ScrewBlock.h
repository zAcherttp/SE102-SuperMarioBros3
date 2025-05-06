#pragma once
#include "Block.h"

class ScrewBlock : public Block
{
public:
    ScrewBlock(Vector2 position, Vector2 size, int countX, int countY, bool isSolid, SpriteSheet* spriteSheet, float depth = 0.0f, int color = 0, bool isFloating = false); // 0: white, 1: pink, 2: blue, 3: green

    void Update(float dt) override;

	//here we use m_size to render too
    void Render(DirectX::SpriteBatch* spriteBatch) override;

    enum ScrewBlockPosition {
        TOP_LEFT,
        TOP_MID,
        TOP_RIGHT,
        MID_LEFT,
        MID_MID,
        MID_RIGHT,
        BOT_LEFT,
        BOT_MID,
        BOT_RIGHT
    };

    // This would be an alternative approach if your sprite IDs follow a pattern
    int GetSpriteID(int color, ScrewBlockPosition position);


private:
    int m_tileXcount;
    int m_tileYcount;
    float m_depth;
    int m_color; // 0: white, 1: pink, 2: blue, 3: green
    bool m_isFloating;
    

};

