#pragma once
#include "pch.h"
#include "Entity.h"
#include <vector>

class World
{
private:
	Entity* player;
	std::vector<Entity*> entities;
	int id;

public:
	World();
	~World();
	void Load();
	void Clear();
	void Update();
	void Render();
	void Reset();


	void HandleInput(DirectX::Keyboard::KeyboardStateTracker* kbTracker);
};

