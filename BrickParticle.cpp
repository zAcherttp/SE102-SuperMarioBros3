#include "pch.h"
#include "BrickParticle.h"
#include "AssetIDs.h"

BrickParticle::BrickParticle(Vector2 position, Vector2 size, SpriteSheet* spriteSheet, Vector2 velocity)
    : Entity(position, size, spriteSheet)
    , m_velocity(velocity)
    , m_gravity(980.0f)
    , m_lifetime(0.0f)
    , m_maxLifetime(2.0f)
{
    // Set up the brick particle sprite
    DefineAnimation(ID_ANIM_BRICK_PARTICLE, { L"shattered-brick" }, false, 0.0f);
    SetAnimation(ID_ANIM_BRICK_PARTICLE, false);
}

void BrickParticle::Update(float dt)
{
    m_lifetime += dt;
    
    // Apply gravity
    m_velocity.y += m_gravity * dt;
    
    // Update position
    Vector2 position = GetPosition();
    position += m_velocity * dt;
    SetPosition(position);
    

    if (m_lifetime > m_maxLifetime || position.y > 1000) {
        m_isActive = false;
        m_visible = false;
    }

    UpdateRotation(dt);
    
    Entity::Update(dt);
}

void BrickParticle::Render(DirectX::SpriteBatch* spriteBatch)
{
    Entity::Render(spriteBatch);
}

void BrickParticle::UpdateRotation(float dt)
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
