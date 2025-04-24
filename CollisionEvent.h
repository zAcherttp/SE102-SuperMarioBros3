#pragma once
#include <vector>
#include <unordered_map>

class Entity;

enum class CollisionDirection {
	None = 0,
	Top = 1,
    Right = 2,
	Bottom = 3,
	Left = 4,
};

class CollisionEvent {
	Entity* entityA;
	Entity* entityB;
	CollisionDirection aDirection;
	CollisionDirection bDirection;
};


