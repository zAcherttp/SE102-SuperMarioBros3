#pragma once
#include "Entity.h"

enum class MushroomPhase
{
    GROWING,
    WALKING
};

class Mushroom : public Entity
{
public:
    Mushroom(Vector2 position, Vector2 size, SpriteSheet* spriteSheet);

    void Update(float dt) override;
    void Render(DirectX::SpriteBatch* spriteBatch) override;

    void OnCollision(const CollisionResult& event) override;\
    void SetupCollisionComponent();

private:

    MushroomPhase m_phase;
    Vector2 m_initialPosition;
    bool m_isVelocitySet = false;
    bool m_hasInitializedCollisionComponent = false;
};