#include "pch.h"
#include "AssetIDs.h"
#include "Block.h"
#include "Debug.h"
#include "GameConfig.h"
#include "Goomba.h"
#include "GreenTroopa.h"
#include "Mario.h"
#include "ParaGoomba.h"

using namespace GameConstants::Enemies::Troopas;

GreenTroopas::GreenTroopas(Vector2 position, Vector2 size, SpriteSheet* spriteSheet, bool hasWing)
	: Troopa(position, size, spriteSheet)
	, m_animTimer(0.0f)
	, m_frameTime(0.15f)
	, m_flipFrame(false)
	, m_lastDirectionX(-1.0f)
	, m_isColliding(false)
	, m_reviveTimer(0.0f)
	, m_reviveTime(5.0f)
	, m_isVibrating(false)
	, m_vibrateTimer(0.0f)
	, m_vibrateInterval(0.0f)
	, m_vibrateAmplitude(0.0f)
	, m_vibrateCount(0)
	, m_maxVibrateCount(0)
	, m_hasWing(hasWing)

{
	SetAnimation(ID_ANIM_GREEN_TROOPAS_WALK, true);
	m_visible = true; // Set visibility to true
	m_isDying = false; // Initialize dying state
	m_state = WALKING; // Initialize state to WALKING

	// Set initial velocity - slow movement to the left
	SetVelocity(Vector2(-WALK_SPEED, 0.0f));

	// Setup collision component
	SetupCollisionComponent();

	// Initialize sprite direction based on initial velocity
	UpdateSpriteDirection();

	// Initialize interaction point states to ensure edge detection works properly
	m_pointCollisionState[InteractionPointType::LeftFoot] = true;
	m_pointCollisionState[InteractionPointType::RightFoot] = true;

	if (m_hasWing) {
		InitializeWing();
	}
}

void GreenTroopas::Render(DirectX::SpriteBatch* spriteBatch)
{
	if (m_bonkEffects.size() > 0) {
		for (auto& effect : m_bonkEffects) {
			effect->Render(spriteBatch);
		}
	}

	// Render wing behind the Troopa if active
	if (m_hasWing && m_wing && m_wing->IsActive()) {
		m_wing->Render(spriteBatch);
	}

	Entity::Render(spriteBatch);
}

void GreenTroopas::Die(DyingType type)
{
	if (type == DyingType::BONKED) {
		SetAnimation(ID_ANIM_GREEN_TROOPAS_SHELL, false);
		m_isCollidable = false;
		m_isDying = true;
		m_state = DEAD;
		m_animator->SetFlipVertical(true);
		SetVelocity(Vector2(WALK_SPEED, -GameConstants::Enemies::DEATH_BOUNCE_VELOCITY));
		return;
	}
}


void GreenTroopas::Update(float dt)
{
	m_isGrounded = Collision::GetInstance()->GroundCheck(this, dt);
	// Log(LOG_INFO, "GreenTroopas Animation: " + std::to_string(m_animator->GetCurrentAnimation()));

	if (!m_isGrounded) {
		Vector2 vel = GetVelocity();
		vel.y += GameConstants::Physics::GRAVITY * dt;
		SetVelocity(vel);
	}

	if (m_hasWing && m_wing && m_wing->IsActive()) {
		// Update wing position
		m_wing->UpdatePosition(GetPosition());

		// Update wing direction based on Troopa direction
		m_wing->SetDirection(m_lastDirectionX > 0 ? 1 : -1);

		m_wing->HandleFlapping(dt, 0.2f);

		if (m_isGrounded)
		{
			Vector2 vel = GetVelocity();
			SetVelocity(Vector2(vel.x, -BOUNCE_FORCE));
		}
	}

	if (m_state == DEAD && m_isActive) {
		m_deathTimer += dt;
		if (m_dyingType == DyingType::BONKED)
		{
			if (m_deathTimer >= GameConstants::Enemies::DEATH_BONK_ANI_TIME) {
				// After 2.0 seconds, deactivate the Goomba
				m_isActive = false;
				m_visible = false;// Remove collision component
				return;
			}
			SetPosition(GetPosition() + GetVelocity() * dt);
			return;
		}
	}

	if (m_isVibrating) {
		UpdateVibration(dt);
		Entity::Update(dt);
		return;
	}

	if (m_state == SHELL_IDLE) {
		m_reviveTimer += dt;
		if (m_reviveTimer >= 5.0f && !m_isVibrating) {
			StartVibration();
			m_reviveTimer = 0.0f;
		}
	}
	else {
		m_reviveTimer = 0.0f;
	}

	UpdateSpriteDirection();

	SetPosition(GetPosition() + GetVelocity() * dt);

	if (!m_bonkEffects.empty()) {
		for (auto& effect : m_bonkEffects) {
			effect->Update(dt);
		}
	}
	// Log(LOG_INFO, "GreenTroopas::Update - Position: " + std::to_string(GetPosition().x) + ", " + std::to_string(GetPosition().y));
	Entity::Update(dt);
}


void GreenTroopas::OnCollision(const CollisionResult& event)
{
	Mario* mario = dynamic_cast<Mario*>(event.collidedWith);
	Block* block = dynamic_cast<Block*>(event.collidedWith);

	if (event.collidedWith->GetCollisionGroup() == CollisionGroup::NONSOLID && event.contactNormal.x != 0) {
		return;
	}

	if (event.contactNormal.x != 0)  // side collision
	{
		if (block)
		{
			if (block->IsSolid())
			{
				Vector2 vel = GetVelocity();
				vel.x = -vel.x;
				SetVelocity(vel);
				block->Bump();
				return;
			}
		}

		if (mario) {
			if (m_state == SHELL_IDLE) {
				Direction dir = event.contactNormal.x < 0 ? Direction::Left : Direction::Right;
				if (mario->Kick(dir, this)) {
					m_animator->SetAnimation(ID_ANIM_GREEN_TROOPAS_SHELL_SLIDE, true);
					m_state = SHELL_SLIDE;
				}
				return;
			}
			else {
				mario->Damage();
				return;
			}
		}

		if (event.collidedWith)
		{
			if (m_state == SHELL_SLIDE)
			{
				event.collidedWith->Die(DyingType::BONKED);
				EffectManager::GetInstance()->CreateBonkEffect(event.collidedWith->GetPosition());
			}
		}
	}

	if (event.contactNormal.y < 0 && block) // foot collision
	{
		Vector2 vel = GetVelocity();
		vel.y = 0.0f;
		SetVelocity(vel);
		m_isGrounded = true;
		return;

		if (mario) {
			// mario->Damage();
		}
	}

	if (event.contactNormal.y > 0) // Collision from above
	{
		if (mario) {
			if (m_hasWing) {
				TransformToRegularTroopa();
				Vector2 vel = mario->GetVelocity();
				vel.y = GameConstants::Player::BOUNCE_FORCE; // Use Mario's bounce force
				mario->SetVelocity(vel);
				return;
			}

			if (m_state == WALKING) {
				Vector2 vel = mario->GetVelocity();
				vel.y = GameConstants::Player::BOUNCE_FORCE; // Use Mario's bounce force
				mario->SetVelocity(vel);
				TransformToShell();
				return;
			}
			if (m_state == SHELL_IDLE) {

				if (mario->GetPosition().x < GetPosition().x + 8.0f) {
					SetVelocity(Vector2(WALK_SPEED * 7, 0.0f));
				}
				else {
					SetVelocity(Vector2(-WALK_SPEED * 7, 0.0f));
				}

				Vector2 vel = mario->GetVelocity();
				vel.y = GameConstants::Player::BOUNCE_FORCE; // Use Mario's bounce force
				mario->SetVelocity(vel);

				m_animator->SetAnimation(ID_ANIM_GREEN_TROOPAS_SHELL_SLIDE, true);
				m_state = SHELL_SLIDE;
				return;
			}
			if (m_state == SHELL_SLIDE) {
				Vector2 vel = mario->GetVelocity();
				vel.y = GameConstants::Player::BOUNCE_FORCE; // Use Mario's bounce force
				mario->SetVelocity(vel);

				m_animator->SetAnimation(ID_ANIM_GREEN_TROOPAS_SHELL);
				SetVelocity(Vector2(0.0f, 0.0f));
				m_state = SHELL_IDLE;
				return;
			}
			if (block) {
				if (block->IsSolid()) {
					Vector2 vel = GetVelocity();
					vel.y = 0.0f;
					SetVelocity(vel);
				}
				return;
			}
		}
	}

	else if (mario && !m_isDying) {
	}
}

void GreenTroopas::SetupCollisionComponent()
{
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x, curSize.y);
	m_collisionComponent->SetSize(newSize);
	m_collisionComponent->SetPosition(GetPosition() + Vector2(newSize.x / 2, newSize.y / 2));
}


void GreenTroopas::UpdateSpriteDirection()
{
	Vector2 vel = GetVelocity();

	if (vel.x != 0 && ((vel.x > 0) != (m_lastDirectionX > 0)))
	{
		if (GetAnimator())
		{
			GetAnimator()->SetFlipHorizontal(vel.x > 0);
		}
		m_lastDirectionX = vel.x;
	}
}

void GreenTroopas::TransformToShell()
{
	SetState(TroopaState::SHELL_IDLE);

	Vector2 newSize = Vector2(16.0f, 16.0f);
	m_collisionComponent->SetSize(newSize);
	m_collisionComponent->SetPosition(GetPosition());
	SetPosition(GetPosition() + Vector2(0.0f, 5.0f));
	SetVelocity(Vector2(0.0f, 0.0f));

	m_animator->SetAnimation(ID_ANIM_GREEN_TROOPAS_SHELL);

}

void GreenTroopas::TransformToTroopa()
{
	m_state = WALKING;

	Mario* m_mario = dynamic_cast<Mario*>(World::GetInstance()->GetPlayer());

	Vector2 newSize = Vector2(16.0f, 27.0f);
	m_collisionComponent->SetSize(newSize);
	m_collisionComponent->SetPosition(GetPosition() - Vector2(0.0f, 5.0f));

	if (m_mario) {
		Vector2 marioPos = m_mario->GetPosition();
		Vector2 goombaPos = GetPosition();

		if (marioPos.x < goombaPos.x + 8.0f) {
			SetVelocity(Vector2(-WALK_SPEED, 0.0f));
		}
		else {
			SetVelocity(Vector2(WALK_SPEED, 0.0f));
		}
	}
	m_animator->SetAnimation(ID_ANIM_GREEN_TROOPAS_WALK, true);
}


void GreenTroopas::StartVibration() {
	if (!m_isVibrating) {
		m_isVibrating = true;
		m_vibrateTimer = 0.0f;
		m_vibrateInterval = 0.03f;
		m_vibrateAmplitude = 1.0f;
		m_vibrateCount = 0;
		m_maxVibrateCount = 20;
		m_vibrateDirection = true;
		m_firstRevivePhase = false;
		m_secondRevivePhase = false;


		m_animator->SetAnimation(ID_ANIM_GREEN_TROOPAS_REVIVE_SLOW, false);

		m_reviveTimer = 0.0f;
		m_reviveTime = 1.77f;
	}
}

void GreenTroopas::UpdateVibration(float dt) {
	if (!m_isVibrating) return;

	m_vibrateTimer += dt;
	m_reviveTimer += dt;

	if (m_vibrateTimer >= m_vibrateInterval) {
		m_vibrateTimer = 0.0f;
		m_vibrateCount++;

		float offsetX = m_vibrateDirection ? -m_vibrateAmplitude : m_vibrateAmplitude;

		m_vibrateDirection = !m_vibrateDirection;

		// Before transforming, do a final check for blocks
		std::vector<std::pair<int, int>> cells = Collision::GetInstance()->GetEntityCells(this, dt);
		m_canRevive = true;

		for (const auto& cell : cells) {
			auto& gridEntities = Collision::GetInstance()->GetGrid()[cell.first][cell.second].entities;

			for (Entity* other : gridEntities) {
				Block* block = dynamic_cast<Block*>(other);
				if (block && block->IsSolid() && block->IsActive()) {
					Rectangle blockRect = block->GetCollisionComponent()->GetRectangle();
					Rectangle futureShellRect = GetCollisionComponent()->GetRectangle();

					if (futureShellRect.x < blockRect.x + blockRect.width &&
						futureShellRect.x + futureShellRect.width > blockRect.x &&
						futureShellRect.y < blockRect.y + blockRect.height &&
						futureShellRect.y + futureShellRect.height > blockRect.y) {
						m_canRevive = false;
					}
				}
			}
		}

		SetPosition(GetPosition() + Vector2(offsetX, 0.0f));
	}

	if (m_reviveTimer > 0.5f && m_reviveTimer < 1.12f && !m_firstRevivePhase) {
		m_firstRevivePhase = true;
		m_animator->SetAnimation(ID_ANIM_GREEN_TROOPAS_REVIVE_SLOW, true);
	}

	if (m_reviveTimer >= 1.12f && !m_secondRevivePhase) {
		m_secondRevivePhase = true;
		m_animator->SetAnimation(ID_ANIM_GREEN_TROOPAS_REVIVE_FAST, true);
	}

	if (m_reviveTimer >= m_reviveTime) {
		m_isVibrating = false;
		if (m_canRevive)
			TransformToTroopa();
		else
			Die(DyingType::BONKED);
	}
}

void GreenTroopas::InitializeWing()
{
	if (!m_hasWing) return;

	SpriteSheet* m_spriteSheet = Game::GetInstance()->GetSpriteSheet();

	// Create a single wing positioned behind the Troopa
	m_wing = new Wings(GetPosition(), Vector2(WINGS_WIDTH, WINGS_HEIGHT), m_spriteSheet);

	// Position the wing behind the Troopa
	m_wing->SetOffset(Vector2(4.0f, -9.0f)); // Centered horizontally, slightly above center

	// Set the wing's direction based on the Troopa's initial direction
	m_wing->SetDirection(GetVelocity().x < 0 ? -1 : 1);

	// Initially set animation to wings down
	m_wing->SetAnimation(ID_ANIM_WINGS_FLAP_DOWN, false);
}

void GreenTroopas::TransformToRegularTroopa()
{
	if (m_hasWing && m_wing) {
		// Deactivate wing
		m_wing->Deactivate();
		m_hasWing = false;

		// Continue moving as a regular Troopa
		SetVelocity(Vector2(-WALK_SPEED, 0.0f));
	}
}