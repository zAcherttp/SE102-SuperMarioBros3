#include "pch.h"
#include "MarioMovementStates.h"
#include "AssetIDs.h"
#include "Debug.h"
#include "Mario.h"
#include "RedTroopas.h"

using namespace DirectX;

// Physics constants
constexpr auto PLAYER_TOP_WALKSPEED = 1.5f * 60.0f; // topSpeed when not running
constexpr auto PLAYER_TOP_RUNSPEED = 2.5f * 60.0f;  // topSpeed when running
constexpr auto PLAYER_TOP_SPRINTSPEED = 3.5f * 60.0f; // topSpeed when sprinting
constexpr auto PLAYER_TOP_ENDOFLEVELSPEED =
1.25f * 60.0f;                              // topSpeed when sprinting
constexpr auto PLAYER_TOP_SPEED = 4.0f * 60.0f; // Maximum allowed speed

constexpr auto PLAYER_HOLD_THROW_SPEED = 4.0f * 60.0f;

// Jump force values
constexpr float PLAYER_JUMP_FORCE[] = { -3.5f * 60.0f, -3.625f * 60.0f,
									   -3.75f * 60.0f, -4.0f * 60.0f };

// Acceleration values
constexpr auto PLAYER_ACCEL_NORMAL =
14.0f / 256.0f * 60.0f * 60.0f; // Normal acceleration
constexpr auto PLAYER_ACCEL_FRIC =
20.0f / 256.0f * 60.0f * 60.0f; // Friction deceleration
constexpr auto PLAYER_ACCEL_SKID =
32.0f / 256.0f * 60.0f * 60.0f; // Skidding deceleration

// Gravity values
constexpr auto PLAYER_GRAVITY_SLOW =
1.0f / 16.0f * 60.0f * 60.0f; // Slow gravity when holding jump
constexpr auto PLAYER_GRAVITY_FAST =
5.0f / 16.0f * 60.0f * 60.0f; // Fast gravity when not holding jump

std::string MarioDieMState::GetStateName() const { return "die"; }
std::string MarioIdleState::GetStateName() const { return "idle"; }
std::string MarioWalkState::GetStateName() const { return "walk"; }
std::string MarioRunState::GetStateName() const { return "run"; }
std::string MarioSkidState::GetStateName() const { return "skid"; }
std::string MarioJumpState::GetStateName() const { return "jump"; }
std::string MarioSitState::GetStateName() const { return "sit"; }
std::string MarioHoldState::GetStateName() const { return "hold"; }
std::string MarioKickState::GetStateName() const { return "kick"; }

int MarioDieMState::GetStateAnimValue() const { return ID_ANIM_MARIO_DIE; }
int MarioIdleState::GetStateAnimValue() const { return ID_ANIM_MARIO_IDLE; }
int MarioWalkState::GetStateAnimValue() const { return ID_ANIM_MARIO_WALK; }
int MarioRunState::GetStateAnimValue() const { return ID_ANIM_MARIO_WALK; }
int MarioSkidState::GetStateAnimValue() const { return ID_ANIM_MARIO_SKID; }
int MarioJumpState::GetStateAnimValue() const {
	if (HeadUpDisplay::GetInstance()->IsPMeterFull()) {
		return ID_ANIM_MARIO_POWERJUMP;
	}
	return ID_ANIM_MARIO_JUMP;
}
int MarioSitState::GetStateAnimValue() const { return ID_ANIM_MARIO_SIT; }
int MarioHoldState::GetStateAnimValue() const { return ID_ANIM_MARIO_HOLD_IDLE; }
int MarioKickState::GetStateAnimValue() const { return ID_ANIM_MARIO_KICK; }

Vector2 MarioDieMState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioIdleState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioWalkState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioRunState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioSkidState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioJumpState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioSitState::GetStateSizeOffset() const { return Vector2(0, -9); }
Vector2 MarioHoldState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioKickState::GetStateSizeOffset() const { return Vector2(0, 0); }

void MarioMovementState::Enter(Mario* mario) {
	mario->SetPosition(mario->GetPosition() - mario->GetCurrentMStateSizeOffset());
	mario->SetSize(mario->GetCurrentPStateSizeOffset() +
		mario->GetCurrentMStateSizeOffset());
	SetAnimation(mario, mario->GetCurrentPStateAnimValue() +
		mario->GetCurrentMStateAnimValue());
}

void MarioMovementState::Exit(Mario* mario) {
	//mario->SetPosition(mario->GetPosition() + mario->GetCurrentMStateSizeOffset());
	mario->SetSize(mario->GetCurrentPStateSizeOffset() -
		mario->GetCurrentMStateSizeOffset());
	SetAnimation(mario, mario->GetCurrentPStateAnimValue() -
		mario->GetCurrentMStateAnimValue());
}

void MarioDieMState::Enter(Mario* mario) {
	mario;
}

void MarioJumpState::Enter(Mario* mario) {
	// Determine jump force based on current horizontal speed
	float absSpeed = std::abs(mario->GetVelocity().x);
	int speedIndex = std::min(3, (int)std::floor(absSpeed / 60.0f));

	// Set initial jump velocity based on horizontal speed
	mario->SetVelocity(
		Vector2(mario->GetVelocity().x, PLAYER_JUMP_FORCE[speedIndex]));

	// Set animation and size
	mario->SetSize(mario->GetCurrentPStateSizeOffset() +
		mario->GetCurrentMStateSizeOffset());
	SetAnimation(mario, mario->GetCurrentPStateAnimValue() +
		mario->GetCurrentMStateAnimValue());
}

void MarioKickState::Enter(Mario* mario) {
	m_entity->SetVelocity(Vector2(PLAYER_HOLD_THROW_SPEED * (int)GetDirection(),
		0.f));

	RedTroopas* shell = dynamic_cast<RedTroopas*>(m_entity);

	if (shell && shell->GetState() == TroopaState::SHELL_IDLE) {
		shell->SetState(TroopaState::SHELL_SLIDE);
		shell->SetAnimation(ID_ANIM_RED_TROOPAS_SHELL_SLIDE, true);
	}


	// Set animation and size
	mario->SetSize(mario->GetCurrentPStateSizeOffset() +
		mario->GetCurrentMStateSizeOffset());
	SetAnimation(mario, mario->GetCurrentPStateAnimValue() +
		mario->GetCurrentMStateAnimValue());
}

void MarioMovementState::Update(Mario* mario, float dt) {
	// Apply universal forces
	Vector2 vel = mario->GetVelocity();
	auto input = mario->GetInput();

	if (input->isRightDown)
		mario->SetDirection((int)Direction::Right);
	if (input->isLeftDown)
		mario->SetDirection((int)Direction::Left);

	// Determine gravity based on jump button and upward momentum
	if (!mario->IsGrounded()) {
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
	if (vel.x > PLAYER_TOP_SPEED)
		vel.x = PLAYER_TOP_SPEED;
	if (vel.y > PLAYER_TOP_SPEED)
		vel.y = PLAYER_TOP_SPEED;
	if (vel.x < -PLAYER_TOP_SPEED)
		vel.x = -PLAYER_TOP_SPEED;
	if (vel.y < -PLAYER_TOP_SPEED)
		vel.y = -PLAYER_TOP_SPEED;

	mario->SetVelocity(vel);
}

std::unique_ptr<MarioMovementState> MarioDieMState::HandleInput(Mario* mario) {
	mario;
	return nullptr;
}

void MarioDieMState::Update(Mario* mario, float dt) {
	mario, dt;
}

std::unique_ptr<MarioMovementState> MarioIdleState::HandleInput(Mario* mario) {
	auto input = mario->GetInput();

	if (input->isLeftDown && !input->isRightDown) {
		return std::make_unique<MarioWalkState>(Direction::Left);
	}
	if (input->isRightDown && !input->isLeftDown) {
		return std::make_unique<MarioWalkState>(Direction::Right);
	}
	if (input->isDownDown && mario->IsGrounded() &&
		mario->GetCurrentPStateName() != "small") {
		return std::make_unique<MarioSitState>(GetDirection());
	}
	if (input->isAPressed && mario->IsGrounded()) {
		return std::make_unique<MarioJumpState>(GetDirection());
	}

	return nullptr;
}

void MarioIdleState::Update(Mario* mario, float dt) {
	Vector2 vel = mario->GetVelocity();

	if (vel.x < 0) {
		vel.x += PLAYER_ACCEL_FRIC * dt;
		if (vel.x > 0)
			vel.x = 0;
	}
	else if (vel.x > 0) {
		vel.x -= PLAYER_ACCEL_FRIC * dt;
		if (vel.x < 0)
			vel.x = 0;
	}

	mario->SetVelocity(vel);

	MarioMovementState::Update(mario, dt);
}

std::unique_ptr<MarioMovementState> MarioWalkState::HandleInput(Mario* mario) {
	auto input = mario->GetInput();

	if (input->isAPressed) {
		if (mario->IsGrounded()) {
			return std::make_unique<MarioJumpState>(GetDirection());
		}
	}

	if (input->isLeftDown && input->isRightDown == false) {
		if (GetDirection() == Direction::Right && mario->IsGrounded()) {
			return std::make_unique<MarioSkidState>(Direction::Right);
		}
	}

	if (input->isRightDown && input->isLeftDown == false) {
		if (GetDirection() == Direction::Left && mario->IsGrounded()) {
			return std::make_unique<MarioSkidState>(Direction::Left);
		}
	}

	if (input->isBDown && (input->isRightDown ^ input->isLeftDown)) {
		return std::make_unique<MarioRunState>(GetDirection());
	}

	if (input->isDownDown && mario->IsGrounded() &&
		mario->GetCurrentPStateName() != "small") {
		return std::make_unique<MarioSitState>(GetDirection());
	}

	if (!input->isLeftDown && !input->isRightDown &&
		mario->GetVelocity().x == 0 && mario->IsGrounded() ||
		input->isLeftDown && input->isRightDown && mario->GetVelocity().x == 0) {
		return std::make_unique<MarioIdleState>(GetDirection());
	}

	return nullptr;
}

void MarioWalkState::Update(Mario* mario, float dt) {
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
			if (vel.x > 0)
				vel.x = 0;
		}
		else if (vel.x > 0) {
			vel.x -= PLAYER_ACCEL_FRIC * dt;
			if (vel.x < 0)
				vel.x = 0;
		}
	}

	mario->SetVelocity(vel);

	MarioMovementState::Update(mario, dt);
}

std::unique_ptr<MarioMovementState> MarioRunState::HandleInput(Mario* mario) {
	auto input = mario->GetInput();
	bool isPMeterFull = HeadUpDisplay::GetInstance()->IsPMeterFull();

	if (input->isAPressed && mario->IsGrounded()) {
		return std::make_unique<MarioJumpState>(GetDirection());
	}
	if (input->isLeftDown && input->isRightDown == false) {
		if (GetDirection() == Direction::Right && mario->IsGrounded()) {
			return std::make_unique<MarioSkidState>(Direction::Right);
		}
	}

	if (input->isRightDown && input->isLeftDown == false) {
		if (GetDirection() == Direction::Left && mario->IsGrounded()) {
			return std::make_unique<MarioSkidState>(Direction::Left);
		}
	}

	if (isPMeterFull && !m_isSprinting) {
		SetAnimation(mario, mario->GetAnimId() - ID_ANIM_MARIO_WALK +
			ID_ANIM_MARIO_SPRINT);
		m_isSprinting = true;
	}
	else if (!isPMeterFull && m_isSprinting) {
		SetAnimation(mario, mario->GetAnimId() + ID_ANIM_MARIO_WALK -
			ID_ANIM_MARIO_SPRINT);
		m_isSprinting = false;
	}

	if (input->isDownDown && mario->IsGrounded() &&
		mario->GetCurrentPStateName() != "small") {
		return std::make_unique<MarioSitState>(GetDirection());
	}

	if (input->isRightDown == false && input->isLeftDown == false ||
		input->isRightDown && input->isLeftDown && input->isBDown) {
		return std::make_unique<MarioWalkState>(GetDirection());
	}

	return nullptr;
}

void MarioRunState::Update(Mario* mario, float dt) {
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
			if ((vel.x > 0 && hitDir == Direction::Right) ||
				(vel.x < 0 && hitDir == Direction::Left)) {
				// Maintain current speed
				if (isPMeterFull && absDX < PLAYER_TOP_SPRINTSPEED) {
					// continue accelerating when pmeter full and
					vel.x += (int)hitDir * PLAYER_ACCEL_NORMAL * dt;
				}
			}
		}
		else {
			// Slow down if changing direction
			vel.x -= (vel.x > 0 ? 1 : -1) * PLAYER_ACCEL_FRIC * dt;
		}
	}
	// No direction input, apply friction
	else if (isGrounded) {
		if (vel.x < 0) {
			vel.x += PLAYER_ACCEL_FRIC * dt;
			if (vel.x > 0)
				vel.x = 0;
		}
		else if (vel.x > 0) {
			vel.x -= PLAYER_ACCEL_FRIC * dt;
			if (vel.x < 0)
				vel.x = 0;
		}
	}

	mario->SetVelocity(vel);

	MarioMovementState::Update(mario, dt);
}

std::unique_ptr<MarioMovementState> MarioSkidState::HandleInput(Mario* mario) {

	auto input = mario->GetInput();

	if (mario->GetVelocity().x == 0 && GetDirection() != m_lastDir) {
		return std::make_unique<MarioWalkState>(GetDirection());
	}

	if (input->isDownDown && mario->IsGrounded() &&
		mario->GetCurrentPStateName() != "small") {
		return std::make_unique<MarioSitState>(GetDirection());
	}

	return nullptr;
}

void MarioSkidState::Update(Mario* mario, float dt) {
	Vector2 vel = mario->GetVelocity();

	// Apply stronger friction during skid
	vel.x -= PLAYER_ACCEL_SKID * (int)m_lastDir * dt;

	// Check if we've fully stopped or changed direction
	if ((m_lastDir == Direction::Right && vel.x <= 0) ||
		(m_lastDir == Direction::Left && vel.x >= 0)) {
		vel.x = 0;
		m_dir =
			(m_lastDir == Direction::Right) ? Direction::Left : Direction::Right;
	}

	mario->SetVelocity(vel);

	MarioMovementState::Update(mario, dt);
}

std::unique_ptr<MarioMovementState> MarioJumpState::HandleInput(Mario* mario) {
	auto input = mario->GetInput();
	auto vel = mario->GetVelocity();
	auto absVelX = std::abs(vel.x);

	if (mario->IsGrounded() && absVelX < PLAYER_TOP_WALKSPEED && vel.y == 0.f ||
		mario->IsGrounded() && absVelX >= PLAYER_TOP_WALKSPEED &&
		!input->isBDown && vel.y == 0.f) {
		return std::make_unique<MarioWalkState>(GetDirection());
	}
	if (mario->IsGrounded() && absVelX >= PLAYER_TOP_WALKSPEED &&
		input->isBDown && vel.y == 0.f) {
		return std::make_unique<MarioRunState>(GetDirection());
	}

	return nullptr;
}

void MarioJumpState::Update(Mario* mario, float dt) {
	auto input = mario->GetInput();
	Vector2 vel = mario->GetVelocity();

	// Handle horizontal movement in air
	bool isLeftDown = input->isLeftDown;
	bool isRightDown = input->isRightDown;

	if (isLeftDown != isRightDown) {
		Direction hitDir = isLeftDown ? Direction::Left : Direction::Right;

		float speedCap =
			input->isBDown ? PLAYER_TOP_RUNSPEED : PLAYER_TOP_WALKSPEED;

		// Apply air control - same speed and accel as walk
		vel.x += (int)hitDir * PLAYER_ACCEL_NORMAL * dt;

		if (std::abs(vel.x) > speedCap) {
			vel.x -= (vel.x > 0 ? 1 : -1) * PLAYER_ACCEL_FRIC * dt;
		}
	}

	if (vel.y > 0 && !m_isFalling && !HeadUpDisplay::GetInstance()->IsPMeterFull()
		&& mario->GetCurrentPStateName() != "small") {
		m_isFalling = true;
		SetAnimation(mario, mario->GetCurrentMStateAnimValue() +
			mario->GetCurrentPStateAnimValue() + ID_ANIM_MARIO_JUMP_ALT);
	}

	mario->SetVelocity(vel);

	MarioMovementState::Update(mario, dt);
}

std::unique_ptr<MarioMovementState> MarioSitState::HandleInput(Mario* mario) {
	auto input = mario->GetInput();

	if (!input->isDownDown) {
		return std::make_unique<MarioIdleState>(GetDirection());
	}
	return nullptr;
}

void MarioSitState::Update(Mario* mario, float dt) {
	Vector2 vel = mario->GetVelocity();

	if (vel.x < 0) {
		vel.x += PLAYER_ACCEL_FRIC * dt;
		if (vel.x > 0)
			vel.x = 0;
	}
	else if (vel.x > 0) {
		vel.x -= PLAYER_ACCEL_FRIC * dt;
		if (vel.x < 0)
			vel.x = 0;
	}

	mario->SetVelocity(vel);

	MarioMovementState::Update(mario, dt);
}

MarioHoldState::MarioHoldState(Direction dir, Entity* heldEntity) : MarioMovementState(dir), m_heldEntity(heldEntity) {
	m_jumpTimer = 0.0f;
	m_dirChangeState = dir == Direction::Left ? 0.f : m_dirChangeTimeSpan;

	m_newDir = dir;

	Vector2 size = heldEntity->GetSize();
	m_heldEntityOffset = Vector2(size.x / 2.f * (int)dir, -size.y / 2.f);

	//Log(LOG_INFO, "Initialized");
}

std::unique_ptr<MarioMovementState> MarioHoldState::HandleInput(Mario* mario) {
	auto input = mario->GetInput();
	bool isGrounded = mario->IsGrounded();
	Vector2 vel = mario->GetVelocity();

	if (m_isInterrupted) {
		return std::make_unique<MarioIdleState>(GetDirection());
	}

	if (input->isAPressed && isGrounded && !m_isJumping) {
		Log(LOG_INFO, "MarioHoldState::HandleInput: Jumping");
		float absSpeed = std::abs(vel.x);
		int speedIndex = std::min(3, (int)std::floor(absSpeed / 60.0f));
		m_isJumping = true;
		m_jumpTimer = 0.f;

		mario->SetVelocity(Vector2(vel.x, PLAYER_JUMP_FORCE[speedIndex]));
		SetAnimation(mario, mario->GetCurrentMStateAnimValue()
			+ mario->GetCurrentPStateAnimValue()
			+ ID_ANIM_MARIO_HOLD_JUMP);
	}
	else if (isGrounded && m_isJumping && m_jumpTimer > m_jumpDebounce) {
		Log(LOG_INFO, "MarioHoldState::HandleInput: Landed");
		m_isJumping = false;
		SetAnimation(mario, mario->GetCurrentMStateAnimValue()
			+ mario->GetCurrentPStateAnimValue() + ID_ANIM_MARIO_HOLD);
	}

	if (vel.x != 0 && !m_isWalking && isGrounded) {
		m_isWalking = true;
		SetAnimation(mario, mario->GetCurrentMStateAnimValue()
			+ mario->GetCurrentPStateAnimValue() + ID_ANIM_MARIO_HOLD);
	}
	else if (vel.x == 0 && m_isWalking && isGrounded) {
		m_isWalking = false;
		Log(__FUNCTION__, "Stopped walking");
		SetAnimation(mario, mario->GetCurrentMStateAnimValue()
			+ mario->GetCurrentPStateAnimValue());
	}

	if (!input->isBDown) {
		return std::make_unique<MarioKickState>(GetDirection(), m_heldEntity);
	}

	return nullptr;
}

void MarioHoldState::Update(Mario* mario, float dt) {
	auto input = mario->GetInput();

	RedTroopas* shell = dynamic_cast<RedTroopas*>(m_heldEntity);

	if (shell && shell->GetState() == TroopaState::WALKING)
	{
		m_isInterrupted = true;
		return;
	}

	Vector2 vel = mario->GetVelocity();
	bool isGrounded = mario->IsGrounded();
	bool isPMeterFull = HeadUpDisplay::GetInstance()->IsPMeterFull();
	// Get current direction input
	bool isLeftDown = input->isLeftDown;
	bool isRightDown = input->isRightDown;

	if (m_isJumping) m_jumpTimer += dt;

	if (isLeftDown != isRightDown) {
		Direction hitDir = isLeftDown ? Direction::Left : Direction::Right;
		m_newDir = hitDir;
		float absDX = std::abs(vel.x);

		if (isGrounded) {
			// changing direction sequence

			// Accelerate up to run speed
			if (absDX < PLAYER_TOP_RUNSPEED) {
				vel.x += (int)hitDir * PLAYER_ACCEL_NORMAL * dt;
			}
			// Maintain run speed
			else if (absDX > PLAYER_TOP_RUNSPEED && isGrounded) {
				// No slowing down in run state unless switching directions
				if ((vel.x > 0 && hitDir == Direction::Right) ||
					(vel.x < 0 && hitDir == Direction::Left)) {
					// Maintain current speed
					if (isPMeterFull && absDX < PLAYER_TOP_SPRINTSPEED) {
						// continue accelerating when pmeter full and
						vel.x += (int)hitDir * PLAYER_ACCEL_NORMAL * dt;
					}
				}
			}
		}
		else {
			// Apply air control - same speed and accel as walk
			vel.x += (int)hitDir * PLAYER_ACCEL_NORMAL * dt;

			if (std::abs(vel.x) > PLAYER_TOP_RUNSPEED) {
				vel.x -= (vel.x > 0 ? 1 : -1) * PLAYER_ACCEL_FRIC * dt;
			}
		}
	}
	// No direction input, apply friction
	else if (isGrounded) {
		if (vel.x < 0) {
			vel.x += PLAYER_ACCEL_FRIC * dt;
			if (vel.x > 0)
				vel.x = 0;
		}
		else if (vel.x > 0) {
			vel.x -= PLAYER_ACCEL_FRIC * dt;
			if (vel.x < 0)
				vel.x = 0;
		}
	}

	// not on ground, apply gravity
	if (!isGrounded) {
		float gravity = (vel.y < -2.0f * 60.0f && input->isADown)
			? PLAYER_GRAVITY_SLOW
			: PLAYER_GRAVITY_FAST;
		vel.y += gravity * dt;
	}

	if (vel.x > PLAYER_TOP_RUNSPEED) vel.x = PLAYER_TOP_RUNSPEED;
	if (vel.x < -PLAYER_TOP_RUNSPEED) vel.x = -PLAYER_TOP_RUNSPEED;
	if (vel.y > PLAYER_TOP_SPEED) vel.y = PLAYER_TOP_SPEED;
	if (vel.y < -PLAYER_TOP_SPEED) vel.y = -PLAYER_TOP_SPEED;

	if (m_newDir == Direction::Left && m_dirChangeState > 0.f) {
		m_dirChangeState -= dt;
	}
	else if (m_newDir == Direction::Right && m_dirChangeState < m_dirChangeTimeSpan) {
		m_dirChangeState += dt;
	}

	int state = (int)std::floor(std::clamp(m_dirChangeState, 0.f, m_dirChangeTimeSpan) / m_dirChangeTimeSpan * 4);
	if (state != m_dirState) {
		m_dirState = state;
		switch (m_dirState) {
		case 0:
			SetAnimation(mario, mario->GetCurrentMStateAnimValue()
				+ mario->GetCurrentPStateAnimValue());
			mario->SetDirection(-1);
			m_dir = Direction::Left;
			break;
		case 1:
			SetAnimation(mario, mario->GetCurrentMStateAnimValue()
				+ mario->GetCurrentPStateAnimValue() + ID_ANIM_MARIO_HOLD_FRONT);
			m_dir = Direction::Left;
			break;
		case 2:
			SetAnimation(mario, mario->GetCurrentMStateAnimValue()
				+ mario->GetCurrentPStateAnimValue() + ID_ANIM_MARIO_HOLD_FRONT);
			m_dir = Direction::Right;
			break;
		case 3:
			SetAnimation(mario, mario->GetCurrentMStateAnimValue()
				+ mario->GetCurrentPStateAnimValue());
			mario->SetDirection(1);
			m_dir = Direction::Right;
			break;
		}
	}

	mario->SetVelocity(vel);
	m_heldEntity->SetPosition(mario->GetPosition() + GetHeldEntityOffset(mario, state));
}

Vector2 MarioHoldState::GetHeldEntityOffset(Mario* mario, const int& state)
{
	Vector2 size = m_heldEntity->GetSize();
	Vector2 mSize = mario->GetSize();
	// state / 4.0f * (1 - (-1)) + (-1), get direction transition lerp value
	float lerp = state / 4.0f * 2.f - 1.f;

	m_heldEntityOffset = Vector2(size.x * lerp, -size.y) / 2.f;
	return Vector2(mSize.x * lerp, mSize.y) / 2.f + m_heldEntityOffset;
}

std::unique_ptr<MarioMovementState> MarioKickState::HandleInput(Mario* mario) {
	auto input = mario->GetInput();

	if (input->isLeftPressed) {
		return std::make_unique<MarioWalkState>(GetDirection());
	}
	if (input->isRightPressed) {
		return std::make_unique<MarioWalkState>(GetDirection());
	}
	if (input->isAPressed) {
		return std::make_unique<MarioJumpState>(GetDirection());
	}

	if (m_kickTimer < 0) {
		return std::make_unique<MarioIdleState>(GetDirection());
	}

	return nullptr;
}

void MarioKickState::Update(Mario* mario, float dt) {
	bool isGrounded = mario->IsGrounded();
	Vector2 vel = mario->GetVelocity();

	m_kickTimer -= dt;

	if (isGrounded) {
		if (vel.x < 0) {
			vel.x += PLAYER_ACCEL_FRIC * dt;
			if (vel.x > 0)
				vel.x = 0;
		}
		else if (vel.x > 0) {
			vel.x -= PLAYER_ACCEL_FRIC * dt;
			if (vel.x < 0)
				vel.x = 0;
		}
	}

	mario->SetVelocity(vel);
	MarioMovementState::Update(mario, dt);
}

Direction MarioMovementState::GetDirection() const { return m_dir; }