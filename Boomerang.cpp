#include "pch.h"
#include "Boomerang.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "Mario.h"
#include "GameConfig.h"
#include "Collision.h"
#include "World.h"
#include "BoomerangBro.h"
#include "Game.h"

Boomerang::Boomerang(Vector2 position, Vector2 size, SpriteSheet* spriteSheet) : 
    Enemy(position, size, spriteSheet)
    , m_animTimer(0.0f)
    , m_rotationTimer(0.0f)
    , m_frameTime(0.1f) // Adjust as needed
    , m_rotationFrameTime(0.1f) // Adjust as needed
    , m_currentRotationFrame(0)
    , m_lifeTimer(0.0f)
{
    std::vector<const wchar_t*> boomerangFrames = { L"boomerang" };
    m_state = BoomerangState::HOLDING; // Initial state
    m_visible = true;           // ✅ Make visible
    m_isCollidable = true; 
	DefineAnimation(ID_ANIM_BOOMERANG, boomerangFrames, true, m_frameTime);
    m_startPositionX = position.x;
    m_startPositionY = position.y;
	// Start with the bullet animation
    BOOMERANG_VERTICAL_LENGTH = 0.0f; // Initialize vertical length
    SetAnimation(ID_ANIM_BOOMERANG);
    SetupCollisionComponent();

    m_accelerationX = BOOMERANG_HORIZONTAL_ACCE; // Set horizontal acceleration
    m_accelerationY = BOOMERANG_GRAVITY; // Set vertical acceleration
    SetVelocity(Vector2(-BOOMERANG_HORIZONTAL_SPEED,-BOOMERANG_MAX_VERTICAL_SPEED)); 
}

void Boomerang::Update(float dt)
{   
    m_lifeTimer += dt;
    Vector2 vel = GetVelocity();
    Vector2 pos = GetPosition();
    vel.y += m_accelerationY * dt; // Apply vertical acceleration
    vel.x += m_accelerationX * dt; // Apply horizontal acceleration
    
    if(abs(vel.y) > BOOMERANG_MAX_VERTICAL_SPEED)
    {
        vel.y = (vel.y > 0) ? BOOMERANG_MAX_VERTICAL_SPEED : -BOOMERANG_MAX_VERTICAL_SPEED; // Clamp vertical speed
    }
    if(abs(vel.x) > BOOMERANG_HORIZONTAL_SPEED)
    {
        vel.x = (vel.x > 0) ? BOOMERANG_HORIZONTAL_SPEED : -BOOMERANG_HORIZONTAL_SPEED; // Clamp horizontal speed
    }
    if(vel.y > 0 && pos.y >= m_startPositionY)
    {
        vel.y = 0; 
    }

    // Mario* m_mario = dynamic_cast<Mario*>(Game::GetInstance()->GetCurrentWorld()->GetPlayer());

	// Vector2 boomerangSize = GetSize() * 0.9f;
	// Vector2 marioPosition = m_mario->GetPosition();
	// Vector2 marioSize = m_mario->GetSize();

	// Rectangle boomerangBox = { long(pos.x), long(pos.y), long(boomerangSize.x), long(boomerangSize.y) };

	// Rectangle marioBox = { long(marioPosition.x), long(marioPosition.y), long(marioSize.x), long(marioSize.y) };

	// if (boomerangBox.Intersects(marioBox)) {
	// 	m_mario->Damage();
	// 	return;
	// }
    SetVelocity(vel);
    SetPosition(GetPosition() + GetVelocity() * dt);
}

void Boomerang::SetState(BoomerangState state)
{
    Vector2 vel = GetVelocity();
    Vector2 pos = GetPosition();
	switch (state)
	{
	case BoomerangState::OUTWARD:
        m_state = BoomerangState::OUTWARD;
		vel.y = -BOOMERANG_INITAL_UPWARD_SPEED;
		m_accelerationY = BOOMERANG_GRAVITY * 2;
		vel.x = BOOMERANG_HORIZONTAL_SPEED* 2 * (m_direction ? -1 : 1);
        SetVelocity(vel);
		break;
	case BoomerangState::RETURN:
        m_state = BoomerangState::RETURN;
		vel.x = -vel.x;
        SetVelocity(vel);
        Log(LOG_INFO, "Boomerang returning, velocity: " + std::to_string(vel.x) + ", " + std::to_string(vel.y));
		break;
	case BoomerangState::HOLDING:
        m_state = BoomerangState::HOLDING;
		m_accelerationX = 0;
		m_accelerationY = 0;
        SetVelocity(Vector2::Zero);
		break;
	default:
		break;
	}
}

void Boomerang::Render(DirectX::SpriteBatch* spriteBatch)
{
    if (!m_isActive) return;
    Entity::Render(spriteBatch);
}

void Boomerang::UpdatePosition(const Vector2& ownerPosition)
{
    SetPosition(ownerPosition + Vector2(8.0f, 0.0f)); 
}
void Boomerang::Deactivate()
{
    m_isActive = false;
    m_visible = false; 
    m_isCollidable = false;
}

void Boomerang::SetupCollisionComponent()
{
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x, curSize.y); // Make collision box slightly smaller
	m_collisionComponent->SetSize(newSize);
	m_collisionComponent->SetPosition(GetPosition() + Vector2(newSize.x / 2, newSize.y / 2));
}

// void Boomerang::UpdateRotation(float dt)
// {
//     m_rotationTimer += dt;
//     if (m_rotationTimer >= m_rotationFrameTime) {
//         m_currentRotationFrame = (m_currentRotationFrame + 1) % ROTATION_FRAMES;
//         m_rotationTimer = 0.0f;
//     }
    
//     // Update the boomerang's rotation based on the current frame
//     float angle = (m_currentRotationFrame * 90.0f) * (DirectX::XM_PI / 180.0f); // Convert degrees to radians
//     SetRotation(angle);
// }

void Boomerang::SetDirection()
{
    m_direction = !m_direction; // Toggle direction
    if (GetAnimator())
    {
        GetAnimator()->SetFlipHorizontal(m_direction);
    }
}

void Boomerang::OnCollision(const CollisionResult& event)
{
    Mario* mario = dynamic_cast<Mario*>(event.collidedWith);
    BoomerangBro* boomerangBro = dynamic_cast<BoomerangBro*>(event.collidedWith);

    if (mario)
    {
        // mario->Damage(); // Damage Mario on collision
        // this->Deactivate(); // Deactivate the boomerang after hitting Mario
    }

    if (boomerangBro)
    {
        boomerangBro->OnBoomerangCaught(this);
        this->Deactivate();
    }
}

