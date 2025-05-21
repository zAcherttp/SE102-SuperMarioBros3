#include "pch.h"
#include "AssetIDs.h"
#include "Effect.h"
#include "Entity.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteSheet.h"
#include <vector>

Effect::Effect(Vector2 position, Vector2 size, SpriteSheet* spriteSheet, EffectType type)
	: Entity(position, size, spriteSheet)
	, m_animTimer(0.0f)
	, m_type(type)
	, m_isFlipped(false)
	, m_flipTimer(0.0f)
	, m_flipTime(0.2f) // Time between flip frames
	, m_velocity(Vector2::Zero)
	, m_gravity(750.0f)
	, m_initialJumpSpeed(-300.0f)
	, m_spriteSheet(spriteSheet)

{
	std::vector<const wchar_t*> frames;

	switch (type)
	{
	case EffectType::POINT:
	{
		break;
	}
	case EffectType::BONK:
	{
		frames = { L"spark1", L"spark2" };
		DefineAnimation(ID_ANIM_EFFECT_BONK, frames, true, 0.1f);
		SetAnimation(ID_ANIM_EFFECT_BONK, true);

		break;
	}
	case EffectType::COIN:
	{
		frames = { L"coin-collect1", L"coin-collect2", L"coin-collect3" };
		DefineAnimation(ID_ANIM_EFFECT_COIN, frames, true, 0.07f);
		SetAnimation(ID_ANIM_EFFECT_COIN, true);
		m_velocity.y = m_initialJumpSpeed;
		break;
	}
	case EffectType::SMOKE:
	{
		frames = { L"pop1", L"pop2", L"pop3" , L"pop4" };
		DefineAnimation(ID_ANIM_EFFECT_SMOKE, frames, true, 0.1f);
		SetAnimation(ID_ANIM_EFFECT_SMOKE, true);
		m_velocity.y = m_initialJumpSpeed;
		break;
	}

	}
}

void Effect::Render(DirectX::SpriteBatch* spriteBatch)
{
    Entity::Render(spriteBatch);
    
    // Render brick particles if applicable
    for (auto& particle : m_brickParticles) {
        if (particle->IsActive()) {
            particle->Render(spriteBatch);
        }
    }
}

void Effect::Update(float dt)
{
	if (!m_isActive) return;

	switch (m_type)
	{
		case EffectType::BONK:
		{
			m_animTimer += dt;
			m_flipTimer += dt;
			if (m_animTimer > 0.5f)
			{
				Deactivate();
				return;
			}
			if (m_flipTimer > m_flipTime)
			{
				m_isFlipped = !m_isFlipped;
				m_animator->SetFlipHorizontal(m_isFlipped);
				m_flipTimer = 0.0f;
			}
			break;
		}
		case EffectType::COIN:
		{
			m_animTimer += dt;

			m_velocity.y += m_gravity * dt;

			// Update position based on velocity
			Vector2 position = GetPosition();
			position += m_velocity * dt;
			SetPosition(position);

			if (m_animTimer > 0.7f)
			{
				Deactivate();
				return;
			}
			break;
		}
		case EffectType::SMOKE:
		{
			m_animTimer += dt;
			m_flipTimer += dt;
			if (m_animTimer > 0.4f)
			{
				Deactivate();
				return;
			}
			if (m_flipTimer > m_flipTime)
			{
				m_isFlipped = !m_isFlipped;
				m_animator->SetFlipHorizontal(m_isFlipped);
				m_flipTimer = 0.0f;
			}
			break;
		}
		case EffectType::BRICK:
		{
			SpawnBrickParticle();
			// Update all particles
			for (auto it = m_brickParticles.begin(); it != m_brickParticles.end();) {
				if ((*it)->IsActive()) {
					(*it)->Update(dt);
					++it;
				} else {
					it = m_brickParticles.erase(it);
				}
			}
			
			// Deactivate effect when all particles are gone
			if (m_brickParticles.empty() && m_animTimer > 0.1f) {
				Deactivate();
				return;
			}
			
			m_animTimer += dt;
			break;
		}	
	}
	Entity::Update(dt);
}

void Effect::Deactivate()
{
	m_isActive = false;
	m_visible = false;
}

void Effect::SpawnBrickParticle()
{
    if (!m_brickParticles.empty()) return; // Only spawn once
    
    Vector2 position = GetPosition();

	Vector2 positions[4] = {
		position,
		position + Vector2(16,0),
		position + Vector2(0,16),
		position + Vector2(16,16),
	};

    Vector2 particleSize(8,9); 
    
    // Create 4 particles with different trajectories
    Vector2 velocities[4] = {
        Vector2(-60, -350),  // Top-left
        Vector2(60, -350),   // Top-right
        Vector2(-40, -300),   // Bottom-left
        Vector2(40, -300)     // Bottom-right
    };
    
    for (int i = 0; i < 4; i++) {
        auto particle = std::make_shared<BrickParticle>(positions[i], particleSize, 
                                                      m_spriteSheet, velocities[i]);
        m_brickParticles.push_back(particle);
    }
}
