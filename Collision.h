#pragma once
#include "pch.h"
#include "CollisionComponent.h"
#include <vector>
#include <unordered_map>

constexpr float DEBUG_COLLISION_TTL = 0.1f;
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
    float entryTime = 1.0f;
    float exitTime = 0.0f;
    Vector2 normal = Vector2(0, 0);
    Vector2 pos = Vector2(0, 0);
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

class Collision {
public:
    Collision(int worldWidth, int worldHeight, int cellSize = 16);
    ~Collision();

    Collision(const Collision&) = delete;

    Collision& operator=(const Collision&) = delete;

    void AddEntity(Entity* entity);
    void RemoveEntity(Entity* entity);
    void UpdateEntity(Entity* entity);

    // Clear all entities from the spatial grid
    void Clear();

    // Get potential collision candidates for an entity (broad phase)
    std::vector<Entity*> GetPotentialCollisions(Entity* entity);

    // Check for collision between two entities using Swept AABB (narrow phase)
    CollisionResult CheckCollision(Entity* movingEntity, Entity* staticEntity, float dt);

    // Process all collisions for a frame
    void ProcessCollisions(float dt);
    void UpdateDebugInfo(float dt);

    void Raycast(const Entity* entity, const Vector2& start, const Vector2& end, std::vector<Entity*>& hitEntities);
    bool GroundCheck(const Entity* entity, float dt = 0.0f);

    void RenderDebug(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* primitiveBatch);

    static Collision* GetInstance();
private:
    std::pair<int, int> GetCellCoords(const Vector2& position);
    std::vector<std::pair<int, int>> GetEntityCells(const Entity* entity);
    void SweptAABB(Entity* movingEntity, Entity* staticEntity, float dt, CollisionResult& result);
    void CheckInteractionPointCollision(Entity* entity, Entity* other, float dt, CollisionResult& result);
    void ResolveCollision(Entity* entity, const CollisionResult& result, float dt);
    bool RayVsRect(const Vector2& start, const Vector2& end, const RECT& rect, float& t_entry, float& t_exit);

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