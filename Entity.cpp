#pragma once
#include "pch.h"
#include "Collision.h"
#include "Debug.h"
#include "Entity.h"

using namespace DirectX::SimpleMath;

Entity::Entity(Vector2 position, SpriteSheet* spriteSheet) {
	m_collisionComponent = std::make_unique<CollisionComponent>(this);
	//Log(LOG_INFO, "Creating Entity at position: " + std::to_string(position.x) + ", " + std::to_string(position.y));
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
	m_animator->Update(deltaTime, m_collisionComponent->GetVelocity().x);
}

void Entity::Render(DirectX::SpriteBatch* spriteBatch) {	
	if (m_visible) {
		// Get the position
		Vector2 pos = m_collisionComponent->GetPosition();
		pos.x = (float)(int)(pos.x + 0.5f);
		pos.y = (float)(int)(pos.y + 0.5f);
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
	return m_collisionComponent->GetPosition();
}

void Entity::SetPosition(const Vector2& pos)
{
	m_collisionComponent->SetPosition(pos);
}

Vector2 Entity::GetVelocity() const
{
	return m_collisionComponent->GetVelocity();
}

void Entity::SetVelocity(const Vector2& vel)
{
	m_collisionComponent->SetVelocity(vel);
}

//Vector2 Entity::GetRelativeVelocity() const
//{
//	return m_collisionComponent->GetRelativeVelocity();
//}
//
//void Entity::SetRelativeVelocity(const Vector2& vel)
//{
//	m_collisionComponent->SetRelativeVelocity(vel);
//}

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
	//Log(LOG_INFO, "Setting animation: " + std::to_string(animId) + " for entity: " + std::to_string(m_collisionComponent->GetPosition().x) + ", " + std::to_string(m_collisionComponent->GetPosition().y));
	m_animId = animId;
	m_animator->SetAnimation(animId, reset);
}

void Entity::SetDirection(int direction) {
	// Direction: -1 = left, 1 = right
	m_animator->SetFlipHorizontal(direction > 0);
}

CollisionGroup Entity::GetCollisionGroup() const
{
	return m_collisionGroup;
}

void Entity::SetCollisionGroup(const CollisionGroup& group)
{
	m_collisionGroup = group;
}

int Entity::GetAnimId() const {
	return m_animId;
}

void Entity::SetAnimId(const int& id) {
	m_animId = id;
	m_animator->SetAnimation(id, true);
}

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

void Entity::OnCollision(const CollisionResult& result) {
	result;
}

void Entity::OnNoCollision(float dt, Axis axis) {
	dt;
	axis;
}

void Entity::OnTopHeadCollision(const CollisionResult& event) {
	event;
}

void Entity::OnFootCollision(const CollisionResult& event) {
	event;
}

void Entity::OnLeftSideCollision(const CollisionResult& event) {
	event;
}

void Entity::OnRightSideCollision(const CollisionResult& event) {
	event;
}

bool Entity::IsGrounded() const {
	return m_isGrounded;
}

void Entity::Die(DyingType type) {
	type;
}
