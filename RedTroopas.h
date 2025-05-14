#pragma once
#include "Entity.h"

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
    void Die() override;
    
    // New function to update sprite direction based on velocity
    void UpdateSpriteDirection();

    void TransformToShell();
    // void TransformToTroopa();
    
    // Check for platform edges using raycasts
    bool CheckEdge();

    bool UsesInteractionPoints() const override;

    std::vector<std::pair<InteractionPointType, DirectX::SimpleMath::Vector2>> GetInteractionPoints() const override;
    
    // void OnFootCollision(const CollisionResult& event) override;
    // void OnTopHeadCollision(const CollisionResult& event) override;
    // void OnLeftSideCollision(const CollisionResult& event) override;
    // void OnRightSideCollision(const CollisionResult& event) override;
    // void OnNoCollision(float dt, Axis axis) override;
    void OnCollision(const CollisionResult& event) override;
    void UpdateInteractionPointState(InteractionPointType type, bool isColliding);
    void ResetInteractionPoints();
    
    private:
    float m_animTimer;
    float m_frameTime;
    bool m_flipFrame;
    float m_deathTimer;
    bool m_isDying;
    
    bool m_isColliding;
    
    // Store the last direction for comparison
    float m_lastDirectionX;
    
    // Direction change cooldown to prevent rapid reversals
    
    std::map<InteractionPointType, bool> m_pointCollisionState;
    
    TroopaState m_state;
};

