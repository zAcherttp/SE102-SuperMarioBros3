#include "pch.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "FloatingPlatform.h"
#include "GameConfig.h"
#include "Mario.h"

using namespace GameConstants::Blocks::FloatingPlatform;

FloatingPlatform::FloatingPlatform(Vector2 position, Vector2 size, int countX, bool isSolid, SpriteSheet* spriteSheet, Vector2 patrolOffset, float roundTime)
	: Block(position, size, spriteSheet)
{
	m_tileXcount = countX;
	m_tileYcount = 1;
	m_isSolid = isSolid;
	m_isStatic = true;

	m_patrolOffset = patrolOffset;
	m_roundTime = roundTime;
	m_startPosition = position;

	SetVelocity(Vector2(-32.f, 0.f));

	m_entitiesOnPlaform = {};

	// update the collision box to match the size of the ground
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x * m_tileXcount, curSize.y);
	m_collisionComponent->SetSize(newSize);
	// update the position as size is the center of the entity
	m_collisionComponent->SetPosition(position + Vector2(newSize.x / 2, newSize.y / 2));
}

void FloatingPlatform::Fall()
{
	if (m_isFalling) return;
	m_isFalling = true;
	SetVelocity(Vector2(0.f, 0.f));
}

bool FloatingPlatform::GetIsFalling() const
{
	return m_isFalling;
}

void FloatingPlatform::Render(DirectX::SpriteBatch* spriteBatch)
{
	Vector2 size = m_collisionComponent->GetSize();
	Vector2 tileSize = Vector2(size.x / m_tileXcount, size.y / m_tileYcount);
	Vector2 pos = m_collisionComponent->GetPosition() - Vector2(size.x / 2 - tileSize.x / 2, size.y / 2 - tileSize.y / 2);

	// draw the first tile
	m_animator->Draw(spriteBatch, ID_SPRITE_MOVING_PLATFORM_LEFT, pos, 0.5f);
	for (int i = 1; i < m_tileXcount - 1; ++i)  // draw the mid tiles
	{
		m_animator->Draw(spriteBatch, ID_SPRITE_MOVING_PLATFORM_MID, pos + Vector2(i * tileSize.x, 0), 0.5f);
	}
	// draw the last tile
	m_animator->Draw(spriteBatch, ID_SPRITE_MOVING_PLATFORM_RIGHT, pos + Vector2((m_tileXcount - 1) * tileSize.x, 0), 0.5f);
}

void FloatingPlatform::Update(float dt)
{
	if (!Game::GetInstance()->GetCamera()->IsInGameView(GetPosition(), 64)) return;
	Vector2 oldPosition = GetPosition();

	if (!m_isFalling) {
		//m_roundTimer += dt;

		//	// update position based on patrol offset and round time
		//	// move linearly between the two points
		//	if (m_roundTimer >= m_roundTime) {
		//		m_roundTimer = 0.0f;
		//	}

		//	// [0, 1] range for the timer -> posFactor 1 -> 0 -> 1
		//	float posFactor = 0.0f;
		//	if (m_roundTimer >= m_roundTime / 2.0f) {
		//		posFactor = 1.0f - (m_roundTimer - m_roundTime / 2.0f) / (m_roundTime / 2.0f);
		//	}
		//	else {
		//		posFactor = m_roundTimer / (m_roundTime / 2.0f);
		//	}

		//	// calculate the appropriate velocity based on the movement direction
		//	Vector2 velocity = m_patrolOffset / m_roundTime * 2.f;
		//	m_collisionComponent->SetVelocity(velocity * (m_roundTimer > m_roundTime / 2.0f ? -1.0f : 1.0f));
		//	Vector2 newPosition = m_startPosition + m_patrolOffset * posFactor;
		//	m_collisionComponent->SetPosition(newPosition);

		SetPosition(GetPosition() + GetVelocity() * dt);


	}
	else {
		Vector2 velocity = GetVelocity();
		velocity.y += ACCEL_FALL * dt;

		Vector2 position = GetPosition() + velocity * dt;
		SetVelocity(velocity);
		SetPosition(position);
	}

	// Update entities on platform after platform movement
	UpdateEntitiesOnPlatform(dt);

	if (GetPosition().y > 600.f) m_isActive = false;
}

void FloatingPlatform::OnCollision(const CollisionResult& event) {
	Entity* ent = dynamic_cast<Entity*>(event.collidedWith);

	// Only add entity if it's landing on top of the platform
	if (event.contactNormal.y > 0.7f && ent) // Use threshold to ensure it's mostly vertical contact
	{
		// Check if entity is not already on the platform
		auto it = std::find(m_entitiesOnPlaform.begin(), m_entitiesOnPlaform.end(), ent);
		if (it == m_entitiesOnPlaform.end()) {
			m_entitiesOnPlaform.push_back(ent);

			// Position entity exactly on top of platform
			Vector2 entPos = ent->GetPosition();
			Vector2 entSize = ent->GetSize();
			Vector2 platformPos = GetPosition();
			Vector2 platformSize = GetSize();

			entPos.y = platformPos.y - platformSize.y / 2 - entSize.y / 2;
			ent->SetPosition(entPos);

			// Stop downward velocity when landing
			Vector2 entVel = ent->GetVelocity();
			if (entVel.y > 0.f) {
				entVel.y = 0.f;
				ent->SetVelocity(entVel);
			}
		}
	}
}

void FloatingPlatform::OnNoCollision(float dt, Axis axis) {
	// Don't clear entities here - let UpdateEntitiesOnPlatform handle it
	// This prevents entities from being removed too aggressively
}
bool FloatingPlatform::IsEntityOnPlatform(Entity* entity) const
{
	if (!entity) return false;

	Vector2 platformPos = GetPosition();
	Vector2 platformSize = GetSize();
	Vector2 entityPos = entity->GetPosition();
	Vector2 entitySize = entity->GetSize();
	Vector2 entityVel = entity->GetVelocity();

	// Check if entity is horizontally within platform bounds (with small margin)
	float horizontalMargin = 10.0f;
	bool withinHorizontalBounds = (entityPos.x + entitySize.x / 2 > platformPos.x - platformSize.x / 2 - horizontalMargin) &&
		(entityPos.x - entitySize.x / 2 < platformPos.x + platformSize.x / 2 + horizontalMargin);

	// Check if entity is on top of platform (within reasonable distance)
	float verticalTolerance = 15.0f;
	float entityBottom = entityPos.y + entitySize.y / 2;
	float platformTop = platformPos.y - platformSize.y / 2;
	bool onTop = entityBottom >= platformTop - verticalTolerance &&
		entityBottom <= platformTop + verticalTolerance;

	// If entity is jumping (has significant upward velocity), don't consider it on platform
	bool isJumping = entityVel.y < -50.0f; // Adjust threshold as needed

	return withinHorizontalBounds && onTop && !isJumping;
}

void FloatingPlatform::UpdateEntitiesOnPlatform(float dt)
{
	// Calculate platform movement for this frame
	Vector2 currentPosition = GetPosition();
	Vector2 platformMovement = currentPosition - m_previousPosition;

	// Update entities that are still on the platform
	auto it = m_entitiesOnPlaform.begin();
	while (it != m_entitiesOnPlaform.end()) {
		Entity* entity = *it;

		if (!entity || !IsEntityOnPlatform(entity)) {
			// Remove entity from platform if it's no longer on it
			it = m_entitiesOnPlaform.erase(it);
			continue;
		}

		Vector2 entityPos = entity->GetPosition();
		Vector2 entityVel = entity->GetVelocity();

		if (m_isFalling) {
			// When falling, ensure entity stays on top of platform
			Vector2 platformSize = GetSize();
			Vector2 entitySize = entity->GetSize();

			// Calculate correct position on top of platform
			float correctY = currentPosition.y - platformSize.y / 2 - entitySize.y / 2;

			// Only adjust position if entity would be below the platform surface
			// This prevents piercing while allowing jumping
			if (entityPos.y + entitySize.y / 2 > currentPosition.y - platformSize.y / 2) {
				entityPos.y = correctY;
				entity->SetPosition(entityPos);

				// Match platform's falling velocity if entity isn't jumping upward
				if (entityVel.y >= 0) {
					entityVel.y = GetVelocity().y;
					entity->SetVelocity(entityVel);
				}
			}
		}
		else {
			// When moving normally, move entity with platform horizontally
			// Only apply horizontal movement, let entity handle its own vertical movement
			entityPos.x += platformMovement.x;
			entity->SetPosition(entityPos);
		}

		++it;
	}

	m_previousPosition = currentPosition;
}