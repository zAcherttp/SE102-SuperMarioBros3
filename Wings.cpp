#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "Enemy.h"
#include "Entity.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteSheet.h"
#include "Wings.h"
#include <vector>

Wings::Wings(Vector2 position, Vector2 size, SpriteSheet* spriteSheet)
	: Enemy(position, size, spriteSheet)
	, m_animTimer(0.0f)
	, m_frameTime(0.15f)  // Controls how fast the wing flap animation occurs
	, m_flipFrame(false)
	, m_isActive(true)
	, m_offset(Vector2(-2.0f, 0.0f))  // Default offset, adjust as needed
{    // Define wing flap animations using the actual sprite frame names from sprites.json
	std::vector<const wchar_t*> flapUpFrames = { L"wing-open" };   // Note: There's a typo in sprites.json ("opem" instead of "open")
	std::vector<const wchar_t*> flapDownFrames = { L"wing-folded" };

	// // Define the animations for both states
	DefineAnimation(ID_ANIM_WINGS_FLAP_UP, flapUpFrames, false, m_frameTime);
	DefineAnimation(ID_ANIM_WINGS_FLAP_DOWN, flapDownFrames, false, m_frameTime);

	// Start with the up position
	SetAnimation(ID_ANIM_WINGS_FLAP_DOWN, false);
	m_visible = true;
	m_isCollidable = false; // Wings don't need collision

	Log(__FUNCTION__, "Wings initialized");
}

// Base Update method implementation required by Entity abstract class
void Wings::Update(float dt)
{
	if (m_isActive) {
		// Use current position and zero velocity as defaults
		Update(dt, GetPosition(), Vector2(0, 0));
	}

	// Call base update to handle animation
	Entity::Update(dt);
}

void Wings::Update(float dt, Vector2 ownerPosition, Vector2 ownerVelocity)
{
	if (!m_isActive) return;
	ownerPosition;
	ownerVelocity;
	Entity::Update(dt);
}

void Wings::Render(DirectX::SpriteBatch* spriteBatch)
{
	if (!m_isActive) return;
	// Use the standard Entity rendering which will use our animator
	Entity::Render(spriteBatch);
}

void Wings::UpdatePosition(const Vector2& ownerPosition)
{
	// Update wing position based on owner position and offset
	SetPosition(ownerPosition + m_offset);
}

void Wings::Deactivate()
{
	m_isActive = false;
	m_visible = false;
	delete this;
	Log(__FUNCTION__, "Wings deactivated");
}

void Wings::HandleFlapping(float dt, float flapSpeed)
{
	if (!m_isActive) return;

	// Override the default frame time with the specified speed
	m_frameTime = flapSpeed;

	// Directly force a flap animation update based on the provided speed
	m_animTimer += dt;
	if (m_animTimer >= m_frameTime)
	{
		m_animTimer = 0.0f;
		m_flipFrame = !m_flipFrame;

		// Switch between up and down animation states
		if (m_flipFrame)
		{
			SetAnimation(ID_ANIM_WINGS_FLAP_DOWN, false);
		}
		else
		{
			SetAnimation(ID_ANIM_WINGS_FLAP_UP, false);
		}
	}
}

void Wings::SetDirection(int direction)
{
	// Set the internal flipped state
	m_flipped = (direction == 1 ? true : false);

	// Apply the horizontal flip to the animator
	if (GetAnimator() != nullptr)
	{
		GetAnimator()->SetFlipHorizontal(m_flipped);
	}
}
