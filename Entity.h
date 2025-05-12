#pragma once
#include "State.h"
#include "Animator.h"
#include "Collision.h"
#include "CollisionComponent.h"
#include "Game.h"

using namespace DirectX::SimpleMath;

class Entity
{
public:
	Entity(Vector2 pos, SpriteSheet* spriteSheet);
	Entity(Vector2 pos, Vector2 size, SpriteSheet* spriteSheet);
	
	virtual ~Entity() = default;

	virtual void Update(float dt) = 0;
	virtual void Render(DirectX::SpriteBatch* spriteBatch) = 0;

	Vector2 GetSize() const;
	void SetSize(const Vector2& size);

	Vector2 GetPosition() const;
	void SetPosition(const Vector2& pos);

	Vector2 GetVelocity() const;
	void SetVelocity(const Vector2& vel);

	bool IsActive() const;
	bool IsCollidable() const;
	bool IsStatic() const;
	bool IsDead() const;
	bool IsNotContactWithSolidBlocks() const;

	void DefineAnimation(int animId, const std::vector<const wchar_t*>& frameNames,
		bool loop = true, float timePerFrame = 0.1f,
		bool useVelocity = false, float minTime = 0.05f,
		float maxTime = 0.2f, float velocityFactor = 1.0f);

	void SetAnimation(int animId, bool reset = false);

	void SetDirection(int direction);

	int GetAnimId() const;
	void SetAnimId(const int& id);

	Animator* GetAnimator();

	bool GetIsVisible() const;
	void SetIsVisible(const bool& isVis);

	bool GetIsActive() const;
	void SetIsActive(const bool& IsActive);

	bool GetIsCollidable() const;
	void SetIsCollidable(const bool& isCollidable);

	// Get the collision component, use interaction points if entity defined it, otherwise default bbox
	CollisionComponent* GetCollisionComponent() const;
	virtual std::vector<std::pair<InteractionPointType, Vector2>> GetInteractionPoints() const;
	virtual bool UsesInteractionPoints() const;

	virtual void OnCollision(const CollisionResult& event);
	virtual void OnNoCollision(float dt, Axis axis);

	virtual void OnTopHeadCollision(const CollisionResult& event);
	virtual void OnFootCollision(const CollisionResult& event);
	virtual void OnLeftSideCollision(const CollisionResult& event);
	virtual void OnRightSideCollision(const CollisionResult& event);
	virtual bool IsGrounded() const;

	virtual void Die();

protected:
	//physics
	std::unique_ptr<CollisionComponent> m_collisionComponent;
	bool m_isActive = true;
	bool m_isCollidable = true;
	bool m_isStatic = false;
	bool m_isGrounded = false;
	bool m_doesNotContactWithSolidBlocks = false;

	//sprite
	std::unique_ptr<Animator> m_animator;
	int m_animId = 0;
	bool m_visible = true;
};

