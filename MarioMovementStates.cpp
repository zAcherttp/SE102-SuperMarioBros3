#include "pch.h"
#include "MarioMovementStates.h"
#include "Debug.h"
#include "AssetIDs.h"
#include "Mario.h"

using namespace DirectX;

// Physics constants converted from JavaScript
constexpr auto PLAYER_TOPWALKSPEED = 1.5f * 60.0f;      // topSpeed when not running
constexpr auto PLAYER_TOPRUNSPEED = 2.5f * 60.0f;       // topSpeed when running
constexpr auto PLAYER_MAXSPEED = 4.0f * 60.0f;          // Maximum allowed speed

// Jump force values from JS array converted to C++ (scaled by 60fps)
constexpr float PLAYER_JUMP_FORCE[] = {-3.5f * 60.0f, -3.625f * 60.0f, -3.75f * 60.0f, -4.0f * 60.0f};

// Acceleration values
constexpr auto PLAYER_ACCEL_NORMAL = 14.0f / 256.0f * 60.0f * 60.0f;  // Normal acceleration 
constexpr auto PLAYER_ACCEL_FRIC = 20.0f / 256.0f * 60.0f * 60.0f;    // Friction deceleration
constexpr auto PLAYER_ACCEL_SKID = 32.0f / 256.0f * 60.0f * 60.0f;    // Skidding deceleration

// Gravity values
constexpr auto PLAYER_GRAVITY_SLOW = 1.0f / 16.0f * 60.0f * 60.0f;   // Slow gravity when holding jump
constexpr auto PLAYER_GRAVITY_FAST = 5.0f / 16.0f * 60.0f * 60.0f;   // Fast gravity when not holding jump

void MarioStateBase::SetAnimation(Mario* mario, int animId) const {
    mario->SetAnimId(mario->GetAnimId() + animId);
    mario->SetAnimation(mario->GetAnimId(), true);
    //Log(__FUNCTION__, "Mario " + GetStateName() + ", anim id: " + std::to_string(mario->GetAnimId()));
}
std::string MarioIdleState::GetStateName() const { return "idle"; }
std::string MarioWalkState::GetStateName() const { return "walk"; }
std::string MarioRunState::GetStateName() const { return "run"; }
std::string MarioSkidState::GetStateName() const { return "skid"; }
std::string MarioJumpState::GetStateName() const { return "jump"; }
std::string MarioSitState::GetStateName() const { return "sit"; }

void MarioStateBase::Enter(Mario* mario) {
    Log("State Machine", "Entering state: " + GetStateName());
}

void MarioIdleState::Enter(Mario* mario)
{ 
	SetAnimation(mario, ID_ANIM_MARIO_IDLE);
}

void MarioIdleState::Exit(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() - ID_ANIM_MARIO_IDLE);
}
void MarioWalkState::Enter(Mario* mario)
{
	SetAnimation(mario, ID_ANIM_MARIO_WALK);
}

void MarioWalkState::Exit(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() - ID_ANIM_MARIO_WALK);
}void MarioRunState::Enter(Mario* mario)
{
	SetAnimation(mario, ID_ANIM_MARIO_WALK);
}

void MarioRunState::Exit(Mario* mario)
{
    // walk because it only changes to run when speed is enough
	mario->SetAnimId(mario->GetAnimId() - ID_ANIM_MARIO_WALK);
}
void MarioSkidState::Enter(Mario* mario)
{
	SetAnimation(mario, ID_ANIM_MARIO_SKID);
}

void MarioSkidState::Exit(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() - ID_ANIM_MARIO_SKID);
}
void MarioJumpState::Enter(Mario* mario)
{
    // Determine jump force based on current horizontal speed
    float absSpeed = std::abs(mario->GetVelocity().x);
    int speedIndex = std::min(3, (int)std::floor(absSpeed / 60.0f));

    // Set initial jump velocity based on horizontal speed
    mario->SetVelocity(Vector2(mario->GetVelocity().x, PLAYER_JUMP_FORCE[speedIndex]));
    
    // Set animation
    SetAnimation(mario, ID_ANIM_MARIO_JUMP);
}

void MarioJumpState::Exit(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() - ID_ANIM_MARIO_JUMP);
}
void MarioSitState::Enter(Mario* mario)
{
	SetAnimation(mario, ID_ANIM_MARIO_SIT);
}

void MarioSitState::Exit(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() - ID_ANIM_MARIO_SIT);
}

void MarioMovementState::Update(Mario* mario, float dt)
{
    // Apply universal forces on mario based on grounded state
    Vector2 vel = mario->GetVelocity();
	auto input = mario->GetInput();
    
    if (input->isRightDown)
        mario->SetDirection(1);
    if (input->isLeftDown)
        mario->SetDirection(-1);

    // If in air, apply appropriate gravity
    if (!mario->IsGrounded()) {
        // Determine gravity based on jump button and upward momentum
        float gravity = (vel.y < -2.0f * 60.0f && input->isADown)
            ? PLAYER_GRAVITY_SLOW 
            : PLAYER_GRAVITY_FAST;

        mario->SetVelocity(vel + Vector2(0, gravity * dt));
    }

    // Ensure mario's velocity doesn't exceed maximum speed
    vel = mario->GetVelocity();
    if (vel.x > PLAYER_MAXSPEED) vel.x = PLAYER_MAXSPEED;
    if (vel.x < -PLAYER_MAXSPEED) vel.x = -PLAYER_MAXSPEED;
    
    // Update position based on velocity
    mario->SetPosition(mario->GetPosition() + vel * dt);
}

MarioMovementState* MarioIdleState::HandleInput(Mario* mario)
{
    auto input = mario->GetInput();

    if (input->isLeftDown && input->isRightDown == false) {
        return MarioStateFactory::CreateState(MarioMovementStateType::Walk, Direction::Left);
    }
    if (input->isRightDown && input->isLeftDown == false) {
        return MarioStateFactory::CreateState(MarioMovementStateType::Walk, Direction::Right);
    }
    if (input->isDownDown && mario->IsGrounded()) {
        return MarioStateFactory::CreateState(MarioMovementStateType::Sit, GetDirection());
    }
    if (input->isAPressed && mario->IsGrounded()) {
        return MarioStateFactory::CreateState(MarioMovementStateType::Jump, GetDirection());
    }

    return nullptr;
}

void MarioIdleState::Update(Mario* mario, float dt)
{
    Vector2 vel = mario->GetVelocity();

    if (vel.x < 0) {
        vel.x += PLAYER_ACCEL_FRIC * dt;
        if (vel.x > 0) vel.x = 0;
    } else if (vel.x > 0) {
        vel.x -= PLAYER_ACCEL_FRIC * dt;
        if (vel.x < 0) vel.x = 0;
}

mario->SetVelocity(vel);

MarioMovementState::Update(mario, dt);

	MarioMovementState::Update(mario, dt);
}

MarioMovementState* MarioWalkState::HandleInput(Mario* mario)
{
    auto input = mario->GetInput();


	if (input->isAPressed) {
		if (mario->IsGrounded()) {
			return MarioStateFactory::CreateState(MarioMovementStateType::Jump, GetDirection());
		}
	}

	if (input->isLeftDown && input->isRightDown == false) {
	    if (GetDirection() == Direction::Right && mario->IsGrounded()) {
	 	    return MarioStateFactory::CreateState(MarioMovementStateType::Skid, Direction::Right);
	    }
	}

	if (input->isRightDown && input->isLeftDown == false) {
	    if (GetDirection() == Direction::Left && mario->IsGrounded()) {
	 	    return MarioStateFactory::CreateState(MarioMovementStateType::Skid, Direction::Left);
	    }
	}

	if (input->isBDown) {
		return MarioStateFactory::CreateState(MarioMovementStateType::Run, GetDirection());
	}

	if (mario->GetVelocity().Length() == 0 && mario->IsGrounded()) {
		return MarioStateFactory::CreateState(MarioMovementStateType::Idle, GetDirection());
	}

	return nullptr;
}

void MarioWalkState::Update(Mario* mario, float dt)
{
    Vector2 vel = mario->GetVelocity();
    bool isGrounded = mario->IsGrounded();
    auto input = mario->GetInput();

	bool isLeftDown = input->isLeftDown;
	bool isRightDown = input->isRightDown;

    // Handle horizontal movement
    if (isLeftDown != isRightDown) {
        Direction hitDir = isLeftDown ? Direction::Left : Direction::Right;
        float absDX = std::abs(vel.x);

        if (absDX < PLAYER_TOPWALKSPEED) {
            vel.x += (int)hitDir * PLAYER_ACCEL_NORMAL * dt;
        }
        // Slow down if exceeding walk speed (when transitioning from run to walk)
        else if (absDX > PLAYER_TOPWALKSPEED && isGrounded) {
            vel.x -= (int)hitDir * PLAYER_ACCEL_FRIC * dt;
        }
    } 
    // No direction input, apply friction
    else if (isGrounded) {
        if (vel.x < 0) {
            vel.x += PLAYER_ACCEL_FRIC * dt;
            if (vel.x > 0) vel.x = 0;
        } else if (vel.x > 0) {
            vel.x -= PLAYER_ACCEL_FRIC * dt;
            if (vel.x < 0) vel.x = 0;
        }
    }
    
    mario->SetVelocity(vel);
    
    MarioMovementState::Update(mario, dt);
}

MarioMovementState* MarioRunState::HandleInput(Mario* mario)
{
    auto input = mario->GetInput();

	if (input->isAPressed && mario->IsGrounded()) {
		return MarioStateFactory::CreateState(MarioMovementStateType::Jump, GetDirection());
	}
	if (input->isLeftDown && input->isRightDown == false) {
		if (GetDirection() == Direction::Right && mario->IsGrounded()) {
			return MarioStateFactory::CreateState(MarioMovementStateType::Skid, Direction::Right);
		}
	}

	if (input->isRightDown && input->isLeftDown == false) {
		if (GetDirection() == Direction::Left && mario->IsGrounded()) {
			return MarioStateFactory::CreateState(MarioMovementStateType::Skid, Direction::Left);
		}
	}

	if (!input->isBDown) {
		return MarioStateFactory::CreateState(MarioMovementStateType::Walk, GetDirection());
	}
	return nullptr;
}

void MarioRunState::Update(Mario* mario, float dt)
{
    auto input = mario->GetInput();

    Vector2 vel = mario->GetVelocity();
    bool isGrounded = mario->IsGrounded();
    
    // Get current direction input
    bool isLeftDown = input->isLeftDown;
    bool isRightDown = input->isRightDown;
    
    if (isLeftDown != isRightDown) {
        Direction hitDir = isLeftDown ? Direction::Left : Direction::Right;
        float absDX = std::abs(vel.x);
        
        // Accelerate up to run speed
        if (absDX < PLAYER_TOPRUNSPEED) {
            vel.x += (int)hitDir * PLAYER_ACCEL_NORMAL * dt;
        }
        // Maintain run speed
        else if (absDX > PLAYER_TOPRUNSPEED && isGrounded) {
            // No slowing down in run state unless switching directions
            if ((vel.x > 0 && hitDir == Direction::Right)
                || (vel.x < 0 && hitDir == Direction::Left)) {
                // Maintain current speed
            }
            } else {
                // Slow down if changing direction
                vel.x -= (vel.x > 0 ? 1 : -1) * PLAYER_ACCEL_FRIC * dt;
            }
        }
    // No direction input, apply friction
    else if (isGrounded) {
        if (vel.x < 0) {
            vel.x += PLAYER_ACCEL_FRIC * dt;
            if (vel.x > 0) vel.x = 0;
        } else if (vel.x > 0) {
            vel.x -= PLAYER_ACCEL_FRIC * dt;
            if (vel.x < 0) vel.x = 0;
        }
    }
    
    mario->SetVelocity(vel);
    
    MarioMovementState::Update(mario, dt);
}

MarioMovementState* MarioSkidState::HandleInput(Mario* mario)
{
    auto input = mario->GetInput();

	if (mario->GetVelocity().x == 0 && GetDirection() != m_lastDir) {
		return MarioStateFactory::CreateState(MarioMovementStateType::Walk, GetDirection());
	}
	
	return nullptr;
}

void MarioSkidState::Update(Mario* mario, float dt)
{
    Vector2 vel = mario->GetVelocity();
    
    // Apply stronger friction during skid
    vel.x -= PLAYER_ACCEL_SKID * (int)m_lastDir * dt;
    
    // Check if we've fully stopped or changed direction
    if ((m_lastDir == Direction::Right && vel.x <= 0) || (m_lastDir == Direction::Left && vel.x >= 0)) {
        vel.x = 0;
        m_dir = (m_lastDir == Direction::Right) ? Direction::Left : Direction::Right;
    }
    
    mario->SetVelocity(vel);
    
    MarioMovementState::Update(mario, dt);
}

MarioMovementState* MarioJumpState::HandleInput(Mario* mario)
{
    auto input = mario->GetInput();
    auto vel = mario->GetVelocity();
    auto absVelX = std::abs(vel.x);

    if (mario->IsGrounded() && absVelX < PLAYER_TOPWALKSPEED || absVelX >= PLAYER_TOPWALKSPEED && !input->isBDown) {
        return MarioStateFactory::CreateState(MarioMovementStateType::Walk, GetDirection());
    }
    if (mario->IsGrounded() && absVelX >= PLAYER_TOPWALKSPEED && input->isBDown) {
        return MarioStateFactory::CreateState(MarioMovementStateType::Run, GetDirection());
    }

	return nullptr;
}

void MarioJumpState::Update(Mario* mario, float dt)
{
	auto input = mario->GetInput();
    Vector2 vel = mario->GetVelocity();

    // Handle horizontal movement in air
    bool isLeftDown = input->isLeftDown;
    bool isRightDown = input->isRightDown;
    
    if (isLeftDown != isRightDown) {
        Direction hitDir = isLeftDown ? Direction::Left : Direction::Right;
        
        float speedCap = input->isBDown ? PLAYER_TOPRUNSPEED : PLAYER_TOPWALKSPEED;

        // Apply air control - same speed and accel as walk
        vel.x += (int)hitDir * PLAYER_ACCEL_NORMAL * dt;

        if(std::abs(vel.x) > speedCap) {
            vel.x -= (int)hitDir * PLAYER_ACCEL_FRIC * dt;
        }
    }
    
    mario->SetVelocity(vel);
    
    MarioMovementState::Update(mario, dt);
}

MarioMovementState* MarioSitState::HandleInput(Mario* mario)
{
	auto input = mario->GetInput();

	if (!input->isADown) {
		return new MarioIdleState(GetDirection());
	}
	return nullptr;
}

void MarioSitState::Update(Mario* mario, float dt)
{
	MarioMovementState::Update(mario, dt);
}

Direction MarioStateBase::GetDirection() const
{
    return m_dir;
}

MarioMovementState* MarioStateFactory::CreateState(MarioMovementStateType type, Direction direction) {
    switch (type) {
    case MarioMovementStateType::Idle: return new MarioIdleState(direction);
    case MarioMovementStateType::Walk: return new MarioWalkState(direction);
    case MarioMovementStateType::Run: return new MarioRunState(direction);
    case MarioMovementStateType::Jump: return new MarioJumpState(direction);
    case MarioMovementStateType::Skid: return new MarioSkidState(direction);
    case MarioMovementStateType::Sit: return new MarioSitState(direction);
    default: return new MarioIdleState(direction);
    }
}

MarioPowerupState* MarioStateFactory::CreateState(MarioPowerupStateType type, Direction direction)
{
    return nullptr;
}