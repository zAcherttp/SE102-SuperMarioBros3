#pragma once
#include "Entity.h"

using namespace DirectX::SimpleMath;

class Block : public Entity {
public:
    Block(Vector2 position, Vector2 size, SpriteSheet* spriteSheet);
    Block(Vector2 position, SpriteSheet* spriteSheet);
    virtual ~Block() = default;

    bool IsSolid() const { return m_isSolid; }
    void SetSolid(bool solid) { m_isSolid = solid; }

protected:
    bool m_isSolid = true;
};

