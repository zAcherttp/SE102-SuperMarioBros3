#include "pch.h"
#include "Boomerang.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "Mario.h"
#include "GameConfig.h"
#include "Collision.h"
#include "World.h"
#include "BoomerangBro.h"
#include "Game.h"
#include "Block.h"

using namespace GameConstants::Enemies::BoomerangBro;

Boomerang::Boomerang(Vector2 position, Vector2 size, SpriteSheet* spriteSheet, bool direction) : 
    Enemy(position, size, spriteSheet)
    , m_animTimer(0.0f)
    , m_rotationTimer(0.0f)
    , m_frameTime(0.5f) 
    , m_rotationFrameTime(0.1f) 
    , m_currentRotationFrame(0)
    , m_lifeTimer(0.0f)
    , m_direction(direction) 
{
    std::vector<const wchar_t*> boomerangFrames = { L"boomerang" };
    m_state = BoomerangState::HOLDING; 
    m_visible = true;           
    m_isCollidable = true; 
	DefineAnimation(ID_ANIM_BOOMERANG, boomerangFrames, true, m_frameTime);
    m_startPositionX = position.x;
    m_startPositionY = position.y;

    BOOMERANG_VERTICAL_LENGTH = 0.0f;
    SetAnimation(ID_ANIM_BOOMERANG);    SetupCollisionComponent();    
    int nx = direction ? 1 : -1;
    m_accelerationX = HORIZONTAL_ACCELERATION * nx;
    m_accelerationY = GRAVITY * nx;
    SetVelocity(Vector2(-HORIZONTAL_SPEED * nx,-MAX_VERTICAL_SPEED));
}

void Boomerang::Update(float dt)
{   
    m_lifeTimer += dt;
    Vector2 vel = GetVelocity();
    Vector2 pos = GetPosition();
    vel.y += m_accelerationY * dt; 
    vel.x += m_accelerationX * dt; 
      if(abs(vel.y) > MAX_VERTICAL_SPEED)
    {
        vel.y = (vel.y > 0) ? MAX_VERTICAL_SPEED : -MAX_VERTICAL_SPEED;
    }    if(abs(vel.x) > HORIZONTAL_SPEED)
    {
        vel.x = (vel.x > 0) ? HORIZONTAL_SPEED : -HORIZONTAL_SPEED;
    }
    if(vel.y > 0 && pos.y >= m_startPositionY)
    {
        vel.y = 0; 
        SetState(BoomerangState::RETURN);
    }

    
    GetAnimator()->SetFlipHorizontal(!m_direction);

    if(GetState() != BoomerangState::HOLDING) 
    {
        UpdateRotation(dt); 
    }

    World* world = Game::GetInstance()->GetCurrentWorld();
    for (Entity* entity : world->GetEntities())
    {
        BoomerangBro* boomerangBro = dynamic_cast<BoomerangBro*>(entity);
        if (boomerangBro)
        {
            Vector2 boomerangSize = GetSize();
            Vector2 boomerangbroPosition = boomerangBro->GetPosition();
            Vector2 boomerangbroSize = boomerangBro->GetSize();

            Rectangle boomerangBox = { long(pos.x), long(pos.y), long(boomerangSize.x), long(boomerangSize.y) };
            Rectangle boomerangbroBox = { long(boomerangbroPosition.x), long(boomerangbroPosition.y), long(boomerangbroSize.x), long(boomerangbroSize.y) };

            if (boomerangBox.Intersects(boomerangbroBox) && this->GetState() == BoomerangState::RETURN)
            {
                boomerangBro->OnBoomerangCaught(this);
                Deactivate(); 
                return;
            }
        }
    }

    SetVelocity(vel);
    SetPosition(GetPosition() + GetVelocity() * dt);
}

void Boomerang::SetState(BoomerangState state)
{
    Vector2 vel = GetVelocity();
    Vector2 pos = GetPosition();
	switch (state)
	{	case BoomerangState::OUTWARD:
        m_state = BoomerangState::OUTWARD;
		vel.y = -INITIAL_UPWARD_SPEED;
		m_accelerationY = GRAVITY * 2;
		vel.x = HORIZONTAL_SPEED* 2 * (m_direction ? -1 : 1);
        SetVelocity(vel);
		break;
	case BoomerangState::RETURN:
        m_state = BoomerangState::RETURN;
		break;
	case BoomerangState::HOLDING:
        m_state = BoomerangState::HOLDING;
		m_accelerationX = 0;
		m_accelerationY = 0;
        SetVelocity(Vector2::Zero);
		break;
	default:
		break;
	}
}

void Boomerang::Render(DirectX::SpriteBatch* spriteBatch)
{
    if (!m_isActive) return;
    Vector2 pos = m_collisionComponent->GetPosition();
    pos.x = (float)(int)(pos.x + 0.5f);
    pos.y = (float)(int)(pos.y + 0.5f);
    m_animator->Draw(spriteBatch, pos, 0.0f);
}

void Boomerang::UpdatePosition(const Vector2& ownerPosition)
{
    SetPosition(ownerPosition + (m_direction ? Vector2(10.0f, -5.0f) : Vector2(-10.0f, -5.0f)));
}
void Boomerang::Deactivate()
{
    m_isActive = false;
    m_visible = false; 
    m_isCollidable = false;
}

void Boomerang::SetupCollisionComponent()
{
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x, curSize.y); 
	m_collisionComponent->SetSize(newSize);
	m_collisionComponent->SetPosition(GetPosition() + Vector2(newSize.x / 2, newSize.y / 2));
}

void Boomerang::UpdateRotation(float dt)
{
	m_rotationTimer += dt;

	if (m_rotationTimer >= m_rotationFrameTime)
	{
		m_rotationTimer = 0.0f;
		m_currentRotationFrame = (m_currentRotationFrame + 1) % ROTATION_FRAMES;

		if (GetAnimator())
		{
			float rotationAngle = m_currentRotationFrame * 3.14159f / 2.0f; // 0, 90, 180, 270 degrees
			GetAnimator()->SetRotation(rotationAngle);
		}
	}
}

void Boomerang::SetDirection()
{
    m_direction = !m_direction; // Toggle direction
    if (GetAnimator())
    {
        GetAnimator()->SetFlipHorizontal(m_direction);
    }
}

void Boomerang::OnCollision(const CollisionResult& event)
{
    Mario* mario = dynamic_cast<Mario*>(event.collidedWith);
    Block* block = dynamic_cast<Block*>(event.collidedWith);

    if (mario)
    {
        mario->Damage(); 
    }

    if(block) return;
}

