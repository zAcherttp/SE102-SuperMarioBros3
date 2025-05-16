#pragma once
#include "Entity.h"

using namespace DirectX::SimpleMath;

class Block : public Entity {
public:
    Block(Vector2 position, Vector2 size, SpriteSheet* spriteSheet);
    Block(Vector2 position, SpriteSheet* spriteSheet);
    ~Block() = default;

    bool IsSolid() const;
    void SetSolid(bool solid);

    bool IsCollectible() const;

    void SetIsClaimed(bool claimed);

    virtual void Collect();
    virtual void Bump();
    virtual void Break();

protected:
    bool m_isSolid;
    bool m_isCollectible;

    bool m_isClaimed;
};

