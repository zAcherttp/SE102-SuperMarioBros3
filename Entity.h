#pragma once
#include "State.h"
#include "Animator.h"
#include "CollisionEvent.h"
#include "Game.h"

class Entity
{
public:
	Entity(DirectX::SimpleMath::Vector2 pos, SpriteSheet* spriteSheet);
	Entity(DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector2 size, SpriteSheet* spriteSheet);
	
	virtual ~Entity() = default;

	virtual void Update(float dt) = 0;
	virtual void Render(DirectX::SpriteBatch* spriteBatch) = 0;

	DirectX::SimpleMath::Vector2 GetSize() const;
	void SetSize(const DirectX::SimpleMath::Vector2& size);

	DirectX::SimpleMath::Vector2 GetPosition() const;
	void SetPosition(const DirectX::SimpleMath::Vector2& pos);

	DirectX::SimpleMath::Vector2 GetVelocity() const;
	void SetVelocity(const DirectX::SimpleMath::Vector2& vel);

	DirectX::SimpleMath::Vector2 GetAcceleration() const;
	void SetAcceleration(const DirectX::SimpleMath::Vector2& accel);

	bool IsActive() const;
	bool IsCollidable() const;

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

	virtual void OnCollision(const CollisionEvent& event);

	virtual bool IsGrounded() const;

protected:

	DirectX::SimpleMath::Vector2 m_size;
	DirectX::SimpleMath::Vector2 m_pos;
	DirectX::SimpleMath::Vector2 m_vel;
	DirectX::SimpleMath::Vector2 m_accel;

	//physics
	bool m_isActive = true;
	bool m_isCollidable = true;

	//sprite
	int m_animId = 0;
	std::unique_ptr<Animator> m_animator;
	bool m_visible = true;
};

