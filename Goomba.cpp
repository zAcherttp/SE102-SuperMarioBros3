#include "pch.h"
#include "Goomba.h"
#include "Debug.h"
#include "AssetIDs.h"
#include "Mario.h"
#include "GameConfig.h"

Goomba::Goomba(Vector2 position, Vector2 size, SpriteSheet* spriteSheet)
	: Entity(position, size, spriteSheet)
	, m_animTimer(0.0f)
	, m_frameTime(0.15f)  // Controls how fast the flip animation occurs
	, m_flipFrame(false)
{
	// Define the animation using the single frame we have
	std::vector<const wchar_t*> walkFrames = { L"red-goomba-walk" };
	DefineAnimation(ID_ANIM_GOOMBA_WALK, walkFrames, true, m_frameTime);
	SetAnimation(ID_ANIM_GOOMBA_WALK, true);
	m_visible = true; // Set visibility to true
	m_isDying = false; // Initialize dying state

	// Set initial velocity - slow movement to the left
	SetVelocity(Vector2(-GameConfig::Enemies::Goomba::WALK_SPEED, 0.0f));

	// Setup collision component
	SetupCollisionComponent();
	//Log(__FUNCTION__, "Collision component initialized");
}

void Goomba::Render(DirectX::SpriteBatch* spriteBatch)
{
	// Debugging render information
	// Log("GoombaRender", "Rendering at position (" + 
	//     std::to_string(GetPosition().x) + ", " + 
	//     std::to_string(GetPosition().y) + ")");

	Entity::Render(spriteBatch);
}

void Goomba::OnCollision(const CollisionResult& event)
{
	// Handle general collision
	//Log("GoombaCollision", "Collision detected");
	//Log("GoombaCollision", "Collision normal: " + std::to_string(event.contactNormal.x) + ", " + std::to_string(event.contactNormal.y));

	// Check if the collision is with Mario
	Mario* mario = dynamic_cast<Mario*>(event.collidedWith);

	if (event.contactNormal.y > 0) // Collision from above
	{
		//Log("GoombaCollision", "Collision from above detected");

		if (mario) {
			//Log("GoombaCollision", "Mario jumped on Goomba's head!");
			Die(); // Goomba dies when Mario jumps on it

			// Make Mario bounce a bit
			Vector2 vel = mario->GetVelocity();
			vel.y = GameConfig::Mario::STANDARD_JUMP_FORCE; // Use Mario's standard jump force
			mario->SetVelocity(vel);
		}
	}
	else if (event.contactNormal.x != 0) // Collision from the sides
	{
		//Log("GoombaCollision", "Collision from the side detected");

		// If Mario hit the Goomba from the side
		if (mario) {
			//Log("GoombaCollision", "Mario collided with Goomba from the side");
			mario->Damage();
		}
	}
}

void Goomba::Die()
{
	if (!m_isDying) {
		m_isDying = true; // Set dying state to true
		m_isCollidable = false; // Disable collision during death
		//Log("GoombaDie", "Goomba is dying at position (" + std::to_string(GetPosition().x) + ", " + std::to_string(GetPosition().y) + ")");
		SetAnimation(ID_ANIM_GOOMBA_DIE, false); // Set death animation
		m_deathTimer = 0.0f; // Initialize the death timer
	}
}

void Goomba::Update(float dt)
{
	if (m_isDying) {
		m_deathTimer += dt;
		if (m_deathTimer >= GameConfig::Enemies::Goomba::DEATH_ANIMATION_TIME) {
			// After 0.5 seconds, deactivate the Goomba 
			m_isActive = false;
			m_visible = false;
			//Log("GoombaDie", "Goomba deactivated after death animation");
		}
		return; // Skip normal updates when dying
	}

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

	m_collisionComponent->SetPosition(GetPosition() + Vector2(newSize.x / 2, newSize.y / 2));
}
