#include "pch.h"
#include "AssetIDs.h"
#include "Effect.h"
#include "Entity.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteSheet.h"
#include <vector>

Effect::Effect(Vector2 position, Vector2 size, SpriteSheet* spriteSheet, EffectType type,int points)
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
		switch (points)
		{
			case 100:
			{
				frames = { L"100" };
				DefineAnimation(ID_ANIM_EFFECT_POINT_100, frames, false);
				SetAnimation(ID_ANIM_EFFECT_POINT_100, false);
				break;
			}
			case 200:
			{
				frames = { L"200" };
				DefineAnimation(ID_ANIM_EFFECT_POINT_200, frames, false);
				SetAnimation(ID_ANIM_EFFECT_POINT_200, false);
				break;
			}
			case 400:
			{
				frames = { L"400" };
				DefineAnimation(ID_ANIM_EFFECT_POINT_400, frames, false);
				SetAnimation(ID_ANIM_EFFECT_POINT_400, false);
				break;
			}
			case 800:
			{
				frames = { L"800" };
				DefineAnimation(ID_ANIM_EFFECT_POINT_800, frames, false);
				SetAnimation(ID_ANIM_EFFECT_POINT_800, false);
				break;
			}
			case 1000:
			{
				frames = { L"1000" };
				DefineAnimation(ID_ANIM_EFFECT_POINT_1000, frames, false);
				SetAnimation(ID_ANIM_EFFECT_POINT_1000, false);
				break;
			}
			case 2000:
			{
				frames = { L"2000" };
				DefineAnimation(ID_ANIM_EFFECT_POINT_2000, frames, false);
				SetAnimation(ID_ANIM_EFFECT_POINT_2000, false);
				break;
			}
			case 4000:
			{
				frames = { L"4000" };
				DefineAnimation(ID_ANIM_EFFECT_POINT_4000, frames, false);
				SetAnimation(ID_ANIM_EFFECT_POINT_4000, false);
				break;
			}
			case 8000:
			{
				frames = { L"8000" };
				DefineAnimation(ID_ANIM_EFFECT_POINT_8000, frames, false);
				SetAnimation(ID_ANIM_EFFECT_POINT_8000, false);
				break;
			}
			default:
				frames = { L"100" };
				DefineAnimation(ID_ANIM_EFFECT_POINT_100, frames, false);
				SetAnimation(ID_ANIM_EFFECT_POINT_100, false);
				break;

		}
		m_velocity.y = -30.0f;
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
		break;
	}	
	case EffectType::ONE_UP:
	{
		frames = { L"1up" };
		DefineAnimation(ID_ANIM_EFFECT_ONE_UP, frames, false);
		SetAnimation(ID_ANIM_EFFECT_ONE_UP, false);
		m_velocity.y = -30.0f;
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
				EffectManager::GetInstance()->CreatePointEffect(GetPosition() + Vector2(0.0f, 16.0f), 100, true);
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
			
			if (m_brickParticles.empty() && m_animTimer > 0.1f) {
				Deactivate();
				return;
			}
			
			m_animTimer += dt;
			break;
		}	
		case EffectType::ONE_UP:
		{
			m_animTimer += dt;
			Vector2 position = GetPosition();
			position += m_velocity * dt;
			SetPosition(position);
			if (m_animTimer >= 1.0f)
			{
				Deactivate();
				return;
			}
			break;
		}
		case EffectType::POINT:
		{
			m_animTimer += dt;
			Vector2 position = GetPosition();
			position += m_velocity * dt;
			SetPosition(position);
			if (m_animTimer >= 0.75f)
			{
				Deactivate();
				return;
			}
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
