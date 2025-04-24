#pragma once
#include "State.h"
#include "Animator.h"
#include "CollisionEvent.h"

class Entity
{
public:
	Entity(DirectX::SimpleMath::Vector2 pos)
		: m_pos(pos) {
	}
	Entity(DirectX::SimpleMath::Vector2 pos, DirectX::SimpleMath::Vector2 size)
		: m_pos(pos), m_size(size) {
	}
	~Entity() = default;

	virtual void Update(float dt) = 0;
	virtual void Render() = 0;


	DirectX::SimpleMath::Vector2 GetSize() const { return m_size; }
	void SetSize(const DirectX::SimpleMath::Vector2& size) { m_size = size; }

	DirectX::SimpleMath::Vector2 GetPosition() const { return m_pos; }
	void SetPosition(const DirectX::SimpleMath::Vector2& pos) { m_pos = pos; }

	DirectX::SimpleMath::Vector2 GetVelocity() const { return m_vel; }
	void SetVelocity(const DirectX::SimpleMath::Vector2& vel) { m_vel = vel; }

	DirectX::SimpleMath::Vector2 GetAcceleration() const { return m_accel; }
	void SetAcceleration(const DirectX::SimpleMath::Vector2& accel) { m_accel = accel; }

	bool IsActive() const { return m_isActive; }
	bool IsCollidable() const { return m_isCollidable; }

	virtual void OnCollision(const CollisionEvent& event) {
		event;
	}

	virtual bool IsGrounded() const {
		return false;
	}
private:
	/// <summary>
	/// Smart pointer for entity animator.
	/// </summary>
	std::unique_ptr<Animator> m_animator;

	DirectX::SimpleMath::Vector2 m_size;
	DirectX::SimpleMath::Vector2 m_pos;
	DirectX::SimpleMath::Vector2 m_vel;
	DirectX::SimpleMath::Vector2 m_accel;

	bool m_isActive = true;
	bool m_isCollidable = true;
};

