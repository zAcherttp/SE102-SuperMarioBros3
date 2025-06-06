#pragma once
#include <vector>
#include <memory>
#include "json.hpp"

using json = nlohmann::json;
class Entity;
class Collision;

class World
{
public:
	World(std::string worldPath, std::string name);
	~World();

	void Update(float dt);
	void Render(DirectX::SpriteBatch* spriteBatch);
	void RenderDebug(DirectX::PrimitiveBatch<DirectX::DX11::VertexPositionColor>* primitiveBatch);

	void Load(SpriteSheet* spriteSheet);
	void Unload();
	void Reset();
	void Teleport();
	void TogglePause();
	void SetNextWorld();

	Entity* GetPlayer();
	static World* GetInstance();

	DirectX::XMVECTORF32 GetBackgroundColor() const;
	std::string GetName() const;
	int GetWidth() const;
	int GetHeight() const;

	void HandleInput(DirectX::Keyboard::State* kbState, DirectX::Keyboard::KeyboardStateTracker* kbsTracker);

	void AddEntity(Entity* entity);
	std::vector<Entity*> GetEntities();
	static void ResetInstance() { s_instance = nullptr; }
	std::unique_ptr<Collision> m_collisionSystem;
private:
	std::string m_path;
	std::string m_name;
	std::vector<Entity*> m_entities;
	Entity* m_player;
	int m_width;
	int m_height;
	DirectX::XMVECTORF32 m_background;

	static World* s_instance;

	bool m_isPaused;


	//Modules of the load function
	json LoadJsonFile(const std::string& filePath);
	void LoadWorldConfig(const json& data);
	void LoadEntities(const json& data, const json& anim, SpriteSheet* spriteSheet);
	Entity* CreateEntity(int entityType, const json& entityData, SpriteSheet* spriteSheet);
	void LoadAnimationsForEntity(Entity* entity, int entityType, const json& animData);
};

