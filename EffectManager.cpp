#include "pch.h"
#include "EffectManager.h"
#include "Effect.h"
#include "Game.h"
#include "Debug.h"


// Initialize static instance
EffectManager* EffectManager::s_instance = nullptr;

EffectManager::EffectManager(SpriteSheet* spriteSheet) : m_spriteSheet(spriteSheet) {
	s_instance = this;
	Log(__FUNCTION__, "EffectManager created");
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

Effect* EffectManager::CreatePointEffect(const Vector2& position, int points) {
	points;
	return CreateEffect(position, Vector2::Zero, EffectType::POINT);
}

Effect* EffectManager::CreateCoinEffect(const Vector2& position) {
	return CreateEffect(position, Vector2::Zero, EffectType::COIN);
}

Effect* EffectManager::CreateBrickEffect(const Vector2& position) {
	return CreateEffect(position, Vector2::Zero, EffectType::BRICK);
}

Effect* EffectManager::CreateEffect(const Vector2& position, Vector2 size, EffectType type) {

	if (!m_spriteSheet) {
		m_spriteSheet = Game::GetInstance()->GetSpriteSheet();
	}

	std::unique_ptr<Effect> effect = std::make_unique<Effect>(position, size, m_spriteSheet, type);

	Effect* effectPtr = effect.get();
	m_effects.push_back(std::move(effect));

	return effectPtr;
}

void EffectManager::Update(float dt) {
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
