#include "pch.h"
#include "Collision.h"
#include "CollisionComponent.h"
#include "Mario.h"
#include "Debug.h"
#include "RedTroopas.h"
#include "Block.h"
#include "Enemy.h"
#include "Paragoomba.h"
#include "DebugOverlay.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Collision* Collision::s_instance = nullptr;

Collision::Collision(int worldWidth, int worldHeight, int cellSize)
	: m_worldWidth(worldWidth), m_worldHeight(worldHeight), m_cellSize(cellSize)
{
	m_gridWidth = (worldWidth + cellSize - 1) / cellSize;
	m_gridHeight = (worldHeight + cellSize - 1) / cellSize;

	// Initialize the grid
	m_grid.resize(m_gridWidth);
	for (int x = 0; x < m_gridWidth; x++) {
		m_grid[x].resize(m_gridHeight);
	}

	s_instance = this;

	Log(__FUNCTION__, "Collision system initialized with grid size: " +
		std::to_string(m_gridWidth) + "x" + std::to_string(m_gridHeight));
}

Collision::~Collision()
{
	Clear();
}

void Collision::Clear()
{
	for (int x = 0; x < m_gridWidth; x++) {
		for (int y = 0; y < m_gridHeight; y++) {
			m_grid[x][y].entities.clear();
		}
	}
	m_entityCells.clear();
}

std::pair<int, int> Collision::GetCellCoords(const Vector2& position)
{
	int cellX = static_cast<int>(position.x) / m_cellSize;
	int cellY = static_cast<int>(position.y) / m_cellSize;

	// Clamp to grid bounds
	cellX = std::max(0, std::min(cellX, m_gridWidth - 1));
	cellY = std::max(0, std::min(cellY, m_gridHeight - 1));

	return { cellX, cellY };
}

std::vector<std::pair<int, int>> Collision::GetEntityCells(const Entity* entity, float dt)
{
	std::vector<std::pair<int, int>> cells;

	// Get entity's bounding box
	RECT bbox = entity->GetCollisionComponent()->GetRect();
	Vector2 velocity = entity->GetVelocity() * dt;

	// Expand bbox by velocity
	RECT sweptBbox = bbox;
	sweptBbox.left = std::min(bbox.left, bbox.left + (LONG)velocity.x);
	sweptBbox.right = std::max(bbox.right, bbox.right + (LONG)velocity.x);
	sweptBbox.top = std::min(bbox.top, bbox.top + (LONG)velocity.y);
	sweptBbox.bottom = std::max(bbox.bottom, bbox.bottom + (LONG)velocity.y);

	// Get cells for all corners of the bounding box
	std::pair<int, int> topLeft
		= GetCellCoords(Vector2(static_cast<float>(sweptBbox.left), static_cast<float>(sweptBbox.top)));
	std::pair<int, int> bottomRight
		= GetCellCoords(Vector2(static_cast<float>(sweptBbox.right), static_cast<float>(sweptBbox.bottom)));

	// Add all cells that the entity occupies
	for (int x = topLeft.first; x <= bottomRight.first; x++) {
		for (int y = topLeft.second; y <= bottomRight.second; y++) {
			if (x >= 0 && x < m_gridWidth && y >= 0 && y < m_gridHeight) {
				cells.push_back({ x, y });
			}
		}
	}

	return cells;
}

void Collision::AddEntity(Entity* entity, float dt)
{
	if (!entity) return;

	// Get all cells the entity occupies
	std::vector<std::pair<int, int>> cells = GetEntityCells(entity, dt);

	// Add entity to each cell
	for (const auto& cell : cells) {
		m_grid[cell.first][cell.second].entities.push_back(entity);
	}

	// Store which cells the entity is in
	m_entityCells[entity] = cells;
}

void Collision::RemoveEntity(Entity* entity)
{
	if (!entity || m_entityCells.find(entity) == m_entityCells.end()) return;

	// Get cells the entity is in
	const auto& cells = m_entityCells[entity];

	// Remove entity from each cell
	for (const auto& cell : cells) {
		auto& cellEntities = m_grid[cell.first][cell.second].entities;
		cellEntities.erase(std::remove(cellEntities.begin(), cellEntities.end(), entity), cellEntities.end());
	}

	// Remove entity from tracking
	m_entityCells.erase(entity);
}

void Collision::UpdateEntity(Entity* entity, float dt)
{
	// Remove entity from current cells
	RemoveEntity(entity);

	// Add entity to new cells based on updated position
	AddEntity(entity, dt);
}

std::vector<Entity*> Collision::GetPotentialCollisions(Entity* entity)
{
	std::vector<Entity*> potentialCollisions;

	if (m_entityCells.find(entity) == m_entityCells.end()) {
		// Entity not in spatial grid yet
		return potentialCollisions;
	}

	// Get all cells the entity is in
	const auto& cells = m_entityCells[entity];

	// Get all entities in those cells
	std::unordered_set<Entity*> uniqueEntities;
	for (const auto& cell : cells) {
		for (Entity* other : m_grid[cell.first][cell.second].entities) {
			if (other != entity && uniqueEntities.find(other) == uniqueEntities.end()) {
				uniqueEntities.insert(other);
				potentialCollisions.push_back(other);
			}
		}
	}

	return potentialCollisions;
}

void Collision::SweptAABB(Entity* movingEntity, Entity* staticEntity, float dt, CollisionResult& result, Axis axis)
{
	result.collided = false;

	if (RayEntVsEnt(*movingEntity, *staticEntity, result.contactPoint, result.contactNormal, result.contactTime, dt, axis)) {
		// Collision detected
		result.collided = true;
		result.collidedWith = staticEntity;
	}
}

void Collision::CheckInteractionPointCollision(Entity* entity, Entity* other, float dt, CollisionResult& result, Axis axis)
{
	// Get entity velocities
	Vector2 velocity = entity->GetVelocity() * dt;
	Vector2 relativeVelocity = velocity;

	// If the second entity is also moving, use relative velocity
	if (!other->IsStatic()) {
		relativeVelocity = velocity - other->GetVelocity() * dt;
	}

	axis == Axis::X ? relativeVelocity.y = 0 : relativeVelocity.x = 0;

	// If not moving, no collision will occur
	if (relativeVelocity.x == 0 && relativeVelocity.y == 0) {
		result.collided = false;
		return;
	}

	// Check if entity implements interaction points
	auto points = entity->GetInteractionPoints();
	if (points.empty()) {
		return; // No interaction points = no collision for points
	}

	// Get other entity's bounding box
	RECT otherBox = other->GetCollisionComponent()->GetRect();

	// Convert RECT to Rectangle
	Rectangle rectangle;
	rectangle.x = static_cast<long>(otherBox.left);
	rectangle.y = static_cast<long>(otherBox.top);
	rectangle.width = static_cast<long>(otherBox.right - otherBox.left);
	rectangle.height = static_cast<long>(otherBox.bottom - otherBox.top);

	// Check each interaction point
	for (const auto& [type, localPoint] : points) {
		// Convert to world space
		Vector2 worldPointStart = localPoint + entity->GetPosition();
		Vector2 worldPointEnd = worldPointStart + relativeVelocity;
		Vector2 contactPoint, contactNormal;
		float contactTime;

		if (RayVsRect(worldPointStart, worldPointEnd, rectangle, contactPoint, contactNormal, contactTime)
			&& (contactTime >= 0.0f && contactTime < 1.0f)) {
			result.collided = true;
			result.collidedWith = other;
			result.pointType = type;
			result.contactNormal = contactNormal;
			result.contactPoint = contactPoint;
			result.contactTime = contactTime;
		}
	}
}

CollisionResult Collision::CheckCollision(Entity* movingEntity, Entity* staticEntity, float dt, Axis axis)
{
	// Try interaction point collision first if entity is flagged for it
	CollisionResult result;
	if (movingEntity->UsesInteractionPoints()) {
		CheckInteractionPointCollision(movingEntity, staticEntity, dt, result, axis);
		return result;
	}
	// Fall back to Swept AABB collision
	SweptAABB(movingEntity, staticEntity, dt, result, axis);
	return result;
}

void Collision::ResolveCollision(Entity* entity, const CollisionResult& result, float dt, Axis axis)
{
	if (!result.collided) return;

	// Add collision to debug visualization
	DebugCollisionInfo info;
	info.position = result.contactPoint;
	info.normal = result.contactNormal;
	info.timeToLive = DEBUG_COLLISION_TTL;
	m_debugCollisions.push_back(info);

	// Different collision handling based on entity type
	if (entity->UsesInteractionPoints()) {

		// Delegate collision handling to entity based on point type
		switch (result.pointType) {
		case InteractionPointType::TopHead:
			entity->OnTopHeadCollision(result);
			break;
		case InteractionPointType::LeftFoot:
		case InteractionPointType::RightFoot:
			entity->OnFootCollision(result);
			break;
		case InteractionPointType::LeftUpper:
		case InteractionPointType::LeftLower:
			entity->OnLeftSideCollision(result);
			break;
		case InteractionPointType::RightUpper:
		case InteractionPointType::RightLower:
			entity->OnRightSideCollision(result);
			break;
		}

		Vector2 velocity = entity->GetVelocity();
		Vector2 position = entity->GetPosition();
		if (axis == Axis::X) {
			position.x += velocity.x * dt;
		}
		else {
			position.y += velocity.y * dt;
		}
		entity->SetPosition(position);
	}
	else {
		entity->OnCollision(result);
	}

	// Notify the other entity (if not static)
	if (result.collidedWith->IsActive()) {
		CollisionResult otherEvent = result;
		otherEvent.collidedWith = entity;
		otherEvent.contactNormal = -result.contactNormal; // Reverse normal for the other entity
		result.collidedWith->OnCollision(otherEvent);
	}
}

void Collision::ProcessCollisions(float dt)
{
	UpdateDebugInfo(dt);

	// Process X-axis collisions
	for (auto& pair : m_entityCells) {
		Entity* entity = pair.first;

		// Skip static and inactive entities
		if (entity->IsStatic() || !entity->IsActive() || !entity->IsCollidable()) {
			continue;
		}

		// Get potential collision candidates
		std::vector<Entity*> potentialCollisions = GetPotentialCollisions(entity);

		ResolveOverlaps(entity, potentialCollisions);

		//Log(LOG_INFO, std::to_string(potentialCollisions.size()));

		// Find earliest collision
		CollisionResult earliestCollision;
		earliestCollision.contactTime = 1.0f;
		earliestCollision.collided = false;

		for (Entity* other : potentialCollisions) {
			// Skip inactive entities
			if (!other->IsActive() || !other->IsCollidable()) {
				continue;
			}
			// Check collision
			CollisionResult result = CheckCollision(entity, other, dt, Axis::X);

			// Keep track of the earliest collision
			if (result.collided && result.contactTime < earliestCollision.contactTime) {
				earliestCollision = result;
			}
		}
		// Resolve the earliest collision
		if (earliestCollision.collided) {
			ResolveCollision(entity, earliestCollision, dt, Axis::X);
		}
		else {
			entity->OnNoCollision(dt, Axis::X);
		}

		// Process Y-axis collisions
		earliestCollision.contactTime = 1.0f;
		earliestCollision.collided = false;

		for (Entity* other : potentialCollisions) {
			if (!other->IsActive() || !other->IsCollidable()) continue;
			CollisionResult result = CheckCollision(entity, other, dt, Axis::Y);
			if (result.collided && result.contactTime < earliestCollision.contactTime)
				earliestCollision = result;
		}
		if (earliestCollision.collided)
			ResolveCollision(entity, earliestCollision, dt, Axis::Y);
		else
			entity->OnNoCollision(dt, Axis::Y);
	}
}

void Collision::ResolveOverlaps(Entity* entity, const std::vector<Entity*>& potentialCollisions)
{
	//Log(LOG_INFO, "RESOLVING OVERLAP");

	if (entity->IsStatic() || !entity->IsActive() || !entity->IsCollidable()) {
		return;
	}
	Mario* mario = dynamic_cast<Mario*>(entity);
	ParaGoomba* prgoomba = dynamic_cast<ParaGoomba*>(entity);
	if (!mario || !prgoomba) return;

	// Check for overlaps with each potential collision
	for (Entity* other : potentialCollisions) {
		// Skip null, inactive, or non-collidable entities
		if (other == nullptr || !other->IsActive() || !other->IsCollidable()) {
			continue;
		}

		// Skip non-solid blocks or enemies
		Block* block = dynamic_cast<Block*>(other);
		Enemy* enemy = dynamic_cast<Enemy*>(other);
		if (block && !block->IsSolid() || enemy) {
			continue;
		}

		// Verify collision components
		CollisionComponent* entityCollision = entity->GetCollisionComponent();
		CollisionComponent* otherCollision = other->GetCollisionComponent();
		if (entityCollision == nullptr || otherCollision == nullptr) {
			continue;
		}

		// Get entity bounding boxes
		RECT entityRect = entityCollision->GetRect();
		RECT otherRect = otherCollision->GetRect();

		// Check if rectangles overlap
		RECT intersection;
		if (IntersectRect(&intersection, &entityRect, &otherRect)) {
			// Calculate overlap dimensions
			float entityMidpointY = (entityRect.top + entityRect.bottom) / 2.0f;

			// Only resolve if intersection is in the lower half of the entity
			if (intersection.top > entityMidpointY || intersection.bottom > entityMidpointY) {
				int overlapWidth = intersection.right - intersection.left;
				int overlapHeight = intersection.bottom - intersection.top;

				// Only resolve significant overlaps (avoid floating point precision issues)
				if (overlapWidth > 1 && overlapHeight > 1) {
					// Determine which axis has the smallest overlap to resolve along that axis first
					Vector2 vel = entity->GetVelocity();

					if (overlapWidth <= overlapHeight) {
						// Resolve X-axis overlap
						Vector2 contactNormal;

						// Determine push direction based on entity centers
						Vector2 entityCenter(
							(entityRect.left + entityRect.right) / 2.0f,
							(entityRect.top + entityRect.bottom) / 2.0f);

						Vector2 otherCenter(
							(otherRect.left + otherRect.right) / 2.0f,
							(otherRect.top + otherRect.bottom) / 2.0f);

						// Calculate normal direction from other entity to this entity
						if (entityCenter.x < otherCenter.x) {
							contactNormal = Vector2(-1, 0); // Push left
						}
						else {
							contactNormal = Vector2(1, 0);  // Push right
						}

						// If other entity is static, move only the current entity
						if (other->IsStatic()) {
							// Move entity out of collision
							Vector2 position = entity->GetPosition();
							position.x += contactNormal.x * overlapWidth;
							entity->SetPosition(position);
							entity->SetVelocity(Vector2(0, vel.y));

							// Create a debug collision visualization
							DebugCollisionInfo info;
							info.position = Vector2(
								(float)intersection.left + overlapWidth / 2.0f,
								(float)intersection.top + overlapHeight / 2.0f);
							info.normal = contactNormal;
							info.timeToLive = DEBUG_COLLISION_TTL;
							m_debugCollisions.push_back(info);
						}
						// If both entities are non-static, distribute the movement
						else if (!entity->IsStatic() && !other->IsStatic()) {
							// Move both entities equally in opposite directions
							Vector2 entityPos = entity->GetPosition();
							entityPos.x += contactNormal.x * (overlapWidth / 2.0f);
							entity->SetPosition(entityPos);
							entity->SetVelocity(Vector2(0, vel.y));

							Vector2 otherPos = other->GetPosition();
							otherPos.x -= contactNormal.x * (overlapWidth / 2.0f);
							other->SetPosition(otherPos);
							other->SetVelocity(Vector2(0, vel.y));

							// Create a debug collision visualization
							DebugCollisionInfo info;
							info.position = Vector2(
								(float)intersection.left + overlapWidth / 2.0f,
								(float)intersection.top + overlapHeight / 2.0f);
							info.normal = contactNormal;
							info.timeToLive = DEBUG_COLLISION_TTL;
							m_debugCollisions.push_back(info);
						}
					}
					else {
						// Resolve Y-axis overlap
						Vector2 contactNormal;

						// Determine push direction based on entity centers
						Vector2 entityCenter(
							(entityRect.left + entityRect.right) / 2.0f,
							(entityRect.top + entityRect.bottom) / 2.0f);

						Vector2 otherCenter(
							(otherRect.left + otherRect.right) / 2.0f,
							(otherRect.top + otherRect.bottom) / 2.0f);

						// Calculate normal direction from other entity to this entity
						if (entityCenter.y < otherCenter.y) {
							contactNormal = Vector2(0, -1); // Push up
						}
						else {
							contactNormal = Vector2(0, 1);  // Push down
						}

						// If other entity is static, move only the current entity
						if (other->IsStatic()) {
							// Move entity out of collision
							Vector2 position = entity->GetPosition();
							position.y += contactNormal.y * overlapHeight;
							entity->SetPosition(position);
							entity->SetVelocity(Vector2(vel.x, 0));


							// Create a debug collision visualization
							DebugCollisionInfo info;
							info.position = Vector2(
								(float)intersection.left + overlapWidth / 2.0f,
								(float)intersection.top + overlapHeight / 2.0f);
							info.normal = contactNormal;
							info.timeToLive = DEBUG_COLLISION_TTL;
							m_debugCollisions.push_back(info);
						}
						// If both entities are non-static, distribute the movement
						else if (!entity->IsStatic() && !other->IsStatic()) {
							// Move both entities equally in opposite directions
							Vector2 entityPos = entity->GetPosition();
							entityPos.y += contactNormal.y * (overlapHeight / 2.0f);
							entity->SetPosition(entityPos);
							entity->SetVelocity(Vector2(vel.x, 0));

							Vector2 otherPos = other->GetPosition();
							otherPos.y -= contactNormal.y * (overlapHeight / 2.0f);
							other->SetPosition(otherPos);
							other->SetVelocity(Vector2(vel.x, 0));

							// Create a debug collision visualization
							DebugCollisionInfo info;
							info.position = Vector2(
								(float)intersection.left + overlapWidth / 2.0f,
								(float)intersection.top + overlapHeight / 2.0f);
							info.normal = contactNormal;
							info.timeToLive = DEBUG_COLLISION_TTL;
							m_debugCollisions.push_back(info);
						}
					}
				}
			}
		}
	}
}

void Collision::UpdateDebugInfo(float dt)
{
	// Update the time-to-live for each debug collision and remove expired ones
	for (auto it = m_debugCollisions.begin(); it != m_debugCollisions.end();) {
		it->timeToLive -= dt;
		if (it->timeToLive <= 0) {
			it = m_debugCollisions.erase(it);
		}
		else {
			++it;
		}
	}
}

bool Collision::GroundCheck(const Entity* entity, float dt)
{
	Vector2 pos = entity->GetPosition();
	Vector2 size = entity->GetSize();
	Vector2 velocity = entity->GetVelocity();

	// Calculate how far the entity will move in the next frame
	float rayLength = std::abs(velocity.y * dt);

	// // Use a minimum distance to check even when velocity is small or zero
	float minRayLength = size.y / 2 + 1.0f;

	rayLength = std::max(rayLength, minRayLength);


	// Get cells around the entity
	std::vector<std::pair<int, int>> cells = GetEntityCells(entity, dt);

	for (const auto& cell : cells) {
		for (Entity* other : m_grid[cell.first][cell.second].entities) {
			// Skip if it's the same entity or not a static object (potential ground)
			if (other != entity && other->IsStatic() && other->IsActive() && other->IsCollidable()) {

				// Print the class name of the entity
				//Log(LOG_INFO, std::string(typeid(*other).name()));

				//std::string randomStr;
				//const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
				//int length = (std::rand() % 8) + 4; // Random length between 4-12
				//for (int i = 0; i < length; i++) {
				//	randomStr += chars[std::rand() % chars.length()];
				//}
				//Log(LOG_INFO, randomStr + " " + std::string(typeid(*other).name()));

				Rectangle otherRect = other->GetCollisionComponent()->GetRectangle();
				Vector2 rayStart = pos;
				Vector2 rayEnd = pos + Vector2(0, rayLength);
				Vector2 contactPoint, contactNormal;
				float contactTime;
				// only check if rayStart is above the other entity
				if (rayStart.y > otherRect.y) continue;

				// Cast a ray downward with our calculated check distance
				if (RayVsRect(rayStart, rayEnd, otherRect, contactPoint, contactNormal, contactTime)
					&& contactTime < 1.0f) {
					// We've hit ground
					return true;
				}
			}
		}
	}

	return false;
}

bool Collision::RayVsRect(const Vector2& origin, const Vector2& end, const Rectangle& rect, Vector2& contactPoint, Vector2& contactNormal, float& contactTime) {
	contactNormal = { 0, 0 };
	contactPoint = { 0, 0 };

	// Calculate ray direction
	Vector2 rayDir = end - origin;

	// Cache division
	Vector2 invdir = Vector2(
		rayDir.x != 0 ? 1.0f / rayDir.x : std::numeric_limits<float>::infinity(),
		rayDir.y != 0 ? 1.0f / rayDir.y : std::numeric_limits<float>::infinity()
	);

	// Calculate intersections with rectangle bounding axes
	Vector2 tNear = Vector2(
		(rect.x - origin.x) * invdir.x,
		(rect.y - origin.y) * invdir.y
	);
	Vector2 tFar = Vector2(
		(rect.x + rect.width - origin.x) * invdir.x,
		(rect.y + rect.height - origin.y) * invdir.y
	);

	if (std::isnan(tFar.y) || std::isnan(tFar.x)) return false;
	if (std::isnan(tNear.y) || std::isnan(tNear.x)) return false;

	// Sort distances
	if (tNear.x > tFar.x) std::swap(tNear.x, tFar.x);
	if (tNear.y > tFar.y) std::swap(tNear.y, tFar.y);

	// Early rejection
	if (tNear.x > tFar.y || tNear.y > tFar.x) return false;

	// Closest 'time' will be the first contact
	contactTime = std::max(tNear.x, tNear.y);

	// Furthest 'time' is contact on opposite side of target
	float tHitFar = std::min(tFar.x, tFar.y);

	// Reject if ray direction is pointing away from object
	if (tHitFar < 0)
		return false;

	// Contact point of collision from parametric line equation
	contactPoint = origin + rayDir * contactTime;

	if (tNear.x > tNear.y) {
		if (invdir.x < 0)
			contactNormal = { 1, 0 };
		else
			contactNormal = { -1, 0 };
	}
	else if (tNear.x < tNear.y) {
		if (invdir.y < 0)
			contactNormal = { 0, 1 };
		else
			contactNormal = { 0, -1 };
	}

	// If tNear == tFar, collision is principally in a diagonal
	// so pointless to resolve. By returning a CN={0,0} even though it's
	// considered a hit, the resolver won't change anything.
	return true;
}

bool Collision::RayEntVsEnt(const Entity& in, const Entity& target, Vector2& contactPoint, Vector2& contactNormal, float& contactTime, float dt, Axis axis) {
	// Check if dynamic entity is actually moving
	Vector2 inVel = in.GetVelocity();

	if (inVel.x == 0 && inVel.y == 0)
		return false;

	axis == Axis::X ? inVel.y = 0 : inVel.x = 0;

	Vector2 inPos = in.GetPosition();
	Vector2 inSize = in.GetSize();
	Vector2 tarPos = target.GetPosition();
	Vector2 tarSize = target.GetSize();

	// Expand target rectangle by source dimensions
	Rectangle expandedRect;
	expandedRect.x = (long)(tarPos.x - std::floor(inSize.x / 2) - std::floor(tarSize.x / 2));
	expandedRect.y = (long)(tarPos.y - std::floor(inSize.y / 2) - std::floor(tarSize.y / 2));
	expandedRect.width = (long)(tarSize.x + inSize.x);
	expandedRect.height = (long)(tarSize.y + inSize.y);

	Vector2 rayOrigin = inPos;
	Vector2 rayEnd = rayOrigin + inVel * dt;

	// Check if ray intersects with expanded rectangle
	if (RayVsRect(rayOrigin, rayEnd, expandedRect, contactPoint, contactNormal, contactTime))
		return (contactTime >= 0.0f && contactTime < 1.0f);
	else
		return false;
}

void Collision::RenderDebug(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* primitiveBatch) {
	if (!primitiveBatch)
		return;

	XMVECTORF32 color = Colors::Black;
	color.f[3] = 0.8f; // Alpha for transparency

	// Draw cells around all checked entities
	for (auto& pair : m_entityCells) {
		Entity* entity = pair.first;
		if (entity->IsActive() && entity->IsCollidable() && !entity->IsStatic()) {
			for (auto& cell : pair.second) {
				RECT cellRect{};
				cellRect.left = cell.first * m_cellSize;
				cellRect.top = cell.second * m_cellSize;
				cellRect.right = cellRect.left + m_cellSize;
				cellRect.bottom = cellRect.top + m_cellSize;
				DebugOverlay::DrawBoundingBox(primitiveBatch, cellRect, color);
			}
		}
	}

	// Draw cells around all entities
	//for (int x = 0; x < m_gridWidth; x++) {
	//  for (int y = 0; y < m_gridHeight; y++) {
	//    // Only draw cells that contain entities
	//    if (!m_grid[x][y].entities.empty()) {
	//      RECT cellRect{};
	//      cellRect.left = x * m_cellSize;
	//      cellRect.top = y * m_cellSize;
	//      cellRect.right = cellRect.left + m_cellSize;
	//      cellRect.bottom = cellRect.top + m_cellSize;
	//      DebugOverlay::DrawBoundingBox(primitiveBatch, cellRect, color);
	//    }
	//  }
	//}

	// Draw collision normals
	for (const auto& info : m_debugCollisions) {
		// Draw the collision normal as a line
		Vector2 normalEnd =
			info.position +
			info.normal * 10.0f; // Scale the normal for better visibility
		DebugOverlay::DrawLine(primitiveBatch, info.position, normalEnd,
			Colors::Red);

		// Draw a small dot at the collision point
		DebugOverlay::DrawQuad(primitiveBatch, info.position, Vector2(2.0f, 2.0f),
			Colors::Red);
	}
}

Collision* Collision::GetInstance() { return s_instance; }
