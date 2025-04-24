#include "pch.h"
#include "World.h"
#include "Mario.h" // Assuming Mario is the derived class of Entity with state machine

using namespace DirectX;
using Keys = Keyboard::Keys;

void World::HandleInput(Keyboard::KeyboardStateTracker* kbTracker) {
    if (!player || !player->IsActive()) return;

    Mario* mario = dynamic_cast<Mario*>(player);
    if (!mario) return;

    mario->m_movementSM->HandleInput(kbTracker);
    mario->m_powerupSM->HandleInput(kbTracker);
}