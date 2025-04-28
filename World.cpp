#include "pch.h"
#include "World.h"
#include "Mario.h"
#include <vector>

using namespace DirectX;
using Keys = Keyboard::Keys;

World::World() {
    m_name = "";
    m_path = "";
    m_gravity = 0;
    m_entities = {};
    m_player = nullptr;
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
    dt;
}

void World::Render() {

}

void World::Reset() {

}

bool World::Load()
{
    return false;
}

bool World::Unload()
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

	return true;
}

std::string World::GetName()
{
    return m_name;
}
