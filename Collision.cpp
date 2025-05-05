#include "pch.h"
#include "Collision.h"
#include "CollisionComponent.h"
#include "Mario.h"
#include "Debug.h"
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

std::vector<std::pair<int, int>> Collision::GetEntityCells(const Entity* entity)
{
    std::vector<std::pair<int, int>> cells;

    // Get entity's bounding box
    RECT bbox = entity->GetCollisionComponent()->GetBoundingBox();

    // Get cells for all corners of the bounding box
    std::pair<int, int> topLeft = GetCellCoords(Vector2(static_cast<float>(bbox.left), static_cast<float>(bbox.top)));
    std::pair<int, int> bottomRight = GetCellCoords(Vector2(static_cast<float>(bbox.right), static_cast<float>(bbox.bottom)));

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

void Collision::AddEntity(Entity* entity)
{
    if (!entity) return;

    // Get all cells the entity occupies
    std::vector<std::pair<int, int>> cells = GetEntityCells(entity);

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

void Collision::UpdateEntity(Entity* entity)
{
    // Remove entity from current cells
    RemoveEntity(entity);

    // Add entity to new cells based on updated position
    AddEntity(entity);
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

CollisionEvent Collision::SweptAABB(Entity* movingEntity, Entity* staticEntity, float dt)
{
    CollisionEvent result;

    // Get bounding boxes
    RECT movingBox = movingEntity->GetCollisionComponent()->GetBoundingBox();
    RECT staticBox = staticEntity->GetCollisionComponent()->GetBoundingBox();

    // Get entity velocities
    Vector2 velocity = movingEntity->GetVelocity() * dt;
    Vector2 relativeVelocity = velocity;

    // If the second entity is also moving, use relative velocity
    if (!staticEntity->IsStatic()) {
        relativeVelocity = velocity - staticEntity->GetVelocity() * dt;
    }

    // If not moving, no collision will occur
    if (relativeVelocity.x == 0 && relativeVelocity.y == 0) {
        return result;
    }

    // Calculate entry and exit times for x and y
    float entryTimeX, entryTimeY;
    float exitTimeX, exitTimeY;

    // Calculate entry and exit times for X axis
    if (relativeVelocity.x > 0) {
        entryTimeX = (staticBox.left - movingBox.right) / relativeVelocity.x;
        exitTimeX = (staticBox.right - movingBox.left) / relativeVelocity.x;
    }
    else if (relativeVelocity.x < 0) {
        entryTimeX = (staticBox.right - movingBox.left) / relativeVelocity.x;
        exitTimeX = (staticBox.left - movingBox.right) / relativeVelocity.x;
    }
    else {
        entryTimeX = -std::numeric_limits<float>::infinity();
        exitTimeX = std::numeric_limits<float>::infinity();
    }

    // Calculate entry and exit times for Y axis
    if (relativeVelocity.y > 0) {
        entryTimeY = (staticBox.top - movingBox.bottom) / relativeVelocity.y;
        exitTimeY = (staticBox.bottom - movingBox.top) / relativeVelocity.y;
    }
    else if (relativeVelocity.y < 0) {
        entryTimeY = (staticBox.bottom - movingBox.top) / relativeVelocity.y;
        exitTimeY = (staticBox.top - movingBox.bottom) / relativeVelocity.y;
    }
    else {
        entryTimeY = -std::numeric_limits<float>::infinity();
        exitTimeY = std::numeric_limits<float>::infinity();
    }

    // Find the latest entry time and earliest exit time
    float entryTime = std::max(entryTimeX, entryTimeY);
    float exitTime = std::min(exitTimeX, exitTimeY);

    // Check if collision occurs within this frame
    if (entryTime > exitTime || (entryTimeX < 0.0f && entryTimeY < 0.0f) || entryTime > 1.0f) {
        return result; // No collision
    }

    // Collision detected
    result.collided = true;
    result.entryTime = entryTime;
    result.exitTime = exitTime;
    result.collidedWith = staticEntity;

    // Calculate collision normal
    if (entryTimeX > entryTimeY) {
        result.normal = Vector2(relativeVelocity.x < 0 ? 1.0f : -1.0f, 0.0f);
    }
    else {
        result.normal = Vector2(0.0f, relativeVelocity.y < 0 ? 1.0f : -1.0f);
    }

    return result;
}

bool Collision::CheckInteractionPointCollision(Entity* entity, Entity* other, float dt, CollisionEvent& result)
{
    // Check if entity implements interaction points
    auto points = entity->GetInteractionPoints();
    if (points.empty()) {
        return false; // No interaction points, use default AABB
    }

    // Get other entity's bounding box
    RECT otherBox = other->GetCollisionComponent()->GetBoundingBox();

    // Get entity velocity
    Vector2 velocity = entity->GetVelocity() * dt;

    // Check each interaction point
    bool collision = false;
    float earliestEntryTime = 1.0f;

    for (const auto& [type, localPoint] : points) {
        // Convert to world space
        Vector2 worldPoint = localPoint + entity->GetPosition();
        Vector2 nextPoint = worldPoint + velocity;

        // Check if point is inside other entity's bounding box
        if (nextPoint.x >= otherBox.left && nextPoint.x <= otherBox.right &&
            nextPoint.y >= otherBox.top && nextPoint.y <= otherBox.bottom) {

            // Calculate entry time for this point
            float entryTimeX, entryTimeY;

            if (velocity.x > 0) {
                entryTimeX = (otherBox.left - worldPoint.x) / velocity.x;
            }
            else if (velocity.x < 0) {
                entryTimeX = (otherBox.right - worldPoint.x) / velocity.x;
            }
            else {
                entryTimeX = -std::numeric_limits<float>::infinity();
            }

            if (velocity.y > 0) {
                entryTimeY = (otherBox.top - worldPoint.y) / velocity.y;
            }
            else if (velocity.y < 0) {
                entryTimeY = (otherBox.bottom - worldPoint.y) / velocity.y;
            }
            else {
                entryTimeY = -std::numeric_limits<float>::infinity();
            }

            float entryTime = std::max(entryTimeX, entryTimeY);

            if (entryTime < earliestEntryTime && entryTime >= 0 && entryTime <= 1.0f) {
                collision = true;
                earliestEntryTime = entryTime;

                // Determine collision normal based on interaction point type
                switch (type) {
                case InteractionPointType::TopHead:
                    result.normal = Vector2(0, 1); // Hitting head on ceiling
                    result.pointType = InteractionPointType::TopHead;
                    break;
                case InteractionPointType::LeftFoot:
                    result.normal = Vector2(0, -1); // Standing on ground
                    result.pointType = InteractionPointType::LeftFoot;  
                    break;
                case InteractionPointType::RightFoot:
                    result.normal = Vector2(0, -1); // Standing on ground
                    result.pointType = InteractionPointType::RightFoot;
                    break;
                case InteractionPointType::LeftMiddle:
                    result.normal = Vector2(1, 0); // Hit wall on left
                    result.pointType = InteractionPointType::LeftMiddle;
                    break;
                case InteractionPointType::RightMiddle:
                    result.normal = Vector2(-1, 0); // Hit wall on right
                    result.pointType = InteractionPointType::RightMiddle;
                    break;
                default:
                    result.normal = entryTimeX > entryTimeY ?
                        Vector2(velocity.x < 0 ? 1.0f : -1.0f, 0.0f) :
                        Vector2(0.0f, velocity.y < 0 ? 1.0f : -1.0f);
                    break;
                }
            }
        }
    }

    if (collision) {
        result.collided = true;
        result.entryTime = earliestEntryTime;
        result.collidedWith = other;
    }

    return collision;
}

CollisionEvent Collision::CheckCollision(Entity* movingEntity, Entity* staticEntity, float dt)
{
    // Try interaction point collision first if entity is flagged for it
    CollisionEvent result;
    if (movingEntity->UsesInteractionPoints()) {
        //Log(LOG_INFO, "Checking interaction point collision for entity: " + std::to_string(movingEntity->GetAnimId()));
        if (CheckInteractionPointCollision(movingEntity, staticEntity, dt, result)) {
            return result;
        }
    }

    // Fall back to Swept AABB collision
    return SweptAABB(movingEntity, staticEntity, dt);
}

void Collision::ResolveCollision(Entity* entity, const CollisionEvent& result, float dt)
{
    if (!result.collided) return;

    // Calculate remaining movement after collision
    float remainingTime = 1.0f - result.entryTime;

    // Get velocity and current position
    Vector2 velocity = entity->GetVelocity();
    Vector2 position = entity->GetPosition() + velocity * dt * result.entryTime;

    // Add collision to debug visualization
    DebugCollisionInfo info;
    info.position = position;
    info.normal = result.normal;
    info.timeToLive = DEBUG_COLLISION_TTL;
    m_debugCollisions.push_back(info);

    // Handle velocity changes based on collision normal
    if (std::abs(result.normal.x) > 0.0f) {
        // Horizontal collision, zero out x velocity
        velocity.x = 0;
    }

    if (std::abs(result.normal.y) > 0.0f) {
        // Vertical collision, zero out y velocity
        velocity.y = 0;
    }

    // Apply updated velocity and position
    entity->SetVelocity(velocity);
    entity->SetPosition(position);

    // Log result point type
    // Notify the entity about the collision
    entity->OnCollision(result);
    // Notify the other entity (if not static)
    if (!result.collidedWith->IsStatic()) {
        CollisionEvent otherEvent = result;
        otherEvent.collidedWith = entity;
        otherEvent.normal = -result.normal; // Reverse normal for the other entity
        result.collidedWith->OnCollision(otherEvent);
    }
}

void Collision::ProcessCollisions(float dt)
{
    UpdateDebugInfo(dt);

    // Process movement-based collisions for all entities
    for (auto& pair : m_entityCells) {
        Entity* entity = pair.first;

        // Skip static and inactive entities
        if (entity->IsStatic() || !entity->IsActive() || !entity->IsCollidable()) {
            continue;
        }

        // Get potential collision candidates
        std::vector<Entity*> potentialCollisions = GetPotentialCollisions(entity);

        // Track if any collision occurred
        bool hasCollided = false;

        // Find earliest collision
        CollisionEvent earliestCollision;
        earliestCollision.entryTime = 1.0f;

        for (Entity* other : potentialCollisions) {
            // Skip inactive entities
            if (!other->IsActive() || !other->IsCollidable()) {
                continue;
            }

            // Check collision
            CollisionEvent result = CheckCollision(entity, other, dt);

            // Keep track of the earliest collision
            if (result.collided && result.entryTime < earliestCollision.entryTime) {
                earliestCollision = result;
                hasCollided = true;
            }
        }

        // Resolve the earliest collision
        if (hasCollided) {
            ResolveCollision(entity, earliestCollision, dt);
        }
        else {
            entity->OnNoCollision();
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

void Collision::Raycast(const Entity *entity,
                        const Vector2 &start,
                        const Vector2 &end,
                        std::vector<Entity *> &hitEntities)
{
    hitEntities.clear();

    std::vector<std::pair<int, int>> cells = GetEntityCells(entity);
    for (const auto& cell : cells) {
        for (Entity* other : m_grid[cell.first][cell.second].entities) {
            if (other != entity) {
                RECT otherBox = other->GetCollisionComponent()->GetBoundingBox();
                float t_entry, t_exit;
                if (RayVsRect(start, end, otherBox, t_entry, t_exit) && t_entry < 1.0f ) {
                    if(!other->IsCollidable())
                    continue;
                    hitEntities.push_back(other);
                }
            }
        }
    }
}

bool Collision::RayVsRect(const Vector2 &start, const Vector2 &end, const RECT &rect, float& t_entry, float& t_exit) {
    Vector2 t_near = (Vector2(rect.left, rect.top) - start) / (end - start);
    Vector2 t_far = (Vector2(rect.right, rect.bottom) - start) / (end - start);

    if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
    if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

    if(t_near.x > t_far.y || t_near.y > t_far.x) {
        return false;
    }

    t_entry = std::max(t_near.x, t_near.y);
    t_exit = std::min(t_far.x, t_far.y);

    if (t_entry > t_exit || t_exit < 0.0f) {
        return false;
    }

    return true;
}

void Collision::RenderDebug(
    DirectX::PrimitiveBatch<DirectX::VertexPositionColor> *primitiveBatch) {
  if (!primitiveBatch)
    return;

  // Draw the spatial grid
  for (int x = 0; x < m_gridWidth; x++) {
    for (int y = 0; y < m_gridHeight; y++) {
      // Only draw cells that contain entities
      if (!m_grid[x][y].entities.empty()) {
        RECT cellRect{};
        cellRect.left = x * m_cellSize;
        cellRect.top = y * m_cellSize;
        cellRect.right = cellRect.left + m_cellSize;
        cellRect.bottom = cellRect.top + m_cellSize;

        // Color based on entity count
        XMVECTORF32 color = Colors::Yellow;
        color.f[3] = 0.3f; // Alpha for transparency

        DebugOverlay::DrawBoundingBox(primitiveBatch, cellRect, color);
      }
    }
  }

  // Draw collision normals
  for (const auto &info : m_debugCollisions) {
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
