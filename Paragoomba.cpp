#include "pch.h"
#include "ParaGoomba.h"
#include "Debug.h"
#include "AssetIDs.h"
#include "Mario.h"
#include "MarioStateFactory.h"
#include "GameConfig.h"
#include "World.h"
#include "Goomba.h"

ParaGoomba::ParaGoomba(Vector2 position, Vector2 size, SpriteSheet* spriteSheet)
    : Entity(position, size, spriteSheet)
    , m_animTimer(0.0f)
    , m_frameTime(0.15f)  // Controls how fast the flip animation occurs
    , m_flipFrame(false)
    , m_deathTimer(0.0f)
    , m_isDying(false)
    , m_jumpTimer(0.0f)
    , m_jumpInterval(0.5f)  // Time between jumps
    , m_isJumping(false)
    , m_jumpCount(0)
    , m_jumpsBeforeBigJump(3)  // After 3 small jumps, do a big jump
    , m_hasWings(true)  // Start with wings
    , m_currentPhase(0)  // Start with closed wings
    , m_phaseTimer(0.0f)
    
    // Configurable parameters - adjust these values as needed
    , m_closedWingsDuration(0.5f)   // Walk with wings closed for 3 seconds
    , m_mediumFlapsDuration(2.0f)   // Medium flaps for 2 seconds (3 small jumps)
    , m_rapidFlapsDuration(1.0f)    // Rapid flaps for 1 second (big jump)
    , m_mediumFlapSpeed(0.17f)       // Medium flap every 0.2 seconds
    , m_rapidFlapSpeed(0.066f)        // Rapid flap every 0.1 seconds
    , m_smallJumpForce(-1.0f * 60.0f)  // Small jump force
    , m_bigJumpForce(-3.5f * 60.0f)    // Big jump force
{    // Create left wing with offset to the left side
    m_wing_left = new Wings(position, Vector2(WINGS_WIDTH, WINGS_HEIGHT), spriteSheet);
    m_wing_left->SetOffset(Vector2(-6.0f, -10.0f)); // Offset to the left
    m_wing_left->SetDirection(1); 
    
    // Create right wing with offset to the right side
    m_wing_right = new Wings(position, Vector2(WINGS_WIDTH, WINGS_HEIGHT), spriteSheet);
    m_wing_right->SetOffset(Vector2(6.0f, -10.0f)); // Offset to the right
    m_wing_right->SetDirection(-1);

    SetAnimation(ID_ANIM_PARAGOOMBA_WALK, true);
    m_visible = true; 
    
    // Set initial velocity - slow movement to the left
    SetVelocity(Vector2(-GameConfig::Enemies::Goomba::WALK_SPEED, 0.0f));
      // Setup collision component properly
    SetupCollisionComponent();
    
    m_isCollidable = true;

}

ParaGoomba::~ParaGoomba()
{
    // Clean up dynamically allocated wings
    if (m_wing_left)
    {
        delete m_wing_left;
        m_wing_left = nullptr;
    }
    
    if (m_wing_right)
    {
        delete m_wing_right;
        m_wing_right = nullptr;
    }
    
}

void ParaGoomba::Render(DirectX::SpriteBatch* spriteBatch)
{
    // Render the base entity (Goomba)
    Entity::Render(spriteBatch);
    
    // Render wings if they are active and the entity has wings
    if (!m_isDying && m_hasWings) {
        if (m_wing_left->IsActive()) {
            m_wing_left->Render(spriteBatch);
        }
        if (m_wing_right->IsActive()) {
            m_wing_right->Render(spriteBatch);
        }
    }
}

void ParaGoomba::OnCollision(const CollisionResult& event)
{
    // Handle general collision
    
    Mario* mario = dynamic_cast<Mario*>(event.collidedWith);    // Handle wall collisions - reverse direction
    Goomba* goomba = dynamic_cast<Goomba*>(event.collidedWith);
    if (event.contactNormal.x != 0 && event.collidedWith != mario) {
        float Speed = GameConfig::Enemies::Goomba::WALK_SPEED;

        // Reverse X direction when hitting a wall
        Vector2 vel = GetVelocity();
        vel.x = (vel.x > 0) ? -Speed : Speed;
        SetVelocity(vel);
    }    // Handle ground collision - prevent bouncing but allow jumping
    else if (event.contactNormal.y < 0 && event.collidedWith != mario && event.collidedWith != goomba) {
        // If hitting ground, immediately stop vertical velocity

        Vector2 vel = GetVelocity();
        vel.y = 0.0f;
        SetVelocity(vel);
    }
    
    // Check if the collision is with Mario
      if (event.contactNormal.y > 0) // Collision from above
    {

        
        if (mario) {
            if (m_hasWings) {
                // First hit just removes wings
                TransformToGoomba();
                // Make Mario bounce a bit
                Vector2 vel = mario->GetVelocity();
                vel.y = GameConfig::Mario::BOUNCE_FORCE;
                mario->SetVelocity(vel);
            } else {
                // Second hit kills the Goomba
                Die();
                
                // Make Mario bounce more
                Vector2 vel = mario->GetVelocity();
                vel.y = GameConfig::Mario::BOUNCE_FORCE;
                mario->SetVelocity(vel);
            }
        }
    }
    else if (mario && !m_isDying) {
        // If Mario hits from the side or below, hurt Mario
      //  mario->TakeDamage();
    }
}

void ParaGoomba::Die()
{
    if (!m_isDying) {
        m_isDying = true;
        m_deathTimer = 0.0f;

        SetAnimation(ID_ANIM_PARAGOOMBA_DIE, false);

        m_isCollidable = false;
        
        // Stop movement
        SetVelocity(Vector2::Zero);
        

    }
}

void ParaGoomba::TransformToGoomba()
{
    // Remove both wings but keep the Goomba alive
    m_wing_left->Deactivate();
    m_wing_right->Deactivate();
    
    // Update the wings status flag
    m_hasWings = false;
    
    // Reset jump mechanics
    m_isJumping = false;
    

}

bool ParaGoomba::HasWings() const
{
    return m_hasWings;
}

void ParaGoomba::Update(float dt)
{
    Entity::Update(dt);
    // Log("ParaGoomba", "grounded: " + std::to_string(m_isGrounded));
    if (m_isDying) {
        m_deathTimer += dt;
        if (m_deathTimer >= 0.5f) { // 0.5 seconds before disappearing
            SetIsVisible(false);
            m_isActive = false;
        }
        return;
    }     

    Mario* mario = dynamic_cast<Mario*>(World::GetInstance()->GetPlayer());
    
    static float directionUpdateTimer = 0;
    static int attempt = 0;
    const float UPDATE_INTERVAL = 2.0f; // Update direction every 2 seconds
    const int MAX_ATTEMPTS = 8; // Give up after 8 failed attempts
    if(abs(mario->GetPosition().x - GetPosition().x) < 80.0f){
    // Timer for direction updates and a counter to give up after certain attempts
        
            directionUpdateTimer += dt;
            if (directionUpdateTimer >= UPDATE_INTERVAL) {
                directionUpdateTimer = 0;
                
                if (mario && attempt < MAX_ATTEMPTS) {
                    WalkInMarioDirection(mario);
                    attempt++;
                }
            }
    }



    float targetSpeed = GameConfig::Enemies::Goomba::WALK_SPEED;
    Vector2 vel = GetVelocity();
    // If horizontal speed has deviated from target, restore it
    if (std::abs(vel.x) != targetSpeed) {
        vel.x = (vel.x >= 0) ? targetSpeed : -targetSpeed;
        SetVelocity(vel);
    }

     // Check if entity is on ground - important for physics
    m_isGrounded = Collision::GetInstance()->GroundCheck(this, dt);
    // Apply gravity only if in the air
    if (!m_isGrounded) {
        Vector2 vel = GetVelocity();
        vel.y += GameConfig::Physics::GRAVITY * dt;
        SetVelocity(vel);
    }
    // Update position based on velocity
    SetPosition(GetPosition() + GetVelocity() * dt);
      // Different movement behavior based on whether it has wings
    if (m_hasWings) {
        // Update phase timer
        m_phaseTimer += dt;
        
        // Three-phase wing animation system
        switch (m_currentPhase) {            
            case 0: // CLOSED WINGS - walking on ground
                // Only log occasionally to avoid spamming
                if (int(m_phaseTimer * 10) % 10 == 0) 

                
                // In phase 0, wings stay closed (no flapping)
                m_wing_left->SetAnimation(ID_ANIM_WINGS_FLAP_DOWN, false);
                m_wing_right->SetAnimation(ID_ANIM_WINGS_FLAP_DOWN, false);

                // If on ground and phase timer exceeded, transition to medium flaps
                if (m_isGrounded && m_phaseTimer >= m_closedWingsDuration) {
                    m_currentPhase = 1; // Switch to medium flaps
                    m_phaseTimer = 0.0f;
                    m_jumpCount = 0;
                    m_jumpTimer = 0.0f;

                }
                break;
                
            case 1: // MEDIUM FLAPS - three small jumps                // In this phase, wings flap at medium speed and we do small jumps
                // Only log occasionally to avoid spamming
                if (int(m_phaseTimer * 10) % 10 == 0)

                // Handle jumps
                m_jumpTimer += dt;

                if (m_isGrounded && m_jumpCount <= 3 && m_jumpTimer >= 0.35f) {
                    m_jumpTimer = 0.0f;
                    m_isJumping = true;

                    // Apply small jump force
                    Vector2 vel = GetVelocity();
                    vel.y = m_smallJumpForce;
                    SetVelocity(vel);
                    m_wing_left->HandleFlapping(dt, m_mediumFlapSpeed);
                    m_wing_right->HandleFlapping(dt, m_mediumFlapSpeed);
                    
                    m_jumpCount++;

                }

                HandleLanding();


                // Transition to rapid flaps after 3 jumps
                if ((m_jumpCount == 4 && m_isGrounded)) {
                    m_currentPhase = 2; // Transition to rapid flaps
                    m_phaseTimer = 0.0f;
                    m_jumpTimer = 0.0f;
                    m_jumpCount = 0;

                }
                break;                  

                case 2: // RAPID FLAPS - one big jump

                if (!m_isJumping && m_isGrounded)
                 {
                    m_jumpCount++;
                    if(m_jumpCount == 1)
                    {
                    m_isJumping = true;
                    // Apply big jump force
                    Vector2 vel = GetVelocity();
                    vel.y = m_bigJumpForce;
                    SetVelocity(vel);
                    

                }
            }
            
            HandleLanding();
            
            // Check if we need to transition to phase 0 after landing from the big jump
            if (!m_isJumping && m_isGrounded && m_jumpCount >= 2) {
                m_phaseTimer += dt;  // Use phase timer for the delay

                if(m_phaseTimer >= m_rapidFlapsDuration * 3) {
                    m_wing_left->SetAnimation(ID_ANIM_WINGS_FLAP_DOWN, false);
                    m_wing_right->SetAnimation(ID_ANIM_WINGS_FLAP_DOWN, false);
                }

                if (m_phaseTimer >= 0.5f) {
                    // Reset to closed wings phase
                    m_currentPhase = 0;
                    m_phaseTimer = 0.0f;
                    m_jumpCount = 0;

                }
            }

            break;
        }
    } 
    
    // If not flying, just walk 
    else {
        Vector2 vel = GetVelocity();
        if (std::abs(vel.x) != GameConfig::Enemies::Goomba::WALK_SPEED) {
            vel.x = vel.x > 0 ? 
            GameConfig::Enemies::Goomba::WALK_SPEED : 
            -GameConfig::Enemies::Goomba::WALK_SPEED;
            SetVelocity(vel);
        }
    }
    // Handle flip animation for the body
    m_animTimer += dt;
    if (m_animTimer >= m_frameTime)
    {
        m_animTimer = 0.0f;
        m_flipFrame = !m_flipFrame;
        
        // Apply horizontal flip to animator
        if (GetAnimator())
        {
            GetAnimator()->SetFlipHorizontal(m_flipFrame);
        }
    }    // Update both wings animation and position only if still has wings
    if (m_hasWings) {
        Vector2 currentPosition = GetPosition();
        
        // Update position for both wings
        m_wing_left->UpdatePosition(currentPosition);
        m_wing_right->UpdatePosition(currentPosition);
        
        // Handle wing flapping based on the current phase
        float flapSpeed = m_frameTime; // Default speed
        
        switch (m_currentPhase) {
            case 0: // No flapping in closed wings phase
            // Wings stay in closed position
            break;
            
            case 1: // Medium speed flapping
            // Use the medium flap speed
            m_wing_left->HandleFlapping(dt, m_mediumFlapSpeed);
            m_wing_right->HandleFlapping(dt, m_mediumFlapSpeed);
            break;
            
            case 2: // Rapid flapping
            // Use the rapid flap speed
            m_wing_left->HandleFlapping(dt, m_rapidFlapSpeed);
            m_wing_right->HandleFlapping(dt, m_rapidFlapSpeed);

            
            break;
        }
    }
    // Call base class update
}

void ParaGoomba::SetupCollisionComponent()
{
    Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x, curSize.y);
	m_collisionComponent->SetSize(newSize);

	m_collisionComponent->SetPosition(GetPosition() + Vector2(newSize.x / 2 , newSize.y / 2));
}

void ParaGoomba::HandleLanding()
{
    if (m_isJumping && m_isGrounded)
    {
        m_isJumping = false;
        
        // Reset vertical velocity to prevent bouncing but only if we're falling
        Vector2 vel = GetVelocity();
        
        // Only reset if velocity is downward (positive Y)
        if (vel.y > 0.1f) {
            vel.y = 0.0f;
            SetVelocity(vel);
        }
    }
}


void ParaGoomba::WalkInMarioDirection(Mario* mario)
{
    if (mario) {
        Vector2 marioPos = mario->GetPosition();
        Vector2 goombaPos = GetPosition();
        
        Vector2 vel = GetVelocity();

        SetVelocity(vel);
        if (marioPos.x > goombaPos.x) {
            vel.x = GameConfig::Enemies::Goomba::WALK_SPEED;
            SetVelocity(Vector2(vel));
        } else {
            vel.x = -GameConfig::Enemies::Goomba::WALK_SPEED;
            SetVelocity(Vector2(vel));
        }
    }
}