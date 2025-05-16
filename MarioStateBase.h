#pragma once
#include "State.h"

using namespace DirectX::SimpleMath;

class Mario;
enum class Direction { Left = -1, Right = 1 };

class MarioStateBase : protected State<Mario> {
public:
	MarioStateBase() : State() {};
	void Update(Mario* mario, float dt) override = 0;
	virtual void Enter(Mario* mario) override = 0;
	virtual void Exit(Mario* mario) override = 0;
	virtual std::string GetStateName() const = 0;
	virtual int GetStateAnimValue() const = 0;
	virtual Vector2 GetStateSizeOffset() const = 0;
	void SetAnimation(Mario* mario, int animId) const;
};

