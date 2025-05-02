#pragma once
#include "pch.h"
#include "CollisionComponent.h"
#include <vector>
#include <unordered_map>

class Entity;

struct SpatialGridCell {
    std::vector<Entity*> entities;
};

struct DebugCollisionInfo {
    DirectX::SimpleMath::Vector2 position;
    DirectX::SimpleMath::Vector2 normal;
    float timeToLive;
};

struct CollisionEvent {
    bool collided;
    float entryTime;
    float exitTime;
    DirectX::SimpleMath::Vector2 normal;
    Entity* collidedWith;
    InteractionPointType pointType;

    CollisionEvent()
        : collided(false),
        entryTime(1.0f),
        exitTime(0.0f),
        normal(0, 0),
        collidedWith(nullptr),
        pointType(InteractionPointType::TopHead)
    {}
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
    CollisionEvent CheckCollision(Entity* movingEntity, Entity* staticEntity, float dt);

    // Check for collision using interaction points
    bool CheckInteractionPointCollision(Entity* entity, Entity* other, float dt, CollisionEvent& result);

    // Process all collisions for a frame
    void ProcessCollisions(float dt);
    void UpdateDebugInfo(float dt);

    void Raycast(const Entity* entity,
        const DirectX::SimpleMath::Vector2& start,
        const DirectX::SimpleMath::Vector2& end,
        std::vector<Entity*>& hitEntities);

    bool RayVsRect(const DirectX::SimpleMath::Vector2& start,
        const DirectX::SimpleMath::Vector2& end,
        const RECT& rect, float& t_entry, float& t_exit);

    void RenderDebug(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* primitiveBatch);

    static Collision* GetInstance();
private:
    std::pair<int, int> GetCellCoords(const DirectX::SimpleMath::Vector2& position);

    std::vector<std::pair<int, int>> GetEntityCells(const Entity* entity);

    CollisionEvent SweptAABB(Entity* movingEntity, Entity* staticEntity, float dt);

    void ResolveCollision(Entity* entity, const CollisionEvent& result, float dt);

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
    const float DEBUG_COLLISION_TTL = 0.1f;
};