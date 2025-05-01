#include "pch.h"
#include "CollisionComponent.h"
#include "Entity.h"
#include "DebugOverlay.h"
#include "Debug.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

CollisionComponent::CollisionComponent(Entity* owner)
	: m_owner(owner), m_size(0, 0), m_pos(0, 0)
{
}

RECT CollisionComponent::GetBoundingBox() const
{
    RECT rect{};    
    rect.left = static_cast<LONG>(m_pos.x - m_size.x / 2.f);
    rect.top = static_cast<LONG>(m_pos.y - m_size.y / 2.f);
    rect.right = static_cast<LONG>(m_pos.x + m_size.x / 2.f);
    rect.bottom = static_cast<LONG>(m_pos.y + m_size.y / 2.f);

    return rect;
}

Vector2 CollisionComponent::GetSize() const
{
    return m_size;
}

void CollisionComponent::SetSize(DirectX::SimpleMath::Vector2 size)
{
    m_size = size;
}

Vector2 CollisionComponent::GetPosition() const
{
    return m_pos;
}

void CollisionComponent::SetPosition(DirectX::SimpleMath::Vector2 position)
{
    m_pos = position;
}

bool CollisionComponent::ContainsPoint(const Vector2& point) const
{
    RECT rect = GetBoundingBox();
	return (point.x >= rect.left && point.x <= rect.right &&
        point.y >= rect.top && point.y <= rect.bottom);
}

bool CollisionComponent::ContainsRectangle(const RECT& rect) const
{
    RECT thisRect = GetBoundingBox();
    return (rect.left >= thisRect.left && rect.right <= thisRect.right &&
        rect.top >= thisRect.top && rect.bottom <= thisRect.bottom);
}

std::vector<std::pair<InteractionPointType, Vector2>> CollisionComponent::GetInteractionPoints() const
{
    //return default top left right and two feets if entity implement this function
    std::vector<std::pair<InteractionPointType, Vector2>> points;

    points.emplace_back(InteractionPointType::TopHead, Vector2(0, -m_size.y / 2.f));
    points.emplace_back(InteractionPointType::LeftMiddle, Vector2(-m_size.x / 2.f, 0));
    points.emplace_back(InteractionPointType::RightMiddle, Vector2(m_size.x / 2.f, 0));
    points.emplace_back(InteractionPointType::LeftFoot, Vector2(-m_size.x / 2.f, m_size.y / 2.f));
    points.emplace_back(InteractionPointType::RightFoot, Vector2(m_size.x / 2.f, m_size.y / 2.f));

    return points;
}

void CollisionComponent::RenderDebug(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* primitiveBatch,
    DirectX::FXMVECTOR boundingBoxColor,
    DirectX::FXMVECTOR pointsColor) const
{
    if (!primitiveBatch) return;

    // Draw bounding box
    DebugOverlay::DrawBoundingBox(primitiveBatch, GetBoundingBox(), boundingBoxColor);

    // Draw interaction points
    for (const auto& [type, pointPos] : m_owner->GetInteractionPoints()) {
        Vector2 worldPos = pointPos + m_owner->GetPosition();
        DebugOverlay::DrawQuad(primitiveBatch, worldPos, Vector2(1, 1), pointsColor);
    }
}