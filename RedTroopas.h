#pragma once
#include "Entity.h"
#include "Effect.h"
#include <vector>

enum TroopaState
{
    WALKING,
    SHELL_IDLE,
    SHELL_SLIDE,
    DEAD
};

class RedTroopas : public Entity
{
public:
    RedTroopas(Vector2 position, Vector2 size, SpriteSheet* spriteSheet);
    ~RedTroopas() = default;

    void Update(float dt) override;
    void Render(DirectX::SpriteBatch* spriteBatch) override;

    // void OnCollision(const CollisionResult& event) override;
    void SetupCollisionComponent();
    void Die(DyingType type) override;

    // New function to update sprite direction based on velocity
    void UpdateSpriteDirection();

    void TransformToShell();
    void TransformToTroopa();
    
    // Check for platform edges using raycasts
    bool CheckEdge();

    void OnCollision(const CollisionResult& event) override;

    void StartVibration();
    void UpdateVibration(float dt);
    
private:
    float m_animTimer;
    float m_frameTime;
    bool m_flipFrame;
    float m_deathTimer;
    bool m_isDying;

    bool m_isVibrating;
    float m_vibrateTimer;
    float m_vibrateInterval;
    float m_vibrateAmplitude;
    int m_vibrateCount;
    int m_maxVibrateCount;
    Vector2 m_originalPosition;
    bool m_vibrateDirection; 
    bool m_firstRevivePhase;
    bool m_secondRevivePhase;

    float m_reviveTimer;
    float m_reviveTime;
    
    bool m_isColliding;
    
    float m_lastDirectionX;

    std::vector<Effect*> m_bonkEffects;
    
    std::map<InteractionPointType, bool> m_pointCollisionState;
    
    TroopaState m_state;
};

