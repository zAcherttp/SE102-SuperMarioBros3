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
    , m_frameTime(0.15f)  // Controls how fast the flip animation occurs
    , m_flipFrame(false)
    , m_lastDirectionX(-1.0f)  // Initialize to match initial velocity direction (left)
    , m_isColliding(false)
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
    
    Log(__FUNCTION__, "Collision component initialized");
}

void RedTroopas::Render(DirectX::SpriteBatch* spriteBatch)
{
    Entity::Render(spriteBatch);
}

void RedTroopas::Die()
{

}

void RedTroopas::Update(float dt)
{
    // Update direction change cooldown
    ResetInteractionPoints();
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
    Entity::Update(dt);
}


void RedTroopas::OnCollision(const CollisionResult& event)
{
    Mario* mario = dynamic_cast<Mario*>(event.collidedWith);    
    Goomba* goomba = dynamic_cast<Goomba*>(event.collidedWith);
    Block* block = dynamic_cast<Block*>(event.collidedWith);  

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
        
        
        Log(__FUNCTION__, "Edge detected, changing direction");
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
    
    // Only update if there's horizontal movement and the direction has changed
    // This ensures we only flip the sprite when actually changing direction
    if (vel.x != 0 && ((vel.x > 0) != (m_lastDirectionX > 0)))
    {
        // Update the animator's flip state based on direction
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

bool RedTroopas::UsesInteractionPoints() const
{
    return false;  // Disable interaction points for RedTroopas
}

std::vector<std::pair<InteractionPointType, Vector2>> RedTroopas::GetInteractionPoints() const
{
    std::vector<std::pair<InteractionPointType, Vector2>> points;
    Vector2 size = GetCollisionComponent()->GetSize();
    
    points.push_back({ InteractionPointType::LeftFoot, Vector2(-size.x / 4 , size.y / 2 - 1.0f) });
    points.push_back({ InteractionPointType::RightFoot, Vector2(size.x / 4 , size.y / 2 - 1.0f) });
    points.push_back({ InteractionPointType::LeftUpper, Vector2(-size.x / 2 , -size.y / 4) });
    points.push_back({ InteractionPointType::RightUpper, Vector2(size.x / 2 , -size.y / 4) });
    points.push_back({ InteractionPointType::TopHead, Vector2(0.0f , -size.y / 2) });
    points.push_back({ InteractionPointType::LeftLower, Vector2(-size.x / 2 , size.y / 4) });
    points.push_back({ InteractionPointType::RightLower, Vector2(size.x / 2 , size.y / 4) });
    
    return points;
}


// void RedTroopas::OnFootCollision(const CollisionResult& event)
// {
//     // Make sure it's an actual ground collision
//     Block* block = dynamic_cast<Block*>(event.collidedWith);
//     if (!block) {
//         return;  // Only consider solid blocks for platform edge detection
//     }

//     Vector2 vel = GetVelocity();
//     vel.y = 0.0f;  
//     SetVelocity(vel);
//     m_isGrounded = true;
//     m_isColliding = true;
    
//     // Update the appropriate foot collision state
//     if (event.pointType == InteractionPointType::LeftFoot) {
//         UpdateInteractionPointState(InteractionPointType::LeftFoot, true);
//     } 
//     else if (event.pointType == InteractionPointType::RightFoot) {
//         UpdateInteractionPointState(InteractionPointType::RightFoot, true);
//     }
// }

// void RedTroopas::OnTopHeadCollision(const CollisionResult& event)
// {
//     Mario* mario = dynamic_cast<Mario*>(event.collidedWith);
//     Block* block = dynamic_cast<Block*>(event.collidedWith);
//     if (mario) {
//         if(m_state == WALKING) {
//             TransformToShell();
//             Vector2 vel = mario->GetVelocity();
//             vel.y = GameConfig::Mario::BOUNCE_FORCE; // Use Mario's bounce force
//             mario->SetVelocity(vel);
//         }

//         if(block) {
//             if(block->IsSolid()) {
//                 Vector2 vel = GetVelocity();
//                 vel.y = 0.0f;
//                 SetVelocity(vel);
//             }
//         }
//         return;
//     }
// }

// void RedTroopas::OnLeftSideCollision(const CollisionResult& event)
// {
//     Block* block = dynamic_cast<Block*>(event.collidedWith);

//     if(event.collidedWith->GetCollisionGroup() == CollisionGroup::NONSOLID && event.contactNormal.x != 0) {
//         return;
//     }
    
//     if (event.contactNormal.x != 0 && block) 
//     {
//         if(block->IsSolid()) 
//         {
//             float targetSpeed = GameConfig::Enemies::Goomba::WALK_SPEED;
//             Vector2 vel = GetVelocity();
//             vel.x = -vel.x;
//             SetVelocity(vel);
//         }
//         return;
//     }
// }

// void RedTroopas::OnRightSideCollision(const CollisionResult& event)
// {
//     Block* block = dynamic_cast<Block*>(event.collidedWith);

//     if(event.collidedWith->GetCollisionGroup() == CollisionGroup::NONSOLID && event.contactNormal.x != 0) {
//         return;
//     }
    
//     if (event.contactNormal.x != 0 && block) 
//     {
//         if(block->IsSolid()) 
//         {
//             float targetSpeed = GameConfig::Enemies::Goomba::WALK_SPEED;
//             Vector2 vel = GetVelocity();
//             vel.x = -vel.x;
//             SetVelocity(vel);
//         }
//         return;
//     }
// }


// void RedTroopas::OnNoCollision(float dt, Axis axis)
// {
//     // if (m_state == WALKING && axis == Axis::Y && !m_isColliding) {
//     //     Vector2 vel = GetVelocity();
//     //     m_isGrounded = Collision::GetInstance()->GroundCheck(this, dt);

//     //     if (m_isGrounded ) {
//     //         // Only check edge if we didn't already change direction
//     //         if (!CheckEdge()) {
//     //             vel.x = -vel.x;
//     //             SetVelocity(vel);
//     //             UpdateSpriteDirection(); 
//     //         }
//     //     }
//     // }
// }

void RedTroopas::UpdateInteractionPointState(InteractionPointType type, bool isColliding) {
    m_pointCollisionState[type] = isColliding;
    // Limit the logging to reduce console spam
    if (type == InteractionPointType::LeftFoot || type == InteractionPointType::RightFoot) {
        Log(__FUNCTION__, "Point " + std::to_string(static_cast<int>(type)) + 
            " collision state: " + std::to_string(isColliding));
    }
}

void RedTroopas::ResetInteractionPoints() {

    if(m_isGrounded) {
        m_pointCollisionState[InteractionPointType::LeftFoot] = true;
        m_pointCollisionState[InteractionPointType::RightFoot] = true;
    }

    for (auto& [type, state] : m_pointCollisionState) {
            state = false;  // Reset all except foot collisions
    }
    
}

