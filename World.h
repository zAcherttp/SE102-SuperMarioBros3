#pragma once
#include <vector>

class Entity;

class World
{
private:
	Entity* m_player;
	std::vector<Entity*> m_entities;
	int m_id;
	float m_gravity;
public:
	World();
	~World();
	void Clear();
	void Update(float dt);
	void Render();
	void Reset();


	void HandleInput(DirectX::Keyboard::KeyboardStateTracker* kbTracker);
};

