#pragma once
#include "pch.h"
#include <vector>
#include <map>
#include <unordered_set>

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

    RECT GetBoundingBox() const;

    DirectX::SimpleMath::Vector2 GetSize() const;
	void SetSize(DirectX::SimpleMath::Vector2 size);

	DirectX::SimpleMath::Vector2 GetPosition() const;
    void SetPosition(DirectX::SimpleMath::Vector2 position);

    // Check if a point is inside the bounding box
    bool ContainsPoint(const DirectX::SimpleMath::Vector2& point) const;
	bool ContainsRectangle(const RECT& rect) const;

    // Get the owner entity
    Entity* GetOwner() const { return m_owner; }

    virtual std::vector<std::pair<InteractionPointType, DirectX::SimpleMath::Vector2>> GetInteractionPoints() const;
    
    // Debug rendering
    void RenderDebug(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* primitiveBatch,
        DirectX::FXMVECTOR boundingBoxColor = DirectX::Colors::Yellow,
        DirectX::FXMVECTOR pointsColor = DirectX::Colors::Red) const;

private:
    Entity* m_owner;
    DirectX::SimpleMath::Vector2 m_size;
    DirectX::SimpleMath::Vector2 m_pos;
};