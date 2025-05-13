#include "pch.h"
#include "Goomba.h"
#include "Debug.h"
#include "AssetIDs.h"
#include "Mario.h"
#include "MarioStateFactory.h"
#include "GameConfig.h"
#include "ParaGoomba.h"
#include "Block.h"

Goomba::Goomba(Vector2 position, Vector2 size, SpriteSheet* spriteSheet)
    : Entity(position,size, spriteSheet)
    , m_animTimer(0.0f)
    , m_frameTime(0.15f)  // Controls how fast the flip animation occurs
    , m_flipFrame(false)
{
    // Define the animation using the single frame we have

    SetAnimation(ID_ANIM_GOOMBA_WALK, true);
    m_visible = true; // Set visibility to true
    m_isDying = false; // Initialize dying state
    
    // Set initial velocity - slow movement to the left
    SetVelocity(Vector2(-GameConfig::Enemies::Goomba::WALK_SPEED, 0.0f));
    
    // Setup collision component
    SetupCollisionComponent();
    Log(__FUNCTION__, "Collision component initialized");
}

void Goomba::Render(DirectX::SpriteBatch* spriteBatch)
{
    Entity::Render(spriteBatch);
}

void Goomba::OnCollision(const CollisionResult& event)
{
    // Handle general collision
    // Log("GoombaCollision", "Collision detected");
    // Log("GoombaCollision", "Collision normal: " + std::to_string(event.contactNormal.x) + ", " + std::to_string(event.contactNormal.y));
    
    // Check if the collision is with Mario
    Mario* mario = dynamic_cast<Mario*>(event.collidedWith);
    ParaGoomba* paraGoomba = dynamic_cast<ParaGoomba*>(event.collidedWith);
    Block* block = dynamic_cast<Block*>(event.collidedWith);

    if(event.collidedWith->GetCollisionGroup() == CollisionGroup::NONSOLID) {
        return;
    }

    if (event.contactNormal.x != 0 && block  ) // Collision from the sides
    {
        if(block->IsSolid()) 
        {
            // If hitting a wall (not Mario), reverse direction and maintain standard speed
            float targetSpeed = GameConfig::Enemies::Goomba::WALK_SPEED;
            Vector2 vel = GetVelocity();
            Log("Goomba", "Reversing direction");
            vel.x = -vel.x;
            SetVelocity(vel);
            Log("Goomba", "Velocity: " + std::to_string(vel.x) + ", " + std::to_string(vel.y));
        }
        return;
    }
    if (event.contactNormal.y > 0) // Collision from above
    {
        if (mario) {
            Die(); // Goomba dies when Mario jumps on it
            
            // Make Mario bounce a bit
            Vector2 vel = mario->GetVelocity();
            vel.y = GameConfig::Mario::BOUNCE_FORCE; // Use Mario's bounce force
            mario->SetVelocity(vel);
        }
        return;
    }
    if (event.contactNormal.y < 0 && event.collidedWith != mario) {
        // If hitting ground, immediately stop vertical velocity
        Vector2 vel = GetVelocity();
        vel.y = 0.0f;
        SetVelocity(vel);
        m_isGrounded = true;
        return;
    }
}

void Goomba::Die()
{
    if (!m_isDying) {
        m_isDying = true; // Set dying state to true
        m_isCollidable = false; // Disable collision during death
        SetAnimation(ID_ANIM_GOOMBA_DIE, false); // Set death animation
        m_deathTimer = 0.0f; // Initialize the death timer
    }
}

void Goomba::Update(float dt)
{
    if (m_isDying) {
        m_deathTimer += dt;
        if (m_deathTimer >=  GameConfig::Enemies::Goomba::DEATH_ANIMATION_TIME) {
            // After 0.5 seconds, deactivate the Goomba 
            m_isActive = false;
            m_visible = false;
        }
        return; // Skip normal updates when dying
    }
    
    // Check if entity is on ground - important for physics
    m_isGrounded = Collision::GetInstance()->GroundCheck(this, dt);
    
    // Apply gravity only if in the air
    if (!m_isGrounded) {
        Vector2 vel = GetVelocity();
        vel.y += GameConfig::Physics::GRAVITY * dt;
        SetVelocity(vel);
    }
    
    // float targetSpeed = GameConfig::Enemies::Goomba::WALK_SPEED;
    // Vector2 vel = GetVelocity();
    // // If horizontal speed has deviated from target, restore it
    // if (std::abs(vel.x) != targetSpeed) {
    //     vel.x = (vel.x >= 0) ? targetSpeed : -targetSpeed;
    //     SetVelocity(vel);
    // }

    // Handle flip animation
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
    }

    SetPosition(GetPosition() + GetVelocity() * dt);
 
    // Update other properties
    Entity::Update(dt);
}

void Goomba::SetupCollisionComponent()
{
    Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x, curSize.y);
	m_collisionComponent->SetSize(newSize);

	m_collisionComponent->SetPosition(GetPosition() + Vector2(newSize.x / 2 , newSize.y / 2));
}
