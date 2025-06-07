#include "pch.h"
#include "FirePiranha.h"
#include "Debug.h"
#include "AssetIDs.h"
#include "Mario.h"
#include "Block.h"
#include "Game.h"
#include "World.h"
#include "RedTroopas.h"


FirePiranha::FirePiranha(Vector2 position, Vector2 size, SpriteSheet* spriteSheet, int color)
	: Enemy(position, size, spriteSheet)
	, m_state(HIDDEN)
	, m_timer(0.0f)
	, m_emergeTime(1.0f) // 1 second to emerge
	, m_fireDelay(1.5f)  // 0.5 second delay before firing
	, m_fireTime(2.0f)   // 2 seconds in firing state
	, m_activeBullet(nullptr)
	, m_world(nullptr)
	, m_isFlipped(false) // Default: looking left (original sprite direction)
	, m_isLookingUp(true) // Default: looking up
	, m_yThreshold(352.0f) // Y threshold for up/down orientation
	, m_initialPosition(position) // Store the initial position
	, BULLET_OFFSET(0.0f, -8.0f)
	, m_flipTime(0.025f)
	, m_flipTimer(0.0f)
	, m_color(color)
	, m_offset(0.0f)
{
	SetupCollisionComponent();
	Log(__FUNCTION__, "Collision component initialized");
	m_mario = dynamic_cast<Mario*>(World::GetInstance()->GetPlayer());
	m_isCollidable = true;

	// Offset for bullet spawn position
}

FirePiranha::~FirePiranha()
{
	CleanupBullets();
}

void FirePiranha::Update(float dt)
{

	if (m_isDying && m_isActive)
	{
		if (m_deathTimer > m_flipTime * 15)
		{
			m_isActive = false;
			return;
		}
		m_deathTimer += dt;

		m_flipTimer += dt;
		if (m_flipTimer > m_flipTime) {
			m_isFlipped = !m_isFlipped;
			m_animator->SetFlipVertical(m_isFlipped);
			m_flipTimer = 0.0f;
		}

		Entity::Update(dt);
		if (m_activeBullet != nullptr) {
			m_activeBullet->Update(dt);
		}
		return;
	}

	Vector2 m_pos = GetPosition();
	Vector2 mario_pos = m_mario ? m_mario->GetPosition() : Vector2(0, 0);


	// Debug position periodically
	static float debugTimer = 0;
	debugTimer += dt;


	Vector2 vel = GetVelocity();

	if (m_state != HIDDEN && m_mario != nullptr)
	{
		UpdatePlantOrientation(m_mario);
	}

	m_timer += dt;

	switch (m_state)
	{
	case HIDDEN:
		if (abs(m_pos.x - mario_pos.x) < 120.0f && abs(m_pos.x - mario_pos.x) >= 24.0f)
		{
			m_state = EMERGING;
			m_timer = 0.0f; // Reset timer for emerging state
		}
		break;

	case EMERGING:
		// Set the appropriate animation based on vertical orientation (up/down)
		if (m_isLookingUp) {
			if(m_color == 0)
			{
				SetAnimation(ID_ANIM_GREEN_PIRANHA_LOOK_UP);
			}
			else
			{
				SetAnimation(ID_ANIM_RED_PIRANHA_LOOK_UP);
			}

		}
		else {
			if(m_color == 0)
			{
				SetAnimation(ID_ANIM_GREEN_PIRANHA_LOOK_DOWN);
			}
			else
			{
				SetAnimation(ID_ANIM_RED_PIRANHA_LOOK_DOWN);
			}
		}

		// Apply the correct horizontal orientation based on Mario's position
		if (GetAnimator()) {
			GetAnimator()->SetFlipHorizontal(m_isFlipped);
		}

		m_offset = m_color == 0 ? 24.0f : 32.0f;

		if (m_pos.y > m_initialPosition.y - m_offset) // If above the initial position
		{
			vel.x = 0.0f;
			vel.y = -50.0f;
			SetVelocity(vel);
			SetPosition(GetPosition() + GetVelocity() * dt);
		}
		if (m_pos.y <= m_initialPosition.y - m_offset) // If at or below the initial position
		{
			vel.x = 0.0f;   // No horizontal movement
			vel.y = 0.0f;   // Stop vertical movement
			SetVelocity(vel);
			m_timer = 0.0f; // Reset timer for firing state
			m_state = FIRING; // Transition to firing state
			CleanupBullets();
		}

		break;

	case FIRING:
		// Set the appropriate "mouth open" animation based on vertical orientation

		if (m_isLookingUp) {
			if(m_color == 0)
			{
				SetAnimation(ID_ANIM_GREEN_PIRANHA_LOOK_UP_OPEN, true);
			}
			else
			{
				SetAnimation(ID_ANIM_RED_PIRANHA_LOOK_UP_OPEN, true);
			}
		}
		else {
			if(m_color == 0)
			{
				SetAnimation(ID_ANIM_GREEN_PIRANHA_LOOK_DOWN_OPEN, true);
			}
			else
			{
				SetAnimation(ID_ANIM_RED_PIRANHA_LOOK_DOWN_OPEN, true);
			}
		}

		// Maintain the correct horizontal orientation
		if (GetAnimator()) {
			GetAnimator()->SetFlipHorizontal(m_isFlipped);
		}

		// Fire a bullet after a delay
		if (m_timer >= m_fireDelay && m_activeBullet == nullptr)
		{
			// Fire a bullet in the direction of Mario
			BulletDirection fireDirection = GetFireDirection();
			Fire(fireDirection);
			Log(__FUNCTION__, "Fired bullet in direction: " + std::to_string(static_cast<int>(fireDirection)));
		}
		// Transition to receding state after firing time
		if (m_timer >= m_fireTime)
		{
			m_state = RECEDING;
			m_timer = 0.0f;

		}
		break;

	case RECEDING:
	{
		if (m_isLookingUp)
		{
			if(m_color == 0)
			{
				SetAnimation(ID_ANIM_GREEN_PIRANHA_LOOK_UP);
			}
			else
			{
				SetAnimation(ID_ANIM_RED_PIRANHA_LOOK_UP);
			}
		}
		else
		{
			if(m_color == 0)
			{
				SetAnimation(ID_ANIM_GREEN_PIRANHA_LOOK_DOWN);
			}
			else
			{
				SetAnimation(ID_ANIM_RED_PIRANHA_LOOK_DOWN);
			}
		}


		if (GetAnimator()) {
			GetAnimator()->SetFlipHorizontal(m_isFlipped);
		}
		if (m_pos.y < m_initialPosition.y)
		{
			vel.y = 50.0f;
			vel.x = 0.0f;
			SetVelocity(vel);
			SetPosition(GetPosition() + GetVelocity() * dt);

		}
		if (m_timer >= 2.0f)
		{


			m_state = HIDDEN;
			m_timer = 0.0f;
		}
		break;
	}
	}
	Entity::Update(dt);
	if (m_activeBullet != nullptr) {
		m_activeBullet->Update(dt);
	}
}

void FirePiranha::Render(DirectX::SpriteBatch* spriteBatch)
{
	// Call base class render to handle animation rendering
	Entity::Render(spriteBatch);

	if (m_activeBullet != nullptr) {
		m_activeBullet->Render(spriteBatch);
	}
}

void FirePiranha::OnCollision(const CollisionResult& event)
{

	Block* block = dynamic_cast<Block*>(event.collidedWith);

	Mario* mario = dynamic_cast<Mario*>(event.collidedWith);

	if (block) {
		if (block->IsSolid()) {}
	}

	if (mario)
	{
		mario->Damage();
		return;
	}


}


void FirePiranha::SetupCollisionComponent()
{
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x, curSize.y); // Make collision box slightly smaller
	m_collisionComponent->SetSize(newSize);

	m_collisionComponent->SetPosition(GetPosition() + Vector2(newSize.x / 2, newSize.y / 2));

	m_collisionComponent->SetSize(newSize); // Make collision box smaller than visual size
	Log(__FUNCTION__, "FirePiranha collision component initialized - collision disabled");
}


BulletDirection FirePiranha::GetFireDirection() const
{
	if (m_mario == nullptr)
		return m_isFlipped ? TOP_RIGHT_60 : TOP_LEFT_60; // Default direction

	Vector2 marioPos = m_mario->GetPosition();
	Vector2 plantPos = GetPosition();

	// Calculate directional vector from plant to Mario
	float dx = marioPos.x - plantPos.x;
	float dy = marioPos.y - plantPos.y;

	// Calculate actual angle in degrees (atan2 gives angle in radians)
	// atan2 returns angle in range [-π, π] where 0 is right, π/2 is up
	float angleRad = atan2(dy, dx);
	float angleDeg = angleRad * 180.0f / 3.14159f;

	// Normalize angle to [0, 360) range
	if (angleDeg < 0) {
		angleDeg += 360.0f;
	}

	angleDeg -= 90.0f; // Adjust to make 0 degrees point up

	if (angleDeg > 180.0f) angleDeg -= (angleDeg - 180.0f) * 2; // Normalize to [-180, 0] range

	angleDeg = abs(angleDeg); // Get absolute value

	Log(__FUNCTION__, "Mario angle from plant: " + std::to_string(angleDeg) + " degrees");

	if (angleDeg <= 50.0f) {
		return m_isFlipped ? TOP_RIGHT_45 : TOP_LEFT_45; // Right
	}
	else if (angleDeg <= 100.0f) {
		return m_isFlipped ? TOP_RIGHT_60 : TOP_LEFT_60; // Up
	}
	else if (angleDeg < 125.0f) {
		return m_isFlipped ? TOP_RIGHT_120 : TOP_LEFT_120; // Left
	}
	else {
		return m_isFlipped ? TOP_RIGHT_135 : TOP_LEFT_135; // Down
	}
}


void FirePiranha::Fire(BulletDirection direction)
{
	if (m_activeBullet != nullptr)
	{
		CleanupBullets();
	}

	// Calculate bullet spawn position based on plant's mouth position
	Vector2 bulletPosition = GetPosition();

	// Create the bullet with the appropriate direction
	Game* game = Game::GetInstance();
	SpriteSheet* spriteSheet = game->GetSpriteSheet();
	m_activeBullet = new Bullet(bulletPosition + BULLET_OFFSET, Vector2(BULLET_WIDTH, BULLET_HEIGHT), spriteSheet, direction);

}

void FirePiranha::CleanupBullets()
{
	// Delete any active bullet and set pointer to nullptr
	if (m_activeBullet != nullptr)
	{
		// Deactivate the bullet first
		m_activeBullet->Deactivate();

		// Delete the bullet object
		delete m_activeBullet;
		m_activeBullet = nullptr;

		Log(__FUNCTION__, "Bullet cleaned up");
	}
}

void FirePiranha::Die(DyingType type)
{
	if (!m_isDying) {
		EffectManager::GetInstance()->CreatePointEffect(GetPosition(), 100, true);
		m_dyingType = type;
		m_isDying = true;
		m_isCollidable = false;
		m_deathTimer = 0.0f;
		SetAnimation(ID_ANIM_EFFECT_PIRANHA_DIE, true);
		return;
	}
}

void FirePiranha::UpdatePlantOrientation(Mario* mario)
{
	if (mario == nullptr)
		return;

	Vector2 marioPos = mario->GetPosition();
	Vector2 plantPos = GetPosition();

	bool shouldFlip = (marioPos.x > plantPos.x);

	if (shouldFlip != m_isFlipped)
	{
		m_isFlipped = shouldFlip;
		if (GetAnimator())
		{
			GetAnimator()->SetFlipHorizontal(m_isFlipped);
		}
	}

	bool shouldLookUp = (marioPos.y < m_yThreshold);

	if (shouldLookUp != m_isLookingUp)
	{
		m_isLookingUp = shouldLookUp;

	}

}