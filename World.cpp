#pragma once
#include "pch.h"
#include "World.h"
#include "Mario.h"
#include "Debug.h"
#include "AssetIDs.h"
#include "Game.h"
#include "SpriteSheet.h"

using namespace DirectX;
using Keys = Keyboard::Keys;

World::World(std::string wPath, std::string name)
{
	m_path = wPath;
	m_name = name;
	m_player = nullptr;
	m_entities = {};
	m_gravity = 0;
	m_background = {};
	m_height = m_width = 0;
}

World::~World()
{
	if (m_player)
	{
		delete m_player;
		m_player = nullptr;
	}

	for (auto entity : m_entities)
	{
		delete entity;
	}
	m_entities.clear();
}

void World::HandleInput(Keyboard::KeyboardStateTracker* kbTracker) {
	if (!m_player || !m_player->IsActive()) return;
	Mario* mario = dynamic_cast<Mario*>(m_player);
	if (!mario) return;

	mario->HandleInput(kbTracker);
}

void World::Update(float dt) {
	m_player->Update(dt);
	for (auto e : m_entities) {
		e->Update(dt);
	}
}

void World::Render(DirectX::SpriteBatch* spriteBatch) {
	m_player->Render(spriteBatch);
	for (auto e : m_entities) {
		e->Render(spriteBatch);
	}
}

void World::Reset() {

}

void World::Load(SpriteSheet* spriteSheet)
{
	try {
		json data = LoadJsonFile(m_path);
		json anim = LoadJsonFile(data["spritesData"]);

		LoadWorldConfig(data);
		LoadEntities(data, anim, spriteSheet);
	}
	catch (const std::exception& e) {
		Log(__FUNCTION__, "Exception occurred: " + std::string(e.what()));
	}
	Log(__FUNCTION__, m_name + " loaded successfully");

	if(m_player)
		dynamic_cast<Mario*>(m_player)->ItsAMe();

	return;
}

void World::Unload()
{
	if (m_player)
	{
		delete m_player;
		m_player = nullptr;
	}
	for (auto entity : m_entities)
	{
		delete entity;
	}
	m_entities.clear();
	m_gravity = 0;

	m_name = "";
	m_path = "";

	return;
}

json World::LoadJsonFile(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open()) {
		Log(__FUNCTION__, "Failed to open file: " + filePath);
		return json{};
	}
	if (file.fail()) {
		Log(__FUNCTION__, "Failed to parse JSON from file: " + filePath);
		file.close();
		return json{};
	}
	json data{};
	file >> data;
	file.close();
	return data;
}

void World::LoadWorldConfig(const json& data) {
	m_width = data["width"];
	m_height = data["height"];

	const json& c = data["background"];
	m_background = { c["r"], c["b"], c["g"], c["a"] };
}

void World::LoadEntities(const json& data, const json& anim, SpriteSheet* spriteSheet) {
	for (const auto& e : data["entities"]) {
		int type = e["type"];

		Entity* ent = CreateEntity(type, e, spriteSheet);

		if (!ent) continue;

		LoadAnimationsForEntity(ent, type, anim);
	}
}

Entity* World::CreateEntity(int type, const json& data, SpriteSheet* spriteSheet)
{
	Entity* entity = nullptr;
	Vector2 position(data["x"], data["y"]);

	switch (type) {
	case ID_ENT_MARIO:
		if (m_player) {
			Log(__FUNCTION__, "Mario has already been created!");
			return nullptr;
		}
		entity = new Mario(position, 0, 0, 0, spriteSheet);
		m_player = (Mario*)entity;
		Log(__FUNCTION__, "Mario has been created!");
		break;
	case ID_ENT_BRICK:
		break;
	default:
		Log(__FUNCTION__, "Unknown entity type: " + std::to_string(type));
		break;;
	}

	return entity;
}

void World::LoadAnimationsForEntity(Entity* entity, int type, const json& anim)
{
	auto typeIt = std::find_if(anim.begin(), anim.end(),
		[type](const json& item) {
			return item.contains("type") && item["type"] == type;
		});

	if (typeIt == anim.end() || !typeIt->contains("sprites")) {
		Log(__FUNCTION__, "No animation data found for entity type: " + std::to_string(type));
		return;
	}

	for (const auto& sequence : (*typeIt)["sprites"]) {
		int animId = sequence.contains("id") ? sequence["id"].get<int>() : -1;
		if (animId == -1) continue;

		//Log(__FUNCTION__, "Entity ID: " + std::to_string(type) + " Animation ID: " + std::to_string(animId) + " Frames: " + std::to_string(sequence["frames"].size()));

		std::vector<const wchar_t*> frameNames;

		// Convert frame names from json array to wchar_t* vector
		for (const auto& frame : sequence["frames"]) {
			// Convert string to wstring and store in a map for persistence
			std::string frameName = frame;
			static std::map<std::string, std::wstring> frameNameCache;

			if (frameNameCache.find(frameName) == frameNameCache.end()) {
				frameNameCache[frameName] = std::wstring(frameName.begin(), frameName.end());
			}

			// Use the cached wstring's c_str()
			frameNames.push_back(frameNameCache[frameName].c_str());
		}
		bool loop = sequence.contains("loop") ? sequence["loop"].get<bool>() : false;
		float timePerFrame = sequence.contains("timePerFrame") ? sequence["timePerFrame"].get<float>() : 0.1f;
		bool useVelocityScaling = sequence.contains("useVelocityScaling") ? sequence["useVelocityScaling"].get<bool>() : false;
		float minTimePerFrame = sequence.contains("minTimePerFrame") ? sequence["minTimePerFrame"].get<float>() : 0.05f;
		float maxTimePerFrame = sequence.contains("maxTimePerFrame") ? sequence["maxTimePerFrame"].get<float>() : 0.2f;
		float velocityScaleFactor = sequence.contains("velocityScaleFactor") ? sequence["velocityScaleFactor"].get<float>() : 1.0f;

		/*Log(__FUNCTION__, "Animation " + std::to_string(animId) + " - timePerFrame: " + (sequence.contains("timePerFrame") ? "found" : "using default"));
		Log(__FUNCTION__, "Animation " + std::to_string(animId) + " - loop: " + (sequence.contains("loop") ? "found" : "using default"));
		Log(__FUNCTION__, "Animation " + std::to_string(animId) + " - useVelocityScaling: " + (sequence.contains("useVelocityScaling") ? "found" : "using default"));
		Log(__FUNCTION__, "Animation " + std::to_string(animId) + " - minTimePerFrame: " + (sequence.contains("minTimePerFrame") ? "found" : "using default"));
		Log(__FUNCTION__, "Animation " + std::to_string(animId) + " - maxTimePerFrame: " + (sequence.contains("maxTimePerFrame") ? "found" : "using default"));
		Log(__FUNCTION__, "Animation " + std::to_string(animId) + " - velocityScaleFactor: " + (sequence.contains("velocityScaleFactor") ? "found" : "using default"));*/

		entity->DefineAnimation(
			animId,
			frameNames,
			loop,
			timePerFrame,
			useVelocityScaling,
			minTimePerFrame,
			maxTimePerFrame,
			velocityScaleFactor);
	}
}

std::vector<Entity*>& World::GetEntities()
{
	return m_entities;
}

Entity* World::GetPlayer()
{
	return m_player;
}

XMVECTORF32 World::GetBackgroundColor() const
{
	return m_background;
}

std::string World::GetName() const
{
	return m_name;
}
