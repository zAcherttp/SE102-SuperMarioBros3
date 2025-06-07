#include "pch.h"
#include "CollisionComponent.h"
#include "Debug.h"
#include "DebugOverlay.h"
#include "Entity.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

CollisionComponent::CollisionComponent(Entity* owner)
	: m_owner(owner),
	m_size(0, 0),
	m_pos(0, 0),
	m_pushVector((0.f, 0.f)),
	m_isBeingPushed(false),
	m_pushVel((0.f, 0.f)),
	m_pushedDistance(0)
{
	m_platform = nullptr;
}

RECT CollisionComponent::GetRect() const
{
	RECT rect{};
	rect.left = static_cast<LONG>(m_pos.x - m_size.x / 2.f);
	rect.top = static_cast<LONG>(m_pos.y - m_size.y / 2.f);
	rect.right = static_cast<LONG>(m_pos.x + m_size.x / 2.f);
	rect.bottom = static_cast<LONG>(m_pos.y + m_size.y / 2.f);

	return rect;
}

Rectangle CollisionComponent::GetRectangle() const
{
	return Rectangle(GetRect());
}

Vector2 CollisionComponent::GetSize() const
{
	return m_size;
}

void CollisionComponent::SetSize(const DirectX::SimpleMath::Vector2& size)
{
	m_size = size;
}

Vector2 CollisionComponent::GetPosition() const
{
	return m_pos;
}

void CollisionComponent::SetPosition(const DirectX::SimpleMath::Vector2& position)
{
	m_pos = position;
}

Vector2 CollisionComponent::GetVelocity() const
{
	return m_vel;
}

void CollisionComponent::SetVelocity(const DirectX::SimpleMath::Vector2& velocity)
{
	m_vel = velocity;
}

void CollisionComponent::SetPlatform(Entity* ent)
{
	if (ent) {
		m_platform = ent->GetCollisionComponent();
		return;
	}
	m_platform = nullptr;
}

void CollisionComponent::Update(float dt) {
	// move along platform
	if (m_platform) {
		m_pos += m_platform->GetVelocity() * dt;
	}

	// consume push
	if (m_isBeingPushed) {
		Vector2 delta = m_pushVel * dt;
		m_pos += delta;
		m_pushedDistance += delta.Length();

		// Log(LOG_INFO, "Pushing entity " + std::to_string(m_owner->GetAnimId()) + " with distance: " +
		//     std::to_string(m_pushVector.x) + ", " + std::to_string(m_pushVector.y) +
		//     " over time: " + std::to_string(m_pushTime) + " seconds.");
		if (m_pushVector.Length() - m_pushedDistance <= 0) {
			m_isBeingPushed = false;
			m_pushVector = Vector2(0, 0);
			m_pushVel = Vector2(0, 0);
			m_pushedDistance = 0;
		}
	}
}

void CollisionComponent::Push(Vector2 distance, float span)
{
	if (distance.Length() == 0 || span < 0) return;

	m_pushVector = distance;
	m_pushedDistance = 0;
	m_pushVel = Vector2(m_pushVector.x / span, m_pushVector.y / span);
	m_isBeingPushed = true;
	/*Log(LOG_INFO, "Pushing entity " + std::to_string(m_owner->GetAnimId()) + " with distance: " +
			 std::to_string(m_pushVector.x) + ", " + std::to_string(m_pushVector.y) +
			 " over time:");*/
}

bool CollisionComponent::GetIsPushed() const { return m_isBeingPushed; }

std::vector<std::pair<InteractionPointType, Vector2>> CollisionComponent::GetInteractionPoints() const
{
	//return default empty vector if entity didnt define any interaction points
	return std::vector<std::pair<InteractionPointType, Vector2>>{};
}

void CollisionComponent::RenderDebug(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* primitiveBatch,
	DirectX::FXMVECTOR boundingBoxColor,
	DirectX::FXMVECTOR pointsColor) const
{
	if (!primitiveBatch) return;

	// Draw bounding box
	DebugOverlay::DrawBoundingBox(primitiveBatch, GetRect(), boundingBoxColor);

	Vector2 pos = GetPosition();
	Vector2 size = GetSize();

	DebugOverlay::DrawLine(primitiveBatch, Vector2(pos.x, pos.y - size.y / 2.f), Vector2(pos.x + size.x, pos.y - size.y / 2.f), Colors::Cyan);

	// Draw interaction points
	for (const auto& [type, pointPos] : m_owner->GetInteractionPoints()) {
		Vector2 worldPos = pointPos + m_owner->GetPosition();
		DebugOverlay::DrawQuad(primitiveBatch, worldPos, Vector2(1, 1), pointsColor);
	}
}