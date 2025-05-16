#pragma once
#include "pch.h"
#include <vector>
#include <map>
#include <unordered_set>

using namespace DirectX::SimpleMath;

enum class InteractionPointType {
    TopHead,

    LeftUpper,
    LeftLower,

    RightUpper,
    RightLower,

    LeftFoot,
    RightFoot,

    None,
};

class Entity;

class CollisionComponent {
public:
    CollisionComponent(Entity* owner);
    ~CollisionComponent() = default;

    RECT GetRect() const;
    Rectangle GetRectangle() const;

    void Update(float dt);

    Vector2 GetSize() const;
	void SetSize(Vector2 size);

	Vector2 GetPosition() const;
    void SetPosition(Vector2 position);

	Vector2 GetVelocity() const;
    void SetVelocity(Vector2 velocity);

    //push entity over timespan
    void Push(Vector2 distance, float span);
    bool GetIsPushed() const;

    // Get the owner entity
    Entity* GetOwner() const { return m_owner; }

    virtual std::vector<std::pair<InteractionPointType, Vector2>> GetInteractionPoints() const;
    
    // Debug rendering
    void RenderDebug(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* primitiveBatch,
        DirectX::FXMVECTOR boundingBoxColor = DirectX::Colors::Yellow,
        DirectX::FXMVECTOR pointsColor = DirectX::Colors::Red) const;

private:
    Entity* m_owner;
    Vector2 m_size;
    Vector2 m_pos;
    Vector2 m_vel;

    bool m_isBeingPushed;
    Vector2 m_pushVector;
    Vector2 m_pushVel;
    float m_pushedDistance;
};