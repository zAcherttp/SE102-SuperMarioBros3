#include "pch.h"
#include "AssetIDs.h"
#include "Block.h"
#include "Brick.h"
#include "Debug.h"
#include "EffectManager.h"
#include "GameConfig.h"
#include "HiddenButton.h"
#include "Mario.h"
#include "World.h"

HiddenButton::HiddenButton(Vector2 position, Vector2 size, SpriteSheet* spriteSheet)
	: Block(position, size, spriteSheet)
{
	m_isSolid = true;
	DefineAnimation(ID_ANIM_HIDDEN_BUTTON, { L"pbutton1", L"pbutton2", L"pbutton3" }, true, 0.3f);
	DefineAnimation(ID_ANIM_HIDDEN_BUTTON_STOMPED, { L"pbutton4" }, false);
	SetAnimation(ID_ANIM_HIDDEN_BUTTON, true);
	SetupCollisionComponent();
}

void HiddenButton::Update(float dt)
{
	Entity::Update(dt);
}

void HiddenButton::Render(DirectX::SpriteBatch* spriteBatch)
{
	Vector2 pos = m_collisionComponent->GetPosition();
	pos.x = (float)(int)(pos.x + 0.5f);
	pos.y = (float)(int)(pos.y + 0.5f);
	m_animator->Draw(spriteBatch, pos, 1.0f);
}

void HiddenButton::OnCollision(const CollisionResult& event)
{
	Mario* mario = dynamic_cast<Mario*>(event.collidedWith);

	if (event.contactNormal.y > 0 && mario)
	{
		SetAnimation(ID_ANIM_HIDDEN_BUTTON_STOMPED, false);
		m_isCollidable = false;
		mario->SetVelocity(Vector2(mario->GetVelocity().x, -5.0f));
		m_isActivated = true;
		TransformAllBricks();
	}
}

void HiddenButton::SetupCollisionComponent()
{
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x, curSize.y);
	m_collisionComponent->SetSize(newSize);
	m_collisionComponent->SetPosition(GetPosition());
}

void HiddenButton::TransformAllBricks()
{
	World* world = World::GetInstance();

	Game::GetInstance()->GetCamera()->Shake(Vector2(0.f, 4.f), 8, .25f);

	// Find all bricks in the world and transform them to coins
	std::vector<Entity*> entities = world->GetEntities();
	for (auto* entity : entities)
	{
		Brick* brick = dynamic_cast<Brick*>(entity);
		if (brick)
		{
			brick->TransformToCoin();
		}
	}
}

