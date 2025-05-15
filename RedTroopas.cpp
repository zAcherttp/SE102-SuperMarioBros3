#include "pch.h"
#include "RedTroopas.h"
#include "Debug.h"
#include "AssetIDs.h"
#include "Mario.h"
#include "MarioStateFactory.h"
#include "GameConfig.h"
#include "ParaGoomba.h"
#include "Block.h"
#include "Goomba.h"

RedTroopas::RedTroopas(Vector2 position, Vector2 size, SpriteSheet* spriteSheet)
    : Entity(position,size, spriteSheet)
    , m_animTimer(0.0f)
    , m_frameTime(0.15f)
    , m_flipFrame(false)
    , m_lastDirectionX(-1.0f)
    , m_isColliding(false)
    , m_reviveTimer(0.0f)
    , m_reviveTime(5.0f)
    , m_isVibrating(false)
    , m_vibrateTimer(0.0f)
    , m_vibrateInterval(0.0f)
    , m_vibrateAmplitude(0.0f)
    , m_vibrateCount(0)
    , m_maxVibrateCount(0)

{
    SetAnimation(ID_ANIM_RED_TROOPAS_WALK, true);
    m_visible = true; // Set visibility to true
    m_isDying = false; // Initialize dying state
    m_state = WALKING; // Initialize state to WALKING
    
    // Set initial velocity - slow movement to the left
    SetVelocity(Vector2(-GameConfig::Enemies::Troopas::WALK_SPEED, 0.0f));

    // Setup collision component
    SetupCollisionComponent();
    
    // Initialize sprite direction based on initial velocity
    UpdateSpriteDirection();
    
    // Initialize interaction point states to ensure edge detection works properly
    m_pointCollisionState[InteractionPointType::LeftFoot] = true;
    m_pointCollisionState[InteractionPointType::RightFoot] = true;
    
}

void RedTroopas::Render(DirectX::SpriteBatch* spriteBatch)
{
    if(m_bonkEffects.size() > 0) {
        for (auto& effect : m_bonkEffects) {
            effect->Render(spriteBatch);
        }
    }
    Entity::Render(spriteBatch);
}

void RedTroopas::Die(DyingType type)
{

}


void RedTroopas::Update(float dt)
{

    if (m_isVibrating) {
        UpdateVibration(dt);
        Entity::Update(dt);
        return;
    }

    if (m_state == SHELL_IDLE) {
        m_reviveTimer += dt;
        if (m_reviveTimer >= 5.0f && !m_isVibrating) {  
            StartVibration();
            m_reviveTimer = 0.0f;
        }
    } else {

        m_reviveTimer = 0.0f;
    }

    m_isGrounded = Collision::GetInstance()->GroundCheck(this, dt);
    
    // Check for platform edges using raycasting if the entity is grounded
    if (m_isGrounded && m_state == WALKING) {
        CheckEdge(); // Use our new raycast-based edge detection
    }

    if (!m_isGrounded) {
        Vector2 vel = GetVelocity();
        vel.y += GameConfig::Physics::GRAVITY * dt;
        SetVelocity(vel);
    }

    UpdateSpriteDirection();
    
    SetPosition(GetPosition() + GetVelocity() * dt);

    if(!m_bonkEffects.empty()) {
        for (auto& effect : m_bonkEffects) {
            effect->Update(dt);
        }
    }
    Entity::Update(dt);
}


void RedTroopas::OnCollision(const CollisionResult& event)
{
    Mario* mario = dynamic_cast<Mario*>(event.collidedWith);    
    Goomba* goomba = dynamic_cast<Goomba*>(event.collidedWith);
    Block* block = dynamic_cast<Block*>(event.collidedWith);  
    Entity* entity = dynamic_cast<Entity*>(event.collidedWith);

    if(event.collidedWith->GetCollisionGroup() == CollisionGroup::NONSOLID && event.contactNormal.x != 0) {
        return;
    }
    
    if (event.contactNormal.x != 0)  // side collision
    {
        if(block)
        { 
            if(block->IsSolid()) 
            {
                float targetSpeed = GameConfig::Enemies::Goomba::WALK_SPEED;
                Vector2 vel = GetVelocity();
                vel.x = -vel.x;
                SetVelocity(vel);
            return;
            }
        }

        if(mario) {
            if (m_state == SHELL_IDLE) {
                if(event.contactNormal.x > 0) {
                    SetVelocity(Vector2(GameConfig::Enemies::Troopas::WALK_SPEED*7, 0.0f));
                } else {
                    SetVelocity(Vector2(-GameConfig::Enemies::Troopas::WALK_SPEED*7, 0.0f));
                }
                m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_SHELL_SLIDE, true);
                m_state = SHELL_SLIDE;
                return;
            }
        }

        if(entity)
        {
            if(m_state == SHELL_SLIDE)
            {
                entity->Die(DyingType::BONKED);
                Game* game = Game::GetInstance();
                SpriteSheet* spriteSheet = game->GetSpriteSheet();
                EffectManager::GetInstance()->CreateBonkEffect(entity->GetPosition());
            }
        }

    }
 
    if(event.contactNormal.y < 0 && block) // foot collision
    { 
        Block* block = dynamic_cast<Block*>(event.collidedWith);
        if (!block) {
            return;  // Only consider solid blocks for platform edge detection
        }
    
        Vector2 vel = GetVelocity();
        vel.y = 0.0f;  
        SetVelocity(vel);
        m_isGrounded = true;
        return;
    }

   if (event.contactNormal.y > 0) // Collision from above
   {
    if (mario) {
        if(m_state == WALKING) {
            Vector2 vel = mario->GetVelocity();
            vel.y = GameConfig::Mario::BOUNCE_FORCE; // Use Mario's bounce force
            mario->SetVelocity(vel);
            TransformToShell();
            return;
        }
        if(m_state == SHELL_IDLE) {
            
            if(mario->GetPosition().x < GetPosition().x + 8.0f) {
                SetVelocity(Vector2(GameConfig::Enemies::Troopas::WALK_SPEED*7, 0.0f));
            } else {
                SetVelocity(Vector2(-GameConfig::Enemies::Troopas::WALK_SPEED*7, 0.0f));
            }
             
            Vector2 vel = mario->GetVelocity();
            vel.y = GameConfig::Mario::BOUNCE_FORCE; // Use Mario's bounce force
            mario->SetVelocity(vel);

            m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_SHELL_SLIDE, true);
            m_state = SHELL_SLIDE;
            return;
        }
        if(m_state == SHELL_SLIDE) {
            Vector2 vel = mario->GetVelocity();
            vel.y = GameConfig::Mario::BOUNCE_FORCE; // Use Mario's bounce force
            mario->SetVelocity(vel);

            m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_SHELL);
            SetVelocity(Vector2(0.0f, 0.0f));
            m_state = SHELL_IDLE;
            return;
        }

        if(block) {
            if(block->IsSolid()) {
                Vector2 vel = GetVelocity();
                vel.y = 0.0f;
                SetVelocity(vel);
            }
            return;
        }
    }
    }

    else if (mario && !m_isDying) {
    }
}

bool RedTroopas::CheckEdge()
{
    if (m_state != WALKING || !m_isGrounded ) {
        return false;
    }
    
    Vector2 position = GetPosition();
    Vector2 size = GetSize();
    Vector2 velocity = GetVelocity();
    
    // Cast rays downward from left and right edges of RedTroopas
    Vector2 leftFootPos = position + Vector2(-size.x / 4, size.y / 2 - 3.0f);
    Vector2 rightFootPos = position + Vector2(size.x / 4, size.y / 2 -3.0f);
    
    // Ray length - slightly longer than needed to detect edges
    float rayLength = 5.0f;
    
    // Determine which ray to check based on movement direction
    Vector2 rayStartPos;
    if (velocity.x < 0) {
        // Moving left, check left foot
        rayStartPos = leftFootPos;
    } else {
        // Moving right, check right foot
        rayStartPos = rightFootPos;
    }
    
    Vector2 rayEndPos = rayStartPos + Vector2(0, rayLength);
    
    // Get cells around the entity for collision check
    std::vector<std::pair<int, int>> cells = Collision::GetInstance()->GetEntityCells(this, 0.016f);
    
    // Flag to track if we found ground under the ray
    bool foundGround = false;
    
    // Check for collisions with the ray
    for (const auto& cell : cells) {
        auto& gridCell = Collision::GetInstance()->GetGrid()[cell.first][cell.second];
        for (Entity* other : gridCell.entities) {
            // Skip if it's the same entity or not a potential ground
            if (other == this || !other->IsStatic() || !other->IsActive() || !other->IsCollidable()) {
                continue;
            }
            
            // Get the other entity's collision rectangle
            Rectangle otherRect = other->GetCollisionComponent()->GetRectangle();

         if (rayStartPos.y > otherRect.y) continue;
            
            // Cast a ray downward
            Vector2 contactPoint, contactNormal;
            float contactTime;
            
            if (Collision::GetInstance()->RayVsRect(rayStartPos, rayEndPos, otherRect, 
                                                  contactPoint, contactNormal, contactTime)) {
                if (contactTime < 1.0f) {
                    // We hit something, which means there's ground under the foot
                    foundGround = true;
                    break;
                }
            }
        }
        
        if (foundGround) {
            break;
        }
    }
    
    // If no ground was found under the ray, we're at an edge
    if (!foundGround) {
        // Change direction
        Vector2 vel = GetVelocity();
        vel.x = -vel.x;
        SetVelocity(vel);
        UpdateSpriteDirection();
        return true;
    }
    
    return false;
}

void RedTroopas::SetupCollisionComponent()
{
    Vector2 curSize = m_collisionComponent->GetSize();
	Vector2 newSize = Vector2(curSize.x, curSize.y);
	m_collisionComponent->SetSize(newSize);

	m_collisionComponent->SetPosition(GetPosition() + Vector2(newSize.x / 2 , newSize.y / 2));
}

void RedTroopas::UpdateSpriteDirection()
{
    Vector2 vel = GetVelocity();
    
    if (vel.x != 0 && ((vel.x > 0) != (m_lastDirectionX > 0)))
    {
        if (GetAnimator())
        {

            GetAnimator()->SetFlipHorizontal(vel.x > 0);

        }

        m_lastDirectionX = vel.x;
    }
}

void RedTroopas::TransformToShell()
{
    if (m_state == WALKING) {
        m_state = SHELL_IDLE;
    }

	Vector2 newSize = Vector2(16.0f, 16.0f);
	m_collisionComponent->SetSize(newSize);
	m_collisionComponent->SetPosition(GetPosition());
    SetPosition(GetPosition() + Vector2(0.0f, 5.0f));
    SetVelocity(Vector2(0.0f, 0.0f));

    m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_SHELL);

}

void RedTroopas::TransformToTroopa()
{
    if (m_state == SHELL_IDLE) {
        m_state = WALKING;
    }
    Mario* m_mario = dynamic_cast<Mario*>(World::GetInstance()->GetPlayer());

    Vector2 newSize = Vector2(16.0f, 27.0f);
    m_collisionComponent->SetSize(newSize);
    m_collisionComponent->SetPosition(GetPosition() - Vector2(0.0f, 5.0f));
    
    if(m_mario) {
        Vector2 marioPos = m_mario->GetPosition();
        Vector2 goombaPos = GetPosition();
        
        if (marioPos.x < goombaPos.x + 8.0f) {
            SetVelocity(Vector2(-GameConfig::Enemies::Troopas::WALK_SPEED, 0.0f));
        } else {
            SetVelocity(Vector2(GameConfig::Enemies::Troopas::WALK_SPEED, 0.0f));
        }
    }
    m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_WALK, true);
}

void RedTroopas::StartVibration() {
    if (!m_isVibrating) {
        m_isVibrating = true;
        m_vibrateTimer = 0.0f;
        m_vibrateInterval = 0.03f;        
        m_vibrateAmplitude = 1.0f;        
        m_vibrateCount = 0;
        m_maxVibrateCount = 20;          
        m_originalPosition = GetPosition(); 
        m_vibrateDirection = true;
        m_firstRevivePhase = false;
        m_secondRevivePhase = false;
        

        m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_REVIVE_SLOW, false);
   
        m_reviveTimer = 0.0f;
        m_reviveTime = 1.77f; 
    }
}

void RedTroopas::UpdateVibration(float dt) {
    if (!m_isVibrating) return;
    
    m_vibrateTimer += dt;
    m_reviveTimer += dt;
    
   
    if (m_vibrateTimer >= m_vibrateInterval) {
        m_vibrateTimer = 0.0f;
        m_vibrateCount++;
        
        float offsetX = m_vibrateDirection? -m_vibrateAmplitude : 0;

        m_vibrateDirection = !m_vibrateDirection;

        SetPosition(m_originalPosition + Vector2(offsetX, 0.0f));
    
    }

    if (m_reviveTimer > 0.5f && m_reviveTimer < 1.12f &&!m_firstRevivePhase) {
        m_firstRevivePhase = true;
        m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_REVIVE_SLOW, true);
    }

    if (m_reviveTimer >= 1.12f && !m_secondRevivePhase) {
        m_secondRevivePhase = true;
        m_animator->SetAnimation(ID_ANIM_RED_TROOPAS_REVIVE_FAST, true);
    }
    
    if (m_reviveTimer >= m_reviveTime) {
        SetPosition(m_originalPosition);
        m_isVibrating = false;
        TransformToTroopa();
    }
}