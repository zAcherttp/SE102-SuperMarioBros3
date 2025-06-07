#pragma once
#include "Block.h"

class HiddenButton : public Block
{
public:
    HiddenButton(Vector2 position, Vector2 size, SpriteSheet* spriteSheet);

    void Update(float dt) override;
    void Render(DirectX::SpriteBatch* spriteBatch) override;

    void OnCollision(const CollisionResult& event) override;
    void SetupCollisionComponent();
    void TransformAllBricks();

private:
    bool m_isActivated = false;

};