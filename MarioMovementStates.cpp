#include "pch.h"
#include "MarioMovementStates.h"
#include "MarioStateFactory.h"
#include "Debug.h"
#include "AssetIDs.h"
#include "Mario.h"

using namespace DirectX;

// Physics constants
constexpr auto PLAYER_TOP_WALKSPEED = 1.5f * 60.0f;      // topSpeed when not running
constexpr auto PLAYER_TOP_RUNSPEED = 2.5f * 60.0f;       // topSpeed when running
constexpr auto PLAYER_TOP_SPRINTSPEED = 3.5f * 60.0f;       // topSpeed when sprinting
constexpr auto PLAYER_TOP_ENDOFLEVELSPEED = 1.25f * 60.0f;       // topSpeed when sprinting
constexpr auto PLAYER_TOP_SPEED = 4.0f * 60.0f;          // Maximum allowed speed

// Jump force values
constexpr float PLAYER_JUMP_FORCE[] = {-3.5f * 60.0f, -3.625f * 60.0f, -3.75f * 60.0f, -4.0f * 60.0f};

// Acceleration values
constexpr auto PLAYER_ACCEL_NORMAL = 14.0f / 256.0f * 60.0f * 60.0f;  // Normal acceleration 
constexpr auto PLAYER_ACCEL_FRIC = 20.0f / 256.0f * 60.0f * 60.0f;    // Friction deceleration
constexpr auto PLAYER_ACCEL_SKID = 32.0f / 256.0f * 60.0f * 60.0f;    // Skidding deceleration

// Gravity values
constexpr auto PLAYER_GRAVITY_SLOW = 1.0f / 16.0f * 60.0f * 60.0f;   // Slow gravity when holding jump
constexpr auto PLAYER_GRAVITY_FAST = 5.0f / 16.0f * 60.0f * 60.0f;   // Fast gravity when not holding jump

std::string MarioIdleState::GetStateName() const { return "idle"; }
std::string MarioWalkState::GetStateName() const { return "walk"; }
std::string MarioRunState::GetStateName() const { return "run"; }
std::string MarioSkidState::GetStateName() const { return "skid"; }
std::string MarioJumpState::GetStateName() const { return "jump"; }
std::string MarioSitState::GetStateName() const { return "sit"; }

int MarioIdleState::GetStateAnimValue() const { return ID_ANIM_MARIO_IDLE; }
int MarioWalkState::GetStateAnimValue() const { return ID_ANIM_MARIO_WALK; }
int MarioRunState::GetStateAnimValue() const { return ID_ANIM_MARIO_WALK; }
int MarioSkidState::GetStateAnimValue() const { return ID_ANIM_MARIO_SKID; }
int MarioJumpState::GetStateAnimValue() const { return ID_ANIM_MARIO_JUMP; }
int MarioSitState::GetStateAnimValue() const { return ID_ANIM_MARIO_SIT; }

Vector2 MarioIdleState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioWalkState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioRunState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioSkidState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioJumpState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioSitState::GetStateSizeOffset() const { return Vector2(0, 0); }

void MarioMovementState::Enter(Mario* mario) {
    mario->SetSize(mario->GetCurrentPStateSizeOffset() + mario->GetCurrentMStateSizeOffset());
    SetAnimation(mario, mario->GetCurrentPStateAnimValue() + mario->GetCurrentMStateAnimValue());
}

void MarioMovementState::Exit(Mario* mario) {
    mario->SetSize(mario->GetCurrentPStateSizeOffset() - mario->GetCurrentMStateSizeOffset());
    SetAnimation(mario, mario->GetCurrentPStateAnimValue() - mario->GetCurrentMStateAnimValue());
}

void MarioJumpState::Enter(Mario* mario)
{
    // Determine jump force based on current horizontal speed
    float absSpeed = std::abs(mario->GetVelocity().x);
    int speedIndex = std::min(3, (int)std::floor(absSpeed / 60.0f));

    // Set initial jump velocity based on horizontal speed
    mario->SetVelocity(Vector2(mario->GetVelocity().x, PLAYER_JUMP_FORCE[speedIndex]));
    
    // Set animation
    mario->SetSize(mario->GetCurrentPStateSizeOffset() + mario->GetCurrentMStateSizeOffset());
    SetAnimation(mario, mario->GetCurrentPStateAnimValue() + mario->GetCurrentMStateAnimValue());
}

void MarioMovementState::Update(Mario* mario, float dt)
{
    // Apply universal forces
    Vector2 vel = mario->GetVelocity();
	auto input = mario->GetInput();
    
    if (input->isRightDown)
        mario->SetDirection((int)Direction::Right);
    if (input->isLeftDown)
        mario->SetDirection((int)Direction::Left);

    // Determine gravity based on jump button and upward momentum
    if (!mario->IsGrounded())
    {
        float gravity = (vel.y < -2.0f * 60.0f && input->isADown)
        ? PLAYER_GRAVITY_SLOW
        : PLAYER_GRAVITY_FAST;

        vel.y += gravity * dt;
    }
    /*if(input->isUpDown) {
        vel.y -= PLAYER_ACCEL_NORMAL * dt;
    }
    if(input->isDownDown) {
        vel.y += PLAYER_ACCEL_NORMAL * dt;
    }
    if(input->isLeftDown) {
        vel.x -= PLAYER_ACCEL_NORMAL * dt;
    }
    if(input->isRightDown) {
        vel.x += PLAYER_ACCEL_NORMAL * dt;
    }

    if(!input->isUpDown && !input->isDownDown) {
        if (vel.y < 0) {
            vel.y += PLAYER_ACCEL_FRIC * dt;
            if (vel.y > 0) vel.y = 0;
        } else if (vel.y > 0) {
            vel.y -= PLAYER_ACCEL_FRIC * dt;
            if (vel.y < 0) vel.y = 0;
        }
    }
    if(!input->isLeftDown && !input->isRightDown) {
        if (vel.x < 0) {
            vel.x += PLAYER_ACCEL_FRIC * dt;
            if (vel.x > 0) vel.x = 0;
        } else if (vel.x > 0) {
            vel.x -= PLAYER_ACCEL_FRIC * dt;
            if (vel.x < 0) vel.x = 0;
        }
    }*/

    // Ensure mario's velocity doesn't exceed maximum speed
    if (vel.x > PLAYER_TOP_SPEED) vel.x = PLAYER_TOP_SPEED;
    if (vel.y > PLAYER_TOP_SPEED) vel.y = PLAYER_TOP_SPEED;
    if (vel.x < -PLAYER_TOP_SPEED) vel.x = -PLAYER_TOP_SPEED;
    if (vel.y < -PLAYER_TOP_SPEED) vel.y = -PLAYER_TOP_SPEED;
    
    mario->SetVelocity(vel);
}

MarioMovementState* MarioIdleState::HandleInput(Mario* mario)
{
    auto input = mario->GetInput();

    if (input->isLeftDown && !input->isRightDown) {
        return MarioStateFactory::CreateState(MarioMovementStateType::Walk, Direction::Left);
    }
    if (input->isRightDown && !input->isLeftDown) {
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

	if (!input->isLeftDown && !input->isRightDown && mario->GetVelocity().x == 0 && mario->IsGrounded() ||
        input->isLeftDown && input->isRightDown && mario->GetVelocity().x == 0) {
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

        if (absDX < PLAYER_TOP_WALKSPEED) {
            vel.x += (int)hitDir * PLAYER_ACCEL_NORMAL * dt;
        }
        // Slow down if exceeding walk speed (when transitioning from run to walk)
        else if (absDX > PLAYER_TOP_WALKSPEED && isGrounded) {
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
    bool isPMeterFull = HeadUpDisplay::GetInstance()->IsPMeterFull();

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

    if (isPMeterFull && !m_isSprinting) {
        mario->SetAnimId(mario->GetAnimId() - ID_ANIM_MARIO_WALK + ID_ANIM_MARIO_RUN);
        m_isSprinting = true;
    }
    else if (m_isSprinting){
        mario->SetAnimId(mario->GetAnimId() + ID_ANIM_MARIO_WALK - ID_ANIM_MARIO_RUN);
        m_isSprinting = false;
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
    bool isPMeterFull = HeadUpDisplay::GetInstance()->IsPMeterFull();
    // Get current direction input
    bool isLeftDown = input->isLeftDown;
    bool isRightDown = input->isRightDown;
    
    if (isLeftDown != isRightDown) {
        Direction hitDir = isLeftDown ? Direction::Left : Direction::Right;
        float absDX = std::abs(vel.x);
        
        // Accelerate up to run speed
        if (absDX < PLAYER_TOP_RUNSPEED) {
            vel.x += (int)hitDir * PLAYER_ACCEL_NORMAL * dt;
        }
        // Maintain run speed
        else if (absDX > PLAYER_TOP_RUNSPEED && isGrounded) {
            // No slowing down in run state unless switching directions
            if ((vel.x > 0 && hitDir == Direction::Right) || (vel.x < 0 && hitDir == Direction::Left)) {
                // Maintain current speed
                if (isPMeterFull && absDX < PLAYER_TOP_SPRINTSPEED) {
                    // continue accelerating when pmeter full and
                    vel.x += (int)hitDir * PLAYER_ACCEL_NORMAL * dt;
                }
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

    if (mario->IsGrounded() && absVelX < PLAYER_TOP_WALKSPEED && vel.y == 0.f ||
        mario->IsGrounded() && absVelX >= PLAYER_TOP_WALKSPEED && !input->isBDown && vel.y == 0.f) {
        return MarioStateFactory::CreateState(MarioMovementStateType::Walk, GetDirection());
    }
    if (mario->IsGrounded() && absVelX >= PLAYER_TOP_WALKSPEED && input->isBDown && vel.y == 0.f) {
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
        
        float speedCap = input->isBDown ? PLAYER_TOP_RUNSPEED : PLAYER_TOP_WALKSPEED;

        // Apply air control - same speed and accel as walk
        vel.x += (int)hitDir * PLAYER_ACCEL_NORMAL * dt;

        if(std::abs(vel.x) > speedCap) {
            vel.x -= (vel.x > 0 ? 1 : -1) * PLAYER_ACCEL_FRIC * dt;
        }
    }
    
    mario->SetVelocity(vel);
    
    MarioMovementState::Update(mario, dt);
}

MarioMovementState* MarioSitState::HandleInput(Mario* mario)
{
	auto input = mario->GetInput();

	if (!input->isDownDown) {
		return new MarioIdleState(GetDirection());
	}
	return nullptr;
}

void MarioSitState::Update(Mario* mario, float dt)
{
	MarioMovementState::Update(mario, dt);
}

Direction MarioMovementState::GetDirection() const
{
    return m_dir;
}