#include "pch.h"
#include "AssetIDs.h"
#include "Block.h"
#include "Debug.h"
#include "GameConfig.h"
#include "Goomba.h"
#include "Mario.h"
#include "ParaGoomba.h"
#include "RedTroopas.h"
#include "HitBox.h"

using namespace GameConstants;
#include "LuckyBlock.h"

RedTroopas::RedTroopas(Vector2 position, Vector2 size, SpriteSheet* spriteSheet,bool hasWing, bool hasSpecialSize)
	: Troopa(position, size, spriteSheet), m_animTimer(0.0f),
	m_frameTime(0.15f), m_flipFrame(false), m_lastDirectionX(-1.0f),
	m_isColliding(false), m_reviveTimer(0.0f), m_reviveTime(5.0f),
	m_isVibrating(false), m_vibrateTimer(0.0f), m_vibrateInterval(0.0f),
	m_vibrateAmplitude(0.0f), m_vibrateCount(0), m_maxVibrateCount(0),
	m_hasWing(hasWing), m_isFlipped(false) {
	SetAnimation(ID_ANIM_RED_TROOPAS_WALK, true);
	m_visible = true;  // Set visibility to true
	m_isDying = false; // Initialize dying state

	// Set initial velocity - slow movement to the left
	SetVelocity(Vector2(-Enemies::Troopas::WALK_SPEED, 0.0f));

	// Setup collision component
	SetupCollisionComponent();

	// Initialize sprite direction based on initial velocity
	UpdateSpriteDirection();

	// Initialize interaction point states to ensure edge detection works properly
	m_pointCollisionState[InteractionPointType::LeftFoot] = true;
	m_pointCollisionState[InteractionPointType::RightFoot] = true;

	if (hasWing) {
		InitializeWing();
		SetVelocity(Vector2::Zero);
		m_startPositionY = position.y;
		m_isGoingUp = false;
	}
	else
		m_state = WALKING;

	if (hasSpecialSize) {
		m_ShellSize = Vector2(10.0f, 16.0f);
	}
	else {
		m_ShellSize = Vector2(16.0f, 16.0f);
	}
}

void RedTroopas::Render(DirectX::SpriteBatch* spriteBatch) {
	if (m_bonkEffects.size() > 0) {
		for (auto& effect : m_bonkEffects) {
			effect->Render(spriteBatch);
		}
	}

	if (m_hasWing && m_wing && m_wing->IsActive()) {
		m_wing->Render(spriteBatch);
	}

	Entity::Render(spriteBatch);
}

void RedTroopas::Die(DyingType type) {
	if (type == DyingType::BONKED) {
		SetAnimation(ID_ANIM_RED_TROOPAS_SHELL, false);
		m_isCollidable = false;
		m_isDying = true;
		m_state = DEAD;
		m_animator->SetFlipVertical(true);
		SetVelocity(
			Vector2(Enemies::Troopas::WALK_SPEED, -Enemies::DEATH_BOUNCE_VELOCITY));
		return;
	}
}

void RedTroopas::Update(float dt) {
	m_isGrounded = Collision::GetInstance()->GroundCheck(this, dt);

	if (m_isFlipped) {
		m_animator->SetFlipVertical(true);
	}
	else {
		m_animator->SetFlipVertical(false);
	}

	if (m_hasWing && m_wing && m_wing->IsActive()) {

		Vector2 pos = GetPosition();
		Vector2 vel = GetVelocity();
		// Update wing position
		m_wing->UpdatePosition(GetPosition());

		m_wing->SetDirection(m_lastDirectionX > 0 ? 1 : -1);
		m_wing->HandleFlapping(dt, 0.1f);
		m_wing->Update(dt);

		float destinationY = m_startPositionY + (!m_isGoingUp ? FLY_HEIGHT : 0);
		float distanceToDestination = abs(pos.y - destinationY);

		if (m_isGoingUp) {

			if (distanceToDestination > 48.0f) {
				vel.y -= VERTICAL_ACCELERATION * dt;
			}
			else if (distanceToDestination > 28.0f) {
				float accelerationMultiplier = distanceToDestination / 25.0f;
				vel.y -= VERTICAL_ACCELERATION * dt * accelerationMultiplier;
			}
			else {

				m_isGoingUp = false;
				float velocityScaler = distanceToDestination / 28.0f;
				vel.y = vel.y * velocityScaler;
			}
		}
		else {
			if (distanceToDestination > 48.0f) {
				vel.y += VERTICAL_ACCELERATION * dt;
			}
			else if (distanceToDestination > 28.0f) {
				float accelerationMultiplier = distanceToDestination / 25.0f;
				vel.y += VERTICAL_ACCELERATION * dt * accelerationMultiplier;
			}
			else {
				m_isGoingUp = true;
				float velocityScaler = distanceToDestination / 28.0f;
				vel.y = vel.y * velocityScaler;
			}
		}

		// Cap the vertical speed
		if (vel.y > MAX_FLYING_SPEED) {
			vel.y = MAX_FLYING_SPEED;
		}
		else if (vel.y < -MAX_FLYING_SPEED) {
			vel.y = -MAX_FLYING_SPEED;
		}

		SetVelocity(vel);
		SetPosition(GetPosition() + GetVelocity() * dt);
		Entity::Update(dt);
		return;
	}

	if (!m_isGrounded) {
		Vector2 vel = GetVelocity();
		vel.y += Enemies::Troopas::GRAVITY * dt;
		SetVelocity(vel);
	}

	if (m_state == DEAD && m_isActive) {
		m_deathTimer += dt;
		if (m_dyingType == DyingType::BONKED) {
			if (m_deathTimer >= Enemies::DEATH_BONK_ANI_TIME) {
				// After 2.0 seconds, deactivate the Goomba
				m_isActive = false;
				m_visible = false; // Remove collision component
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

	// Check for platform edges using raycasting if the entity is grounded
	if (m_isGrounded && m_state == WALKING) {
		CheckEdge();
	}

	UpdateSpriteDirection();

	SetPosition(GetPosition() + GetVelocity() * dt);

	if (!m_bonkEffects.empty()) {
		for (auto& effect : m_bonkEffects) {
			effect->Update(dt);
		}
	}
	Entity::Update(dt);
}

void RedTroopas::OnCollision(const CollisionResult& event) {
	Mario* mario = dynamic_cast<Mario*>(event.collidedWith);
	Block* block = dynamic_cast<Block*>(event.collidedWith);
	LuckyBlock* luckyBlock = dynamic_cast<LuckyBlock*>(event.collidedWith);
	HitBox* hitBox = dynamic_cast<HitBox*>(event.collidedWith);

	if (event.collidedWith->GetCollisionGroup() == CollisionGroup::NONSOLID &&
		event.contactNormal.x != 0) {
		return;
	}

	if (event.contactNormal.x != 0) // side collision
	{
		if(hitBox) {
			if(event.contactNormal.x <= 0)	return;
			else{
				Vector2 vel = GetVelocity();
				vel.x = -vel.x;
				SetVelocity(vel);
				return;
			} 
		}
		if (block) {
			if (block->IsSolid()) {
				Vector2 vel = GetVelocity();
				vel.x = -vel.x;
				SetVelocity(vel);
				block->Bump();
				return;
			}
		}

		if (mario) {
			if (m_state == SHELL_IDLE && mario->GetCurrentMStateName() != GameStrings::SWEEP) {
				Direction dir =
					event.contactNormal.x < 0 ? Direction::Left : Direction::Right;
				if (mario->Kick(dir, this)) {
					m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_SHELL_SLIDE, true);
					m_state = SHELL_SLIDE;
				}
				return;
			}
			else {
				mario->Damage();
				return;
			}
		}

		if (event.collidedWith) {
			if (m_state == SHELL_SLIDE) {
				event.collidedWith->Die(DyingType::BONKED);
				EffectManager::GetInstance()->CreateBonkEffect(
					event.collidedWith->GetPosition());
			}
		}
	}

	if (event.contactNormal.y < 0 && block) // foot collision
	{
		if (m_isBouncing) {
			HandleBounceCollision();
			return;
		}
		Vector2 vel = GetVelocity();
		vel.y = 0.0f;
		SetVelocity(vel);
		m_isGrounded = true;
		return;
	}

	if (event.contactNormal.y > 0) // Collision from above
	{
		if (mario) {
			if (m_hasWing) {
				TransformToTroopa();
				Vector2 vel = mario->GetVelocity();
				vel.y = Player::BOUNCE_FORCE; // Use Mario's bounce force
				mario->SetVelocity(vel);
				return;
			}
			if (m_state == WALKING) {
				Vector2 vel = mario->GetVelocity();
				vel.y = Player::BOUNCE_FORCE; // Use Mario's bounce force
				mario->SetVelocity(vel);
				TransformToShell();
				return;
			}
			if (m_state == SHELL_IDLE) {

				if (mario->GetPosition().x < GetPosition().x + 8.0f) {
					SetVelocity(Vector2(Enemies::Troopas::WALK_SPEED * 7, 0.0f));
				}
				else {
					SetVelocity(Vector2(-Enemies::Troopas::WALK_SPEED * 7, 0.0f));
				}

				Vector2 vel = mario->GetVelocity();
				vel.y = Player::BOUNCE_FORCE; // Use Mario's bounce force
				mario->SetVelocity(vel);

				m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_SHELL_SLIDE, true);
				m_state = SHELL_SLIDE;
				return;
			}
			if (m_state == SHELL_SLIDE) {
				Vector2 vel = mario->GetVelocity();
				vel.y = Player::BOUNCE_FORCE; // Use Mario's bounce force
				mario->SetVelocity(vel);

				m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_SHELL);
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

bool RedTroopas::CheckEdge() {
	if (m_state != WALKING || !m_isGrounded) {
		return false;
	}

	Vector2 position = GetPosition();
	Vector2 size = GetSize();
	Vector2 velocity = GetVelocity();

	// Cast rays downward from left and right edges of RedTroopas
	Vector2 leftFootPos = position + Vector2(-size.x / 4, size.y / 2 - 3.0f);
	Vector2 rightFootPos = position + Vector2(size.x / 4, size.y / 2 - 3.0f);

	// Ray length - slightly longer than needed to detect edges
	float rayLength = 5.0f;

	// Determine which ray to check based on movement direction
	Vector2 rayStartPos;
	if (velocity.x < 0) {
		// Moving left, check left foot
		rayStartPos = leftFootPos;
	}
	else {
		// Moving right, check right foot
		rayStartPos = rightFootPos;
	}

	Vector2 rayEndPos = rayStartPos + Vector2(0, rayLength);

	// Get cells around the entity for collision check
	std::vector<std::pair<int, int>> cells =
		Collision::GetInstance()->GetEntityCells(this, 0.016f);

	// Flag to track if we found ground under the ray
	bool foundGround = false;

	// Check for collisions with the ray
	for (const auto& cell : cells) {
		auto& gridCell =
			Collision::GetInstance()->GetGrid()[cell.first][cell.second];
		for (Entity* other : gridCell.entities) {
			// Skip if it's the same entity or not a potential ground
			if (other == this || !other->IsStatic() || !other->IsActive() ||
				!other->IsCollidable()) {
				continue;
			}

			// Get the other entity's collision rectangle
			Rectangle otherRect = other->GetCollisionComponent()->GetRectangle();

			if (rayStartPos.y > otherRect.y)
				continue;

			// Cast a ray downward
			Vector2 contactPoint, contactNormal;
			float contactTime;

			LuckyBlock* luckyBlock = dynamic_cast<LuckyBlock*>(other);
			if (luckyBlock && luckyBlock->m_isClaiming) {
				HandleSweepCollision(50.0f,m_bounceForce, false);
				break;
			}

			if (Collision::GetInstance()->RayVsRect(rayStartPos, rayEndPos, otherRect,
				contactPoint, contactNormal,
				contactTime)) {
				if (contactTime < 1.0f) {
					// We hit something, which means there's ground under the foot
					foundGround = true;
					break;
				}
			}
		}

		if (foundGround) {
			break;
		}
	}

	// If no ground was found under the ray, we're at an edge
	if (!foundGround) {
		// Change direction
		Vector2 vel = GetVelocity();
		vel.x = -vel.x;
		SetVelocity(vel);
		UpdateSpriteDirection();
		return true;
	}

	return false;
}

void RedTroopas::SetupCollisionComponent() {
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x, curSize.y);
	m_collisionComponent->SetSize(newSize);
	m_collisionComponent->SetPosition(GetPosition() +
		Vector2(newSize.x / 2, newSize.y / 2));
}

void RedTroopas::UpdateSpriteDirection() {
	Vector2 vel = GetVelocity();

	if (vel.x != 0 && ((vel.x > 0) != (m_lastDirectionX > 0))) {
		if (GetAnimator()) {
			GetAnimator()->SetFlipHorizontal(vel.x > 0);
		}
		m_lastDirectionX = vel.x;
	}
}

void RedTroopas::TransformToShell() {
	Vector2 offset = m_state == SHELL_IDLE ? Vector2(0.0f, 0.0f) : Vector2(0.0f, 5.0f);
	m_state = SHELL_IDLE;

	m_collisionComponent->SetSize(m_ShellSize);
	m_collisionComponent->SetPosition(GetPosition());
	SetPosition(GetPosition() + offset);
	SetVelocity(Vector2(0.0f, 0.0f));

	m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_SHELL);
}

void RedTroopas::TransformToTroopa() {
	if (m_isFlipped)
		m_isFlipped = false;

	if (m_hasWing) {
		m_wing->Deactivate();
		m_wing = nullptr;
		m_hasWing = false;
	}
	m_state = WALKING;

	Mario* m_mario = dynamic_cast<Mario*>(World::GetInstance()->GetPlayer());

	Vector2 newSize = Vector2(16.0f, 27.0f);
	m_collisionComponent->SetSize(newSize);
	m_collisionComponent->SetPosition(GetPosition() - Vector2(0.0f, 5.0f));

	if (m_mario) {
		Vector2 marioPos = m_mario->GetPosition();
		Vector2 goombaPos = GetPosition();

		if (marioPos.x < goombaPos.x + 8.0f) {
			SetVelocity(Vector2(-Enemies::Troopas::WALK_SPEED, 0.0f));
		}
		else {
			SetVelocity(Vector2(Enemies::Troopas::WALK_SPEED, 0.0f));
		}
	}
	m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_WALK, true);
}

void RedTroopas::StartVibration() {
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

		m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_REVIVE_SLOW, false);

		m_reviveTimer = 0.0f;
		m_reviveTime = 1.77f;
	}
}

void RedTroopas::UpdateVibration(float dt) {

	if (!m_isVibrating)
		return;

	m_vibrateTimer += dt;
	m_reviveTimer += dt;

	if (m_vibrateTimer >= m_vibrateInterval) {
		m_vibrateTimer = 0.0f;
		m_vibrateCount++;

		float offsetX =
			m_vibrateDirection ? -m_vibrateAmplitude : m_vibrateAmplitude;

		m_vibrateDirection = !m_vibrateDirection;

		// Before transforming, do a final check for blocks
		std::vector<std::pair<int, int>> cells =
			Collision::GetInstance()->GetEntityCells(this, dt);
		m_canRevive = true;

		for (const auto& cell : cells) {
			auto& gridEntities =
				Collision::GetInstance()->GetGrid()[cell.first][cell.second].entities;

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
		m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_REVIVE_SLOW, true);
	}

	if (m_reviveTimer >= 1.12f && !m_secondRevivePhase) {
		m_secondRevivePhase = true;
		m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_REVIVE_FAST, true);
	}

	if (m_reviveTimer >= m_reviveTime) {
		m_isVibrating = false;
		if (m_canRevive)
			TransformToTroopa();
		else
			Die(DyingType::BONKED);
	}
}

void RedTroopas::InitializeWing() {
	if (!m_hasWing)
		return;

	SpriteSheet* m_spriteSheet = Game::GetInstance()->GetSpriteSheet();

	// Create a single wing positioned behind the Troopa
	m_wing = new Wings(GetPosition(), Vector2(WINGS_WIDTH, WINGS_HEIGHT),
		m_spriteSheet);

	// Position the wing behind the Troopa
	m_wing->SetOffset(
		Vector2(4.0f, -9.0f)); // Centered horizontally, slightly above center

	// Set the wing's direction based on the Troopa's initial direction
	m_wing->SetDirection(GetVelocity().x < 0 ? -1 : 1);

	// Initially set animation to wings down
	m_wing->SetAnimation(ID_ANIM_WINGS_FLAP_DOWN, false);
}

void RedTroopas::HandleBounceCollision() {
	Vector2 vel = GetVelocity();
	vel.y = 0.0f;

	if (m_bounceCount < 1) {
		float currentBounceForce =
			m_bounceForce * pow(m_bounceDamping, m_bounceCount);
		vel.x *= 0.4f;
		vel.y = -currentBounceForce;
		m_bounceCount++;
		Log("k", "horizontal velocity: " + std::to_string(vel.x));

	}
	else {

		m_isBouncing = false;
		m_bounceCount = 0;
		vel.x = 0.0f;
	}

	SetVelocity(vel);
	m_isGrounded = true;
}

void RedTroopas::HandleSweepCollision(float x_Force, float y_Force, bool spawnEffect) {
	if(m_isBouncing) {
		return;
	}	
	if(spawnEffect) {
		EffectManager::GetInstance()->CreateBonkEffect(GetPosition());
	}
	m_isBouncing = true;
	m_bounceCount = 0;
	m_reviveTimer = 0.0f;

	Vector2 vel = GetVelocity();
	TransformToShell();

	float horizontalVel = vel.x >= 0 ? x_Force : -x_Force;

	SetVelocity(Vector2(horizontalVel, -y_Force));
	m_isFlipped = true;
}