#pragma once
#include "pch.h"
#include "AssetIDs.h"
#include "BlackBackground.h"
#include "Brick.h"
#include "Bush.h"
#include "Cloud.h"
#include "Coin.h"
#include "Collision.h"
#include "Debug.h"
#include "DebugOverlay.h"
#include "EndPortal.h"
#include "Entity.h"
#include "FirePiranha.h"
#include "FloatingPlatform.h"
#include "Game.h"
#include "Goomba.h"
#include "Ground.h"
#include "HeadUpDisplay.h"
#include "Keyboard.h"
#include "LuckyBlock.h"
#include "Mario.h"
#include "ParaGoomba.h"
#include "Pipe.h"
#include "PrimitiveBatch.h"
#include "RedTroopas.h"
#include "ScrewBlock.h" 
#include "SimpleMath.h"
#include "SkyPlatform.h"
#include "SpriteBatch.h"
#include "SpriteSheet.h"
#include "VertexTypes.h"
#include "World.h"
#include <algorithm>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <exception>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace DirectX;
using Keys = Keyboard::Keys;

World* World::s_instance = nullptr;

World::World(std::string wPath, std::string name)
{
	s_instance = this; // singleton instance

	m_path = wPath;
	m_name = name;
	m_player = nullptr;
	m_entities = {};
	m_background = {};
	m_height = m_width = 0;
	m_isPaused = false;
}

World::~World()
{
	if (s_instance == this)
		s_instance = nullptr;

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

void World::HandleInput(Keyboard::State* kbState, Keyboard::KeyboardStateTracker* kbsTracker) {
	if (kbsTracker->IsKeyPressed(Keys::R)) {
		Reset();
	}
	if (kbsTracker->IsKeyPressed(Keys::I)) {
		TogglePause();
	}

	if (!m_player || !m_player->IsActive() || m_isPaused) return;
	Mario* mario = dynamic_cast<Mario*>(m_player);
	if (!mario || mario->IsTransitioning()) return;
	mario->HandleInput(kbState, kbsTracker);

	if (kbsTracker->IsKeyPressed(Keys::R)) {
		Reset();
	}
	if (kbsTracker->IsKeyPressed(Keys::T)) {
		Teleport();
	}
}

void World::AddEntity(Entity* entity)
{
	if (entity) {
		m_entities.push_back(entity);
	}
}

void World::Update(float dt) {
	m_entities.erase(
		std::remove_if(m_entities.begin(), m_entities.end(),
			[](Entity* e) { return e == nullptr || !e->IsActive(); }),
		m_entities.end()
	);

	if (m_isPaused) return;
	Mario* mario = dynamic_cast<Mario*>(m_player);
	bool skip = mario && mario->IsTransitioning() || mario->IsDying();

	if (m_collisionSystem && !skip) {

		if (m_player) {
			m_collisionSystem->UpdateEntity(m_player, dt);
		}

		for (auto e : m_entities) {
			if (!e->IsStatic()) {
				m_collisionSystem->UpdateEntity(e, dt);
			}
		}

		m_collisionSystem->ProcessCollisions(dt);
	}

	if (m_player) {
		m_player->Update(dt);
		Vector2 pos = m_player->GetPosition();

		pos.x = std::clamp(pos.x, 0.f + 8.f, m_width - 8.f);
		m_player->SetPosition(pos);

		if (pos.y > 500.f) Game::GetInstance()->RestartWorld();

		int gameWidth, gameHeight;
		Game::GetInstance()->GetDefaultGameSize(gameWidth, gameHeight);
		Vector2 cameraPos = pos - Vector2(gameWidth / 2.f, gameHeight / 2.f);
		//clamp position to nearest pixel to avoid pixel rendering artifacts
		//cameraPos.x = (int)(cameraPos.x + 0.5f);
		cameraPos.y = 239;

		Game::GetInstance()->SetCameraPosition(cameraPos, false);
		//Game::GetInstance()->MoveCamera(Vector2(20.f * dt, 0));
	}

	if (skip) return;

	for (auto e : m_entities) {
		e->Update(dt);
	}

	Game::GetInstance()->UpdateHUD(dt);
}

void World::Render(DirectX::SpriteBatch* spriteBatch) {
	if (m_player) m_player->Render(spriteBatch);
	for (auto e : m_entities) {
		e->Render(spriteBatch);
	}
}

void World::RenderDebug(DirectX::PrimitiveBatch<DirectX::DX11::VertexPositionColor>* primitiveBatch) {
	if (m_collisionSystem) {
		m_collisionSystem->RenderDebug(primitiveBatch);
	}

	for (auto e : m_entities) {
		if (e != nullptr && e->GetCollisionComponent()) {
			e->GetCollisionComponent()->RenderDebug(primitiveBatch, Colors::Lime);
		}
	}

	if (m_player)
	{
		m_player->GetCollisionComponent()->RenderDebug(primitiveBatch, Colors::Lime);
		Vector2 pos = m_player->GetPosition();
		Vector2 vel = m_player->GetVelocity();
		DebugOverlay::DrawLine(primitiveBatch, pos, pos + vel, Colors::Yellow);
	}
}

void World::Reset() {
	Game::GetInstance()->SwitchWorld();
}

void World::Teleport() {
	m_player->SetPosition(Vector2(700, 350));
	m_player->SetVelocity(Vector2::Zero);
}

void World::TogglePause()
{
	m_isPaused = !m_isPaused;
}

void World::Load(SpriteSheet* spriteSheet)
{
	try {
		json data = LoadJsonFile(m_path);
		json anim = LoadJsonFile(data["spritesData"]);

		LoadWorldConfig(data);
		LoadEntities(data, anim, spriteSheet);

		m_collisionSystem = std::make_unique<Collision>(m_width, m_height);

		if (m_player) {
			m_collisionSystem->AddEntity(m_player, 0.f);
		}

		for (auto& entity : m_entities) {
			m_collisionSystem->AddEntity(entity, 0.f);
		}
	}
	catch (const std::exception& e) {
		Log(__FUNCTION__, "Exception occurred: " + std::string(e.what()));
	}
	Log(__FUNCTION__, m_name + " loaded successfully");

	if (m_player)
		dynamic_cast<Mario*>(m_player)->ItsAMe();

	// Reset HUD
	// TODO: World saves mario state to Game, then after loading, set the properties
	HeadUpDisplay::GetInstance()->SetTime(300);
	HeadUpDisplay::GetInstance()->StartTimer();

	return;
}

void World::Unload()
{
	if (m_collisionSystem) {
		m_collisionSystem->Clear();
		m_collisionSystem.reset();
	}

	if (m_player) {
		delete m_player;
		m_player = nullptr;
	}

	for (auto entity : m_entities) {
		delete entity;
	}
	m_entities.clear();
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

	Game::GetInstance()->SetWorldSize(m_width, m_height);

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

Entity* World::CreateEntity(int entType, const json& data, SpriteSheet* spriteSheet)
{
	Entity* entity = nullptr;
	Vector2 position(data["x"], data["y"]);

	switch (entType) {
	case ID_ENT_MARIO:
		if (m_player) {
			Log(__FUNCTION__, "Mario has already been created!");
			return nullptr;
		}
		entity = new Mario(position, 0, 0, 0, spriteSheet);
		break;
	case ID_ENT_GOOMBA:
	{
		float width = data["width"];
		float height = data["height"];
		entity = new Goomba(position, Vector2(width, height), spriteSheet);
		break;
	}
	case ID_ENT_PARAGOOMBA:
	{
		float width = data["width"];
		float height = data["height"];
		entity = new ParaGoomba(position, Vector2(width, height), spriteSheet);
		break;
	}
	case ID_ENT_RED_PIRANHA_SPIT_FIRE:
	{
		float width = data["width"];
		float height = data["height"];
		entity = new FirePiranha(position, Vector2(width, height), spriteSheet);
		break;
	}
	case ID_ENT_RED_TROOPA:
	{
		float width = data["width"];
		float height = data["height"];
		entity = new RedTroopas(position, Vector2(width, height), spriteSheet);
		break;
	}
	case ID_ENT_GROUND:
	{
		float width = data["width"];
		float height = data["height"];
		int countX = data["countX"];
		int countY = data["countY"];
		bool isSolid = data["solid"];
		entity = new Ground(position, Vector2(width, height), countX, countY, isSolid, spriteSheet);
		break;
	}
	case ID_ENT_BUSH:
	{
		int brushType = data["brushType"];
		if (brushType == 0)
		{
			int tileXcount = data["tileXcount"];
			entity = new Bush(position, tileXcount, spriteSheet, brushType);
		}
		else entity = new Bush(position, 1, spriteSheet, brushType);
		break;
	}
	case ID_ENT_CLOUD:
	{
		int cloudType = data["cloudType"];
		if (cloudType == 0)
		{
			int count = data["count"];
			entity = new Cloud(position, count, spriteSheet, cloudType);
		}
		else entity = new Cloud(position, 0, spriteSheet, cloudType);
		break;
	}
	case ID_ENT_SCREW_BLOCK:
	{
		float width = data["width"];
		float height = data["height"];
		int countX = data["countX"];
		int countY = data["countY"];
		bool isSolid = data["solid"];
		bool isFloating = data["floating"];
		int color = data["color"];
		float depth = data["depth"];
		entity = new ScrewBlock(position, Vector2(width, height), countX, countY, isSolid, spriteSheet, depth, color, isFloating);
		break;
	}
	case ID_ENT_PIPE:
	{
		float width = data["width"];
		float height = data["height"];
		int countX = data["countX"];
		int countY = data["countY"];
		bool isSolid = data["solid"];
		bool hasHead = data["hasHead"];
		entity = new Pipe(position, Vector2(width, height), countX, countY, isSolid, spriteSheet, hasHead);
		break;
	}
	case ID_ENT_BRICK:
	{
		float width = data["width"];
		float height = data["height"];
		bool isSolid = data["solid"];
		entity = new Brick(position, Vector2(width, height), isSolid, spriteSheet);
		break;
	}
	case ID_ENT_COIN:
	{
		float width = data["width"];
		float height = data["height"];
		bool isSolid = data["solid"];
		entity = new Coin(position, Vector2(width, height), isSolid, spriteSheet);
		break;
	}
	case ID_ENT_SKY_PLATFORM:
	{
		float width = data["width"];
		float height = data["height"];
		int countX = data["countX"];
		bool isSolid = data["solid"];
		entity = new SkyPlatform(position, Vector2(width, height), countX, isSolid, spriteSheet);
		break;
	}
	case ID_ENT_LUCKY_BLOCK:
	{
		float width = data["width"];
		float height = data["height"];
		bool isSolid = data["solid"];
		bool isSpecial = data["isSpecial"];
		entity = new LuckyBlock(position, Vector2(width, height), isSolid, spriteSheet, isSpecial);
		break;
	}
	case ID_ENT_BLACK_BACKGROUND:
	{
		float width = data["width"];
		float height = data["height"];
		int countX = data["countX"];
		int countY = data["countY"];
		int bgtype = data["bgtype"];
		float depth = data["depth"];
		entity = new BlackBackground(position, Vector2(width, height), countX, countY, spriteSheet, bgtype, depth);
		break;
	}
	case ID_ENT_END_PORTAL:
	{
		float width = data["width"];
		float height = data["height"];
		entity = new EndPortal(position, Vector2(width, height), spriteSheet);
		break;
	}
	case ID_ENT_MOVING_PLATFORM:
	{
		float width = data["width"];
		float height = data["height"];
		bool isSolid = data["solid"];
		int countX = data["countX"];
		entity = new FloatingPlatform(position, Vector2(width, height), countX, isSolid, spriteSheet, Vector2(32.f, 0), 2.0f);
		break;
	}



	//// Add more entity types here as needed
	default:
		Log(__FUNCTION__, "Unknown entity type: " + std::to_string(entType));
		break;;
	}

	if (dynamic_cast<Mario*>(entity)) {
		m_player = dynamic_cast<Mario*>(entity);
		Log(__FUNCTION__, "Mario has been created!");
	}
	else if (entity) {
		m_entities.push_back(entity);
	}
	else {
		Log(__FUNCTION__, "Failed to create entity of type: " + std::to_string(entType));
		delete entity;
		return nullptr;
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

	// Special debugging for Goomba
	//if (type == ID_ENT_GOOMBA) {
	//	Log(__FUNCTION__, "Found animation data for Goomba (type " + std::to_string(type) + ")");
	//}

	for (const auto& sequence : (*typeIt)["sprites"]) {
		int animId = sequence.contains("id") ? sequence["id"].get<int>() : -1;
		if (animId == -1) continue;

		/*if (type == ID_ENT_GOOMBA) {
			Log(__FUNCTION__, "Loading Goomba animation ID: " + std::to_string(animId) +
				" Name: " + sequence["name"].get<std::string>() +
				" Frames: " + std::to_string(sequence["frames"].size()));
		}
		else {
			Log(__FUNCTION__, "Entity ID: " + std::to_string(type) + " Animation ID: " +
				std::to_string(animId) + " Frames: " + std::to_string(sequence["frames"].size()));
		}*/

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

			// Log frame names for Goomba
			/*if (type == ID_ENT_GOOMBA) {
				Log(__FUNCTION__, "Goomba frame: " + frameName);
			}*/
		}

		bool loop = sequence.contains("loop") ? sequence["loop"].get<bool>() : false;
		float timePerFrame = sequence.contains("timePerFrame") ? sequence["timePerFrame"].get<float>() : 0.1f;
		bool useVelocityScaling = sequence.contains("useVelocityScaling") ? sequence["useVelocityScaling"].get<bool>() : false;
		float minTimePerFrame = sequence.contains("minTimePerFrame") ? sequence["minTimePerFrame"].get<float>() : 0.05f;
		float maxTimePerFrame = sequence.contains("maxTimePerFrame") ? sequence["maxTimePerFrame"].get<float>() : 0.2f;
		float velocityScaleFactor = sequence.contains("velocityScaleFactor") ? sequence["velocityScaleFactor"].get<float>() : 1.0f;

		// Log(__FUNCTION__, "Animation " + std::to_string(animId) + " - timePerFrame: " + (sequence.contains("timePerFrame") ? "found" : "using default"));
		// Log(__FUNCTION__, "Animation " + std::to_string(animId) + " - loop: " + (sequence.contains("loop") ? "found" : "using default"));
		// Log(__FUNCTION__, "Animation " + std::to_string(animId) + " - useVelocityScaling: " + (sequence.contains("useVelocityScaling") ? "found" : "using default"));
		// Log(__FUNCTION__, "Animation " + std::to_string(animId) + " - minTimePerFrame: " + (sequence.contains("minTimePerFrame") ? "found" : "using default"));
		// Log(__FUNCTION__, "Animation " + std::to_string(animId) + " - maxTimePerFrame: " + (sequence.contains("maxTimePerFrame") ? "found" : "using default"));
		// Log(__FUNCTION__, "Animation " + std::to_string(animId) + " - velocityScaleFactor: " + (sequence.contains("velocityScaleFactor") ? "found" : "using default"));

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

int World::GetWidth() const { return m_width; }

int World::GetHeight() const { return m_height; }

World* World::GetInstance()
{
	return s_instance;
}