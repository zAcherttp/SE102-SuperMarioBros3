#pragma once
#include "pch.h"
#include "Entity.h"

using namespace DirectX::SimpleMath;

class Enemy : public Entity
{
public:
	Enemy(Vector2 position, Vector2 size, SpriteSheet* spriteSheet) : Entity(position, size, spriteSheet) {};
};

