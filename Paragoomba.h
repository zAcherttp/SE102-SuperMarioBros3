#pragma once
#include "Entity.h"
#include "Wings.h"
#include "Mario.h"

#define PARAGOOMBA_WIDTH 16
#define PARAGOOMBA_HEIGHT 16

class ParaGoomba : public Entity
{
public:    ParaGoomba(Vector2 position, Vector2 size, SpriteSheet* spriteSheet);
    ~ParaGoomba();

    void Update(float dt) override;
    void Render(DirectX::SpriteBatch* spriteBatch) override;

    void OnCollision(const CollisionResult& event) override;
    void OnNoCollision(float dt, Axis axis) override;
    void SetupCollisionComponent();
    void Die() override;
      // Method to transform into regular Goomba (remove wings)
    void TransformToGoomba();
    
    // Check if ParaGoomba still has wings
    bool HasWings() const;

    void HandleLanding();

    void WalkInMarioDirection(Mario* mario);
    
    void Jump(float dt);
private:
    float m_animTimer;
    float m_frameTime;
    bool m_flipFrame;
    float m_deathTimer;
    bool m_isDying;
    bool m_hasWings;  // Flag to track if it still has wings
    
    // Composition: ParaGoomba has wings
    Wings* m_wing_left;
    Wings* m_wing_right;
    
    // Flight behavior variables
    float m_jumpTimer;
    float m_jumpInterval;
    bool m_isJumping;
    int m_jumpCount;
    int m_jumpsBeforeBigJump;

    int m_currentPhase; // 0: closed wings, 1: medium speed flapping, 2: rapid flapping
    float m_phaseTimer;
    float m_closedWingsDuration;   // Duration for closed wings phase
    float m_mediumFlapsDuration;   // Duration for medium flaps phase
    float m_rapidFlapsDuration;    // Duration for rapid flaps phase
    float m_mediumFlapSpeed;       // Speed for medium flaps
    float m_rapidFlapSpeed;        // Speed for rapid flaps
    float m_smallJumpForce;        // Force for small jump
    float m_bigJumpForce;          // Force for big jump
    float m_jumpForce;             // Current jump force

};

