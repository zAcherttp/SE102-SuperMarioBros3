#pragma once
#include <vector>

class Entity;

class World
{
private:
	std::string m_path;
	std::string m_name;
	std::vector<Entity*> m_entities;
	Entity* m_player;
	float m_gravity;
public:
	World();
	~World();
	void Update(float dt);
	void Render();
	void Reset();
	bool Load();
	bool Unload();

	std::string GetName();

	void HandleInput(DirectX::Keyboard::KeyboardStateTracker* kbTracker);
};

