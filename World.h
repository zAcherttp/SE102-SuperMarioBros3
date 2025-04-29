#pragma once
#include <vector>
#include "json.hpp"
using json = nlohmann::json;

class Entity;

class World
{
private:
	std::string m_path;
	std::string m_name;
	std::vector<Entity*> m_entities;
	Entity* m_player;
	int m_width;
	int m_height;
	DirectX::XMVECTORF32 m_background;
	float m_gravity;
public:
	World(std::string worldPath, std::string name);
	~World();
	void Update(float dt);
	void Render(DirectX::SpriteBatch* spriteBatch);
	void Reset();
	void Load(SpriteSheet* spriteSheet);
	void Unload();

	std::vector<Entity*>& GetEntities();
	Entity* GetPlayer();

	DirectX::XMVECTORF32 GetColor() const;
	std::string GetName() const;

	void HandleInput(DirectX::Keyboard::KeyboardStateTracker* kbTracker);
};

