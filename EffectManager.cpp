#include "pch.h"
#include "EffectManager.h"
#include "Effect.h"
#include "Game.h"
#include "Debug.h"


// Initialize static instance
EffectManager* EffectManager::s_instance = nullptr;

EffectManager::EffectManager(SpriteSheet* spriteSheet) : m_spriteSheet(spriteSheet) {
	s_instance = this;
}

EffectManager::~EffectManager() {
	if (s_instance == this) {
		s_instance = nullptr;
	}
	ClearEffects();
}

Effect* EffectManager::CreateBonkEffect(const Vector2& position) {
	return CreateEffect(position, Vector2::Zero, EffectType::BONK);
}

Effect* EffectManager::CreatePointEffect(const Vector2& position, int points, bool isCombo) {
	int comboPoints;
	if(isCombo) {
		m_comboCount++;
		m_comboTimer = 0.0f;
		comboPoints = CalculateComboPoints(points);
	} else {
		m_comboCount = 0;
		m_comboTimer = 0.0f;
		comboPoints = points;
	}

	Game::GetInstance()->AddScore(comboPoints);

	return CreateEffect(position + Vector2(0, -16), Vector2::Zero, EffectType::POINT, comboPoints);
}

Effect* EffectManager::CreateCoinEffect(const Vector2& position) {
	return CreateEffect(position, Vector2::Zero, EffectType::COIN);
}

Effect* EffectManager::CreateBrickEffect(const Vector2& position) {
	return CreateEffect(position, Vector2::Zero, EffectType::BRICK);
}

Effect* EffectManager::CreateSmokeEffect(const Vector2& position) {
	return CreateEffect(position, Vector2::Zero, EffectType::SMOKE);
}

Effect *EffectManager::CreateOneUpEffect(const Vector2 &position)
{
	return CreateEffect(position, Vector2::Zero, EffectType::ONE_UP);
}

Effect* EffectManager::CreateEffect(const Vector2& position, Vector2 size, EffectType type, int points) {

	if (!m_spriteSheet) {
		m_spriteSheet = Game::GetInstance()->GetSpriteSheet();
	}

	std::unique_ptr<Effect> effect = std::make_unique<Effect>(position, size, m_spriteSheet, type, points);

	Effect* effectPtr = effect.get();
	m_effects.push_back(std::move(effect));

	return effectPtr;
}

void EffectManager::Update(float dt) {
	UpdateCombo(dt);
	for (auto& effect : m_effects) {
		if (effect->IsActive()) {
			effect->Update(dt);
		}
	}

	auto newEnd = std::remove_if(m_effects.begin(), m_effects.end(),
		[](const std::unique_ptr<Effect>& e) { return !e->IsActive(); });

	if (newEnd != m_effects.end()) {
		m_effects.erase(newEnd, m_effects.end());
	}
}

void EffectManager::Render(DirectX::SpriteBatch* spriteBatch) {
	for (auto& effect : m_effects) {
		if (effect->IsActive()) {
			effect->Render(spriteBatch);
		}
	}
}

void EffectManager::ClearEffects() {
	m_effects.clear();
}


int EffectManager::CalculateComboPoints(int basePoints) {
    // SMB3 combo system: 100, 200, 400, 800, 1000, 2000, 4000, 8000
    switch (m_comboCount) {
        case 1: return 100;
        case 2: return 200;
        case 3: return 400;
        case 4: return 800;
        case 5: return 1000;
        case 6: return 2000;
        case 7: return 4000;
        case 8: return 8000;
        default: return basePoints;
    }
}

void EffectManager::UpdateCombo(float dt) {
    if (m_comboCount > 0) {
        m_comboTimer += dt;

        if (m_comboTimer >= COMBO_TIMEOUT) {
            ResetCombo();
        }
    }
}

void EffectManager::ResetCombo() {
    m_comboCount = 0;
    m_comboTimer = 0.0f;
}