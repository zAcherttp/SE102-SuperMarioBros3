#pragma once
#include "pch.h"
#include "Entity.h"
#include "Collision.h"
#include "Debug.h"

using namespace DirectX::SimpleMath;

Entity::Entity(Vector2 position, SpriteSheet* spriteSheet) {
	m_collisionComponent = std::make_unique<CollisionComponent>(this);
	Log(LOG_INFO, "Creating Entity at position: " + std::to_string(position.x) + ", " + std::to_string(position.y));
	m_collisionComponent->SetPosition(position);
	m_animator = std::make_unique<Animator>();
	m_animator->SetSpriteSheet(spriteSheet);

}

Entity::Entity(DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 size, SpriteSheet* spriteSheet) {
	m_collisionComponent = std::make_unique<CollisionComponent>(this);
	m_collisionComponent->SetPosition(position);
	m_collisionComponent->SetSize(size);
	m_animator = std::make_unique<Animator>();
	m_animator->SetSpriteSheet(spriteSheet);
}

void Entity::Update(float deltaTime) {
	m_animator->Update(deltaTime, m_vel.x);
}

void Entity::Render(DirectX::SpriteBatch* spriteBatch) {
	if (m_visible) {
		// round the position to the nearest pixel
		Vector2 pos = m_collisionComponent->GetPosition();
		/*pos.x = static_cast<int>(pos.x + 0.5f);
		pos.y = static_cast<int>(pos.y + 0.5f);*/
		m_animator->Draw(spriteBatch, pos);
	}
}

Vector2 Entity::GetSize() const
{
	return m_collisionComponent->GetSize();
}

void Entity::SetSize(const Vector2& size)
{
	m_collisionComponent->SetSize(size);
}

Vector2 Entity::GetPosition() const
{
	Vector2 pos = m_collisionComponent->GetPosition();
	return pos;
}

void Entity::SetPosition(const Vector2& pos)
{
	m_collisionComponent->SetPosition(pos);
}

Vector2 Entity::GetVelocity() const
{
	return m_vel;
}

void Entity::SetVelocity(const Vector2& vel) 
{ 
	m_vel = vel;
}

Vector2 Entity::GetAcceleration() const {
	return m_accel;
}

void Entity::SetAcceleration(const Vector2& accel) {
	m_accel = accel;
}

bool Entity::IsActive() const {
	return m_isActive; 
}

bool Entity::IsCollidable() const {
	return m_isCollidable; 
}

bool Entity::IsStatic() const
{
	return m_isStatic;
}

// Animation control
void Entity::DefineAnimation(int animId, const std::vector<const wchar_t*>& frameNames, bool loop, float timePerFrame, bool useVelocity, float minTime, float maxTime, float velocityFactor) {
	m_animator->DefineAnimation(animId, frameNames, loop, timePerFrame,
		useVelocity, minTime, maxTime, velocityFactor);
}

void Entity::SetAnimation(int animId, bool reset) {
	Log(LOG_INFO, "Setting animation: " + std::to_string(animId) + " for entity: " + std::to_string(m_collisionComponent->GetPosition().x) + ", " + std::to_string(m_collisionComponent->GetPosition().y));
	m_animator->SetAnimation(animId, reset);

}

void Entity::SetDirection(int direction) {
	// Direction: -1 = left, 1 = right
	m_animator->SetFlipHorizontal(direction > 0);
}

int Entity::GetAnimId() const {
	return m_animId;
}

void Entity::SetAnimId(const int& id) { m_animId = id; }

Animator* Entity::GetAnimator() { return m_animator.get(); }

bool Entity::GetIsVisible() const { return m_visible; }

void Entity::SetIsVisible(const bool& isVis) { m_visible = isVis; }

bool Entity::GetIsActive() const
{
	return m_isActive;
}

void Entity::SetIsActive(const bool& IsActive)
{
	m_isActive = IsActive;
}

bool Entity::GetIsCollidable() const
{
	return m_isCollidable;
}

void Entity::SetIsCollidable(const bool& isCollidable)
{
	m_isCollidable = isCollidable;
}

// Get the collision component, use interaction points if entity defined it, otherwise default bbox
CollisionComponent* Entity::GetCollisionComponent() const
{
	return m_collisionComponent.get();
}

std::vector<std::pair<InteractionPointType, DirectX::SimpleMath::Vector2>> Entity::GetInteractionPoints() const
{
	return m_collisionComponent->GetInteractionPoints();
}

bool Entity::UsesInteractionPoints() const
{
	return false;
}

void Entity::OnCollision(const CollisionEvent& event) {
	if (UsesInteractionPoints()) {
		switch (event.pointType) {
		case InteractionPointType::TopHead:
			OnTopHeadCollision(event.collidedWith, event.normal);
			break;
		case InteractionPointType::LeftFoot:
		case InteractionPointType::RightFoot:
			OnFootCollision(event.collidedWith, event.normal);
			break;
		case InteractionPointType::LeftMiddle:
			OnLeftSideCollision(event.collidedWith, event.normal);
			break;
		case InteractionPointType::RightMiddle:
			OnRightSideCollision(event.collidedWith, event.normal);
			break;
		default:
			break;
		}
	}
}

void Entity::OnNoCollision() {
}

void Entity::OnTopHeadCollision(Entity* other, const Vector2& normal) {}
void Entity::OnFootCollision(Entity* other, const Vector2& normal) {
}
void Entity::OnLeftSideCollision(Entity* other, const Vector2& normal) {}
void Entity::OnRightSideCollision(Entity* other, const Vector2& normal) {}

bool Entity::IsGrounded() const {
	Vector2 pos = m_collisionComponent->GetPosition();
	Vector2 size = m_collisionComponent->GetSize();
	std::vector<Entity*> m_hitEntities;

	Collision::GetInstance()->Raycast(this, pos, pos + Vector2(0, size.y / 2 ), m_hitEntities);

	return !m_hitEntities.empty();
}
