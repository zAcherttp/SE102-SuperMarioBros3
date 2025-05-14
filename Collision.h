#pragma once
#include "pch.h"
#include "CollisionComponent.h"
#include <vector>
#include <unordered_map>

constexpr float DEBUG_COLLISION_TTL = 2.f;
using namespace DirectX;
using namespace DirectX::SimpleMath;

class Entity;

struct SpatialGridCell {
    std::vector<Entity*> entities;
};

struct DebugCollisionInfo {
    Vector2 position;
    Vector2 normal;
    float timeToLive;
};

struct CollisionResult {
    bool collided = false;
    float contactTime = 1.0f;
    Vector2 contactNormal = Vector2(0, 0);
    Vector2 contactPoint = Vector2(0, 0);
    Entity* collidedWith = nullptr;
    InteractionPointType pointType = InteractionPointType::None;
};

struct RaycastHit {
    Entity* entity = nullptr;
    float distance = 0.0f;
    Vector2 point = Vector2(0, 0);
    Vector2 normal = Vector2(0, 0);
};

struct RaycastResult {
    bool hasHit = false;
    RaycastHit closestHit;
    std::vector<RaycastHit> hits;
};

enum class Axis { X, Y };

class Collision {
public:
    Collision(int worldWidth, int worldHeight, int cellSize = 313 );
    ~Collision();

    Collision(const Collision&) = delete;

    Collision& operator=(const Collision&) = delete;

    void AddEntity(Entity* entity, float dt);
    void RemoveEntity(Entity* entity);
    void UpdateEntity(Entity* entity, float dt);

    // Clear all entities from the spatial grid
    void Clear();

    // Get potential collision candidates for an entity (broad phase)
    std::vector<Entity*> GetPotentialCollisions(Entity* entity);

    // Check for collision between two entities using Swept AABB (narrow phase)
    CollisionResult CheckCollision(Entity* movingEntity, Entity* staticEntity, float dt, Axis axis);

    // Process all collisions for a frame
    void ProcessCollisions(float dt);
    void UpdateDebugInfo(float dt);    bool GroundCheck(const Entity* entity, float dt = 0.0f);

    void RenderDebug(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* primitiveBatch);

    static Collision* GetInstance();
    std::vector<std::pair<int, int>> GetEntityCells(const Entity* entity, float dt);
    bool RayVsRect(const Vector2& origin, const Vector2& end, const Rectangle& rect, Vector2& contactPoint, Vector2& contactNormal, float& contactTime);

    // Get access to the spatial grid
    const std::vector<std::vector<SpatialGridCell>>& GetGrid() const { return m_grid; }
private:
    std::pair<int, int> GetCellCoords(const Vector2& position);
    void SweptAABB(Entity* movingEntity, Entity* staticEntity, float dt, CollisionResult& result, Axis axis);
    void CheckInteractionPointCollision(Entity* entity, Entity* other, float dt, CollisionResult& result, Axis axis);
    void ResolveCollision(Entity* entity, const CollisionResult& result, float dt, Axis axis);
    bool RayEntVsEnt(const Entity& in, const Entity& target, Vector2& contactPoint, Vector2& contactNormal, float& contactTime, float dt, Axis axis);
    bool SweptEntVsEnt(const Entity& in, const Entity& target, Vector2& contactPoint, Vector2& contactNormal, float& contactTime, float dt);

private:
    static Collision* s_instance;

    int m_worldWidth;
    int m_worldHeight;
    int m_cellSize;
    int m_gridWidth;
    int m_gridHeight;

    // 2D grid of cells for spatial partitioning
    std::vector<std::vector<SpatialGridCell>> m_grid;

    // Map to keep track of which cells an entity is in
    std::unordered_map<Entity*, std::vector<std::pair<int, int>>> m_entityCells;

    std::vector<DebugCollisionInfo> m_debugCollisions;
};