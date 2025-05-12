#include "pch.h"
#include "Bullet.h"
#include "AssetIDs.h"
#include "Debug.h"

Bullet::Bullet(Vector2 position, Vector2 size, SpriteSheet* spriteSheet, BulletDirection direction)
    : Entity(position, size, spriteSheet)
    , m_animTimer(0.0f)
    , m_rotationTimer(0.0f)
    , m_frameTime(0.15f)
    , m_rotationFrameTime(0.1f)  // Time between rotation frames
    , m_currentRotationFrame(0)
    , m_isActive(true)
    , m_direction(direction)
    , m_speed(30.0f)  // Pixels per second
{
    m_doesNotContactWithSolidBlocks = true; // Ignore solid blocks

    std::vector<const wchar_t*> bulletFrames = { L"fire-projectile" };
    DefineAnimation(ID_ANIM_BULLET, bulletFrames, true, m_frameTime);

    // Start with the bullet animation
    SetAnimation(ID_ANIM_BULLET);
    m_visible = true;
    m_isCollidable = true; // Bullets need collision for hitting Mario
    
    // Apply initial direction
    if (GetAnimator() ) {
        GetAnimator()->SetFlipHorizontal((m_direction == TOP_RIGHT || m_direction == BOTTOM_RIGHT) ? true : false);
    }

    Log(__FUNCTION__, "Bullet initialized with direction: " + std::to_string(static_cast<int>(m_direction)));
}

void Bullet::Update(float dt) 
{
    if (!m_isActive) return;
    
    // Handle rotation animation
    UpdateRotation(dt);
    
    // Update position based on direction and speed
    Vector2 position = GetPosition();
    float velocityX = 0.0f;
    float velocityY = 0.0f;
    
    // Set velocity components based on direction
    switch (m_direction)
    {
    case TOP_RIGHT:
        velocityX = m_speed;
        velocityY = -m_speed;
        break;
    case TOP_LEFT:
        velocityX = -m_speed;
        velocityY = -m_speed;
        break;
    case BOTTOM_RIGHT:
        velocityX = m_speed;
        velocityY = m_speed;
        break;
    case BOTTOM_LEFT:
        velocityX = -m_speed;
        velocityY = m_speed;
        break;
    }
    
    position.x += velocityX * dt;
    position.y += velocityY * dt;
    SetPosition(position);
    
    // Call base update to handle animation
    Entity::Update(dt);
}

void Bullet::UpdateRotation(float dt)
{
    // Update rotation timer
    m_rotationTimer += dt;
    
    // If it's time to rotate to the next frame
    if (m_rotationTimer >= m_rotationFrameTime)
    {
        m_rotationTimer = 0.0f;
        
        // Move to next rotation frame (0, 90, 180, 270 degrees)
        m_currentRotationFrame = (m_currentRotationFrame + 1) % ROTATION_FRAMES;
        
        // Apply rotation to the animator
        if (GetAnimator())
        {
            // Set rotation angle based on current frame (in degrees)
            float rotationAngle = m_currentRotationFrame * 90.0f;
            GetAnimator()->SetRotation(rotationAngle);
        }
    }
}

void Bullet::Render(DirectX::SpriteBatch* spriteBatch)
{    
    // Use the standard Entity rendering which will use our animator
    Entity::Render(spriteBatch);
}

void Bullet::Deactivate()
{
    m_isActive = false;
    m_visible = false;
    m_isCollidable = false;
    Log(__FUNCTION__, "Bullet deactivated");
}

void Bullet::SetDirection(BulletDirection direction)
{
    m_direction = direction;
    
    // Apply the horizontal flip to the animator based on direction
    if (GetAnimator() != nullptr)
    {
        // Flip horizontally for RIGHT directions
        GetAnimator()->SetFlipHorizontal(m_direction == TOP_RIGHT || m_direction == BOTTOM_RIGHT);
    }
}


