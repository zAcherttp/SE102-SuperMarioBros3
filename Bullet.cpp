#include "pch.h"
#include "Bullet.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "Mario.h"


Bullet::Bullet(Vector2 position, Vector2 size, SpriteSheet* spriteSheet, BulletDirection direction)
    : Entity(position, size, spriteSheet)
    , m_animTimer(0.0f)
    , m_rotationTimer(0.0f)
    , m_frameTime(0.15f)
    , m_rotationFrameTime(0.1f)  // Time between rotation frames
    , m_currentRotationFrame(0)
    , m_isActive(true)
    , m_direction(direction)
    , m_speed(50.0f)  // Pixels per second
{


    std::vector<const wchar_t*> bulletFrames = { L"fire-projectile" };
    DefineAnimation(ID_ANIM_BULLET, bulletFrames, true, m_frameTime);

    // Start with the bullet animation
    SetAnimation(ID_ANIM_BULLET);
    m_visible = true;
    m_isCollidable = true; // Bullets need collision for hitting Mario
    
    // Apply initial direction
    if (GetAnimator() ) {
        GetAnimator()->SetFlipHorizontal((m_direction == TOP_RIGHT_45 || m_direction == TOP_RIGHT_60 || m_direction == TOP_RIGHT_120 || m_direction == TOP_RIGHT_135) ? true : false);
    }

}

void Bullet::Update(float dt) 
{
    if (!m_isActive) return;

    Mario* m_mario = dynamic_cast<Mario*>(World::GetInstance()->GetPlayer());

    // Perform sweep AABB collision detection with Mario
    Vector2 bulletPosition = GetPosition();
    Vector2 bulletSize = GetSize() * 0.9f;
    Vector2 marioPosition = m_mario->GetPosition();
    Vector2 marioSize = m_mario->GetSize();

    Rectangle bulletBox = {long(bulletPosition.x), long(bulletPosition.y), long(bulletSize.x), long(bulletSize.y)};

    Rectangle marioBox = {long(marioPosition.x), long(marioPosition.y), long(marioSize.x), long(marioSize.y)};

    if (bulletBox.Intersects(marioBox)) {
        m_mario->Damage();
        return;
    }

    
    // Handle rotation animation
    UpdateRotation(dt);
    
    // Update position based on direction and speed
    Vector2 position = GetPosition();
    float velocityX = 0.0f;
    float velocityY = 0.0f;
      // Set velocity components based on direction
    switch (m_direction)
    {
    // Right side angles (clockwise from top)
    case TOP_RIGHT_45:      // 45° angle (1, -1)
        velocityX = m_speed * 0.7071f;  // cos(45°) = 0.7071
        velocityY = m_speed * 0.7071f; // -sin(45°) = -0.7071
        break;
    case TOP_RIGHT_60:      // 60° angle (0.5, -0.866)
        velocityX = m_speed * 0.866f;     // cos(60°) = 0.866
        velocityY = m_speed * 0.5f;  // -sin(60°) = -0.5
        break;
    case TOP_RIGHT_120:     // 120° angle (0.5, 0.866)
        velocityX = m_speed * 0.866f;     // cos(120°) = -0.5, but we want right direction
        velocityY = -m_speed * 0.5f;   // sin(120°) = 0.866
        break;
    case TOP_RIGHT_135:     // 135° angle (0.7071, 0.7071)
        velocityX = m_speed * 0.7071f;  // cos(135°) = -0.7071, but we want right direction
        velocityY = -m_speed * 0.7071f;  // sin(135°) = 0.7071
        break;
    
    // Left side angles (clockwise from top)
    case TOP_LEFT_45:       // 45° angle (-0.7071, -0.7071)
        velocityX = -m_speed * 0.7071f; // -cos(45°) = -0.7071
        velocityY = m_speed * 0.7071f; // -sin(45°) = -0.7071
        break;
    case TOP_LEFT_60:       // 60° angle (-0.5, -0.866)
        velocityX = -m_speed * 0.866f;    // -cos(60°) = -0.5
        velocityY = m_speed * 0.5;  // -sin(60°) = -0.866
        break;
    case TOP_LEFT_120:      // 120° angle (-0.5, 0.866)
        velocityX = -m_speed * 0.866f;    // -cos(60°) = -0.5 (mirrored from RIGHT_120)
        velocityY = -m_speed * 0.5;   // sin(60°) = 0.866
        break;
    case TOP_LEFT_135:      // 135° angle (-0.7071, 0.7071)
        velocityX = -m_speed * 0.7071f; // -cos(45°) = -0.7071
        velocityY = -m_speed * 0.7071f;  // sin(45°) = 0.7071
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
        // Check if it's a right-side direction (which should be flipped)
        bool isRightDirection = (m_direction == TOP_RIGHT_45 || 
                               m_direction == TOP_RIGHT_60 || 
                               m_direction == TOP_RIGHT_120 || 
                               m_direction == TOP_RIGHT_135);
        
        GetAnimator()->SetFlipHorizontal(isRightDirection);
    }
}

