#include "pch.h"
#include "BoomerangBro.h"
#include "Debug.h"
#include "AssetIDs.h"
#include "Mario.h"
#include "Game.h"
#include "World.h"
#include "Collision.h"
#include "GameConfig.h"
#include "Boomerang.h"
#include "Block.h"

BoomerangBro::BoomerangBro(Vector2 position, Vector2 size, SpriteSheet* spriteSheet)
    : Enemy(position, size, spriteSheet)  // 2 seconds in firing state
    , m_boomerang1(nullptr)
    , m_boomerang2(nullptr)
    , m_world(nullptr)
    , m_isFlipped(false) // Default: looking left (original sprite direction)
    , m_state(BoomerangBroState::WALKRIGHT)
    , m_isThrowingPhase(false)
    , m_phaseTimer(0.0f)
    , m_waitTimer(0.0f)
    , m_jumpTimer(0.0f)
{
    SetupCollisionComponent();
    Log(__FUNCTION__, "Collision component initialized");
    m_mario = dynamic_cast<Mario*>(World::GetInstance()->GetPlayer());
    m_isCollidable = true;

    // m_boomerang1 = new Boomerang(position + BULLET_OFFSET, Vector2(BULLET_WIDTH, BULLET_HEIGHT), spriteSheet);
    // m_boomerang2 = new Boomerang(position + BULLET_OFFSET + Vector2(16.0f, 0.0f), Vector2(BULLET_WIDTH, BULLET_HEIGHT), spriteSheet);
    SetAnimation(ID_ANIM_BOOMERANG_BRO_THROW);
    m_leftX = position.x;
    m_rightX = position.x + 48;
    m_jumpTime = rand() % 3 + 5;
}

void BoomerangBro::Update(float dt)
{
    Vector2 pos = GetPosition();
    Vector2 vel = GetVelocity();
    
    m_isGrounded = Collision::GetInstance()->GroundCheck(this, dt);
    
	if (!m_isGrounded) {
        vel.y += GameConfig::Physics::GRAVITY * dt;
		SetVelocity(vel);
	}
    
    m_phaseTimer += dt;
    // Log(LOG_INFO, "BoomerangBro phase timer: " + std::to_string(m_phaseTimer));
    if (m_phaseTimer >= 2.54f) {
        m_isThrowingPhase = !m_isThrowingPhase;
        m_phaseTimer = 0.0f; // Reset phase timer
        m_isThrowingPhase ? SetAnimation(ID_ANIM_BOOMERANG_BRO_THROW) : SetAnimation(ID_ANIM_BOOMERANG_BRO_WALK);
        m_state = BoomerangBroState::WALKRIGHT;
    }

    if(m_boomerang1)
    {
        m_boomerang1->m_lifeTimer += dt;
        if (m_boomerang1->m_lifeTimer >= BOOMERANG_LIFETIME) {
            m_boomerang1->Deactivate(); // Deactivate boomerang after its life time
            m_boomerang1 = nullptr; // Remove reference to the boomerang
        }
    }
    if(m_boomerang2)
    {
        m_boomerang2->m_lifeTimer += dt;
        if (m_boomerang2->m_lifeTimer >= BOOMERANG_LIFETIME) {
            m_boomerang2->Deactivate(); // Deactivate boomerang after its life time
            m_boomerang2 = nullptr; // Remove reference to the boomerang
        }
    }

    m_jumpTimer += dt;
    if (m_jumpTimer >= m_jumpTime) {
        Jump(); // Trigger jump after the specified time
        m_jumpTimer = 0.0f; // Reset jump timer
        m_jumpTime = rand() % 3 + 5; // Randomize jump time between 1 and 2 seconds
    }
    
    switch (m_state)
    {
        case BoomerangBroState::WALKRIGHT:
        if (pos.x < m_rightX) {
            vel.x = 50.0f; // Move right
        }
        if(pos.x >= m_rightX) {
            m_state = BoomerangBroState::WAIT; // Switch to wait state
            vel.x = 0.0f; // Stop moving
            m_waitTimer = 0.0f; // Reset wait timer
            pos.x = m_rightX; // Ensure position is clamped to right boundary
        }
        SetVelocity(Vector2(vel.x, GetVelocity().y));
        break;
        case BoomerangBroState::WALKLEFT:
        if (pos.x > m_leftX) {
            vel.x = -50.0f; // Move left
        }
        if(pos.x <= m_leftX) {
            m_state = BoomerangBroState::WAIT; // Switch to wait state
            vel.x = 0.0f; // Stop moving
            m_waitTimer = 0.0f; // Reset wait timer
            pos.x = m_leftX; // Ensure position is clamped to left boundary
        }
        SetVelocity(Vector2(vel.x, GetVelocity().y));
        break;
        case BoomerangBroState::WAIT:
        m_animator->Pause(); // Pause animation while waiting
        vel.x = 0.0f; // Stop moving
        m_waitTimer += dt;
        if (m_waitTimer >= 0.3f) { // Wait for 0.3 seconds
            m_animator->Play(); // Resume animation
            pos.x >= m_rightX ? m_state = BoomerangBroState::WALKLEFT : m_state = BoomerangBroState::WALKRIGHT;
            m_waitTimer = 0.0f; // Reset wait timer
        }

        SetVelocity(Vector2(vel.x, GetVelocity().y));
        break;
    }

    if (m_boomerang1 && m_boomerang1->GetState() == BoomerangState::RETURN && CheckAABBCollision(this, m_boomerang1)) {
        OnBoomerangCaught(m_boomerang1);
    }
    if (m_boomerang2 && m_boomerang2->GetState() == BoomerangState::RETURN && CheckAABBCollision(this, m_boomerang2)) {
        OnBoomerangCaught(m_boomerang2);
    }

    // Update bro's orientation based on Mario's position
    UpdateBroOrientation(m_mario);

    // Update boomerangs if they exist
    if (m_boomerang1) {
        m_boomerang1->Update(dt);
    }
    if (m_boomerang2) {
        m_boomerang2->Update(dt);
    }

    if(m_isThrowingPhase) {
        if(m_animator->GetCurrentFrame() == 4 || m_animator->GetCurrentFrame() == 5) {
            if(!m_isBommerangCreated) {
                CreateBoomerangs(); // Create boomerangs only once when entering throwing phase
                m_isBommerangCreated = true;
                m_isBoomerangThrown = false; 
            }
            Hold();
        }
        else if (m_animator->GetCurrentFrame() == 6 || m_animator->GetCurrentFrame() == 7) {
            if (!m_isBoomerangThrown) {
                Throw(); // Throw boomerangs when in throwing phase
                m_isBommerangCreated = false; // Reset boomerang creation flag
                m_isBoomerangThrown = true;
            }
        }
    }
    else {
        m_isBommerangCreated = false; // Reset boomerang creation flag when not in throwing phase
        m_isBoomerangThrown = false; // Reset boomerang thrown flag when not in throwing phase
    }

    SetPosition(GetPosition() + GetVelocity() * dt);
    Entity::Update(dt);
}

void BoomerangBro::Render(DirectX::SpriteBatch* spriteBatch)
{
    Entity::Render(spriteBatch);
    if (m_boomerang1) {
        m_boomerang1->Render(spriteBatch);
    }
    if (m_boomerang2) {
        m_boomerang2->Render(spriteBatch);
    }
}

void BoomerangBro::OnCollision(const CollisionResult& event)
{
	Mario* mario = dynamic_cast<Mario*>(event.collidedWith);
	Block* block = dynamic_cast<Block*>(event.collidedWith);
    Boomerang* boomerang = dynamic_cast<Boomerang*>(event.collidedWith);


	if (event.contactNormal.y < 0 && block)
	{
		Vector2 vel = GetVelocity();
		//vel += event.contactNormal * Vector2(std::abs(vel.x), std::abs(vel.y)) * (1.0f - event.contactTime);
		vel.y = 0.0f;
		SetVelocity(vel);
		return;
	}

	if (event.contactNormal.y > 0) // Collision from above
	{
		if (mario) 
        {
				Die(DyingType::STOMPED);

				Vector2 vel = mario->GetVelocity();
				vel.y = GameConfig::Mario::BOUNCE_FORCE;
				mario->SetVelocity(vel);
		}
		return;
	}

    if(boomerang)
    {
        if(m_boomerang1 == boomerang)
        {
            boomerang->Deactivate();
            OnBoomerangCaught(m_boomerang1);
        }
        else if(m_boomerang2 == boomerang)
        {
            boomerang->Deactivate();
            OnBoomerangCaught(m_boomerang2);
        }
    }

}

void BoomerangBro::Hold()
{   
    if (m_boomerang1 && m_boomerang1->GetState() == BoomerangState::HOLDING) {
        m_boomerang1->UpdatePosition(GetPosition());
    }
    if (m_boomerang2 && m_boomerang2->GetState() == BoomerangState::HOLDING) {
        m_boomerang2->UpdatePosition(GetPosition());
    }
}

void BoomerangBro::Throw()
{
    if (m_boomerang1 && m_boomerang1->GetState() == BoomerangState::HOLDING){
        Log(LOG_INFO, "BoomerangBro throwing boomerang 1");
        m_boomerang1->SetState(BoomerangState::OUTWARD);
    }
    if (m_boomerang2 && m_boomerang2->GetState() == BoomerangState::HOLDING) {
        Log(LOG_INFO, "BoomerangBro throwing boomerang 2");
        m_boomerang2->SetState(BoomerangState::OUTWARD);
    }
}

void BoomerangBro::Jump()
{
    if (m_isGrounded) {
        Vector2 vel = GetVelocity();
        vel.y = -150.0f; // Apply jump force
        SetVelocity(vel);
        m_isGrounded = false; // Set grounded to false after jumping
    }
}

void BoomerangBro::UpdateBroOrientation(Mario* mario)
{
	if (mario == nullptr)
		return;

	Vector2 marioPos = mario->GetPosition();
	Vector2 broPos = GetPosition();

	bool shouldFlip = (marioPos.x > broPos.x);

	if (shouldFlip != m_isFlipped)
	{
		m_isFlipped = shouldFlip;
		if (GetAnimator())
		{
			GetAnimator()->SetFlipHorizontal(m_isFlipped);
		}
	}
}

void BoomerangBro::CreateBoomerangs()
{
    Game* game = Game::GetInstance();
    SpriteSheet* spriteSheet = game->GetSpriteSheet();

    // Always try to create boomerang1 first
    if (!m_boomerang1 && !m_boomerang2)
    {
        m_boomerang1 = new Boomerang(GetPosition(), Vector2(BOOMERANG_WIDTH, BOOMERANG_HEIGHT), spriteSheet);
        Game::GetInstance()->GetCurrentWorld()->AddEntity(m_boomerang1);
        Log(LOG_INFO, "Boomerang1 created");
    }
    // If boomerang1 exists and is active, create boomerang2
    else if (m_boomerang1 && !m_boomerang2)
    {
        m_boomerang2 = new Boomerang(GetPosition(), Vector2(BOOMERANG_WIDTH, BOOMERANG_HEIGHT), spriteSheet);
        Game::GetInstance()->GetCurrentWorld()->AddEntity(m_boomerang2);
        Log(LOG_INFO, "Boomerang2 created while boomerang1 is active");
    }
}

void BoomerangBro::SetupCollisionComponent()
{
	Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x, curSize.y); // Make collision box slightly smaller
	m_collisionComponent->SetSize(newSize);

	m_collisionComponent->SetPosition(GetPosition() + Vector2(newSize.x / 2, newSize.y / 2));

	m_collisionComponent->SetSize(newSize); // Make collision box smaller than visual size
}

void BoomerangBro::OnBoomerangCaught(Boomerang* boomerang)
{
    // Clear the appropriate pointer to prevent access violation
    if (m_boomerang1 == boomerang) {
        m_boomerang1 = nullptr;
        Log(LOG_INFO, "Boomerang1 caught and deactivated");
    }
    if (m_boomerang2 == boomerang) {
        m_boomerang2 = nullptr;
        Log(LOG_INFO, "Boomerang2 caught and deactivated");
    }
}

bool BoomerangBro::CheckAABBCollision(const Entity* entityA, const Entity* entityB)
{
    // Get positions and sizes of both entities
    Vector2 positionA = entityA->GetPosition();
    Vector2 sizeA = entityA->GetSize();

    Vector2 positionB = entityB->GetPosition();
    Vector2 sizeB = entityB->GetSize();

    // Create bounding boxes
    Rectangle boxA = { long(positionA.x), long(positionA.y), long(sizeA.x), long(sizeA.y) };
    Rectangle boxB = { long(positionB.x), long(positionB.y), long(sizeB.x), long(sizeB.y) };

    // Check for intersection
    return (boxA.x < boxB.x + boxB.width &&
            boxA.x + boxA.width > boxB.x &&
            boxA.y < boxB.y + boxB.height &&
            boxA.y + boxA.height > boxB.y);
}
