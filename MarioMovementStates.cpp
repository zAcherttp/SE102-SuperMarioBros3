#include "pch.h"
#include "AssetIDs.h"
#include "Block.h"
#include "Debug.h"
#include "GameConfig.h"
#include "GreenTroopa.h"
#include "Mario.h"
#include "MarioMovementStates.h"
#include "RedTroopas.h"
#include "Troopa.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace GameStrings;
using namespace GameConstants;

Direction MarioMovementState::GetDirection() const { return m_dir; }

std::string MarioDieMState::GetStateName() const { return DIE; }
std::string MarioIdleState::GetStateName() const { return IDLE; }
std::string MarioWalkState::GetStateName() const { return WALK; }
std::string MarioRunState::GetStateName() const { return RUN; }
std::string MarioSkidState::GetStateName() const { return SKID; }
std::string MarioJumpState::GetStateName() const { return JUMP; }
std::string MarioSitState::GetStateName() const { return SIT; }
std::string MarioHoldState::GetStateName() const { return HOLD; }
std::string MarioKickState::GetStateName() const { return KICK; }
std::string MarioHoverState::GetStateName() const { return HOVER; }
std::string MarioFlyState::GetStateName() const { return FLY; }
std::string MarioSweepState::GetStateName() const { return SWEEP; }

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
int MarioHoldState::GetStateAnimValue() const {
	return ID_ANIM_MARIO_HOLD_IDLE;
}
int MarioKickState::GetStateAnimValue() const { return ID_ANIM_MARIO_KICK; }
int MarioHoverState::GetStateAnimValue() const { return ID_ANIM_MARIO_JUMP; }
int MarioFlyState::GetStateAnimValue() const { return ID_ANIM_MARIO_FLY; }
int MarioSweepState::GetStateAnimValue() const { return ID_ANIM_MARIO_SWEEP; }

Vector2 MarioDieMState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioIdleState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioWalkState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioRunState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioSkidState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioJumpState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioSitState::GetStateSizeOffset() const { return Vector2(0, -9); }
Vector2 MarioHoldState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioKickState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioHoverState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioFlyState::GetStateSizeOffset() const { return Vector2(0, 0); }
Vector2 MarioSweepState::GetStateSizeOffset() const { return Vector2(0, 0); }

void MarioMovementState::Enter(Mario* mario) {
	mario->SetPosition(mario->GetPosition() -
		mario->GetCurrentMStateSizeOffset() / 2.f);
	mario->SetSize(mario->GetCurrentPStateSizeOffset() +
		mario->GetCurrentMStateSizeOffset());
	SetAnimation(mario, mario->GetCurrentPStateAnimValue() +
		mario->GetCurrentMStateAnimValue());
}

void MarioMovementState::Exit(Mario* mario) {
	mario->SetPosition(mario->GetPosition() +
		mario->GetCurrentMStateSizeOffset() / 2.f);
	mario->SetSize(mario->GetCurrentPStateSizeOffset() -
		mario->GetCurrentMStateSizeOffset());
	SetAnimation(mario, mario->GetCurrentPStateAnimValue() -
		mario->GetCurrentMStateAnimValue());
}

void MarioDieMState::Enter(Mario* mario) { mario; }

void MarioJumpState::Enter(Mario* mario) {
	// Determine jump force based on current horizontal speed
	float absSpeed = std::abs(mario->GetVelocity().x);
	int speedIndex = std::min(3, (int)std::floor(absSpeed / 60.0f));

	// Set initial jump velocity based on horizontal speed
	mario->SetVelocity(
		Vector2(mario->GetVelocity().x, Player::JUMP_FORCE[speedIndex]));

	// Set animation and size
	mario->SetSize(mario->GetCurrentPStateSizeOffset() +
		mario->GetCurrentMStateSizeOffset());
	SetAnimation(mario, mario->GetCurrentPStateAnimValue() +
		mario->GetCurrentMStateAnimValue());
}

void MarioFlyState::Enter(Mario* mario) {
	// Determine jump force based on current horizontal speed
	float absSpeed = std::abs(mario->GetVelocity().x);
	int speedIndex = std::min(3, (int)std::floor(absSpeed / 60.0f));

	// Set initial jump velocity based on horizontal speed
	mario->SetVelocity(
		Vector2(mario->GetVelocity().x, Player::JUMP_FORCE[speedIndex]));

	// Set animation and size
	mario->SetSize(mario->GetCurrentPStateSizeOffset() +
		mario->GetCurrentMStateSizeOffset());
	SetAnimation(mario, mario->GetCurrentPStateAnimValue() +
		mario->GetCurrentMStateAnimValue());
}

void MarioSweepState::Enter(Mario* mario) {
	m_originalFlipState = mario->GetAnimator()->GetSpriteEffects() ==
		SpriteEffects_FlipHorizontally;

	// Set animation and size
	mario->SetSize(mario->GetCurrentPStateSizeOffset() +
		mario->GetCurrentMStateSizeOffset());
	SetAnimation(mario, mario->GetCurrentPStateAnimValue() +
		mario->GetCurrentMStateAnimValue());
}

void MarioKickState::Enter(Mario* mario) {
	m_entity->SetVelocity(
		Vector2(Player::HOLD_THROW_SPEED * (int)GetDirection(), 0.f));

	Troopa* shell = dynamic_cast<Troopa*>(m_entity);

	if (shell && shell->GetState() == TroopaState::SHELL_IDLE) {
		RedTroopas* redShell = dynamic_cast<RedTroopas*>(shell);
		GreenTroopas* greenShell = dynamic_cast<GreenTroopas*>(shell);
		if (redShell) {
			redShell->SetState(TroopaState::SHELL_SLIDE);
			redShell->SetAnimation(ID_ANIM_RED_TROOPAS_SHELL_SLIDE, true);
		}
		else if (greenShell) {
			greenShell->SetState(TroopaState::SHELL_SLIDE);
			greenShell->SetAnimation(ID_ANIM_GREEN_TROOPAS_SHELL_SLIDE, true);
		}
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
	// if (!mario->IsGrounded()) {
	float gravity = (vel.y < -2.0f * 60.0f && input->isADown)
		? Player::ACCEL_GRAVITY_SLOW
		: Player::ACCEL_GRAVITY_FAST;

	vel.y += gravity * dt;
	//}
	/*if(input->isUpDown) {
					vel.y -= Player::ACCEL_NORMAL * dt;
	}
	if(input->isDownDown) {
					vel.y += Player::ACCEL_NORMAL * dt;
	}
	if(input->isLeftDown) {
					vel.x -= Player::ACCEL_NORMAL * dt;
	}
	if(input->isRightDown) {
					vel.x += Player::ACCEL_NORMAL * dt;
	}

	if(!input->isUpDown && !input->isDownDown) {
					if (vel.y < 0) {
									vel.y += Player::ACCEL_FRIC * dt;
									if (vel.y > 0) vel.y = 0;
					} else if (vel.y > 0) {
									vel.y -= Player::ACCEL_FRIC * dt;
									if (vel.y < 0) vel.y = 0;
					}
	}
	if(!input->isLeftDown && !input->isRightDown) {
					if (vel.x < 0) {
									vel.x += Player::ACCEL_FRIC * dt;
									if (vel.x > 0) vel.x = 0;
					} else if (vel.x > 0) {
									vel.x -= Player::ACCEL_FRIC * dt;
									if (vel.x < 0) vel.x = 0;
					}
	}*/

	// Ensure mario's velocity doesn't exceed maximum speed
	if (vel.x > Player::MAX_GENERAL_SPEED)
		vel.x = Player::MAX_GENERAL_SPEED;
	if (vel.y > Player::MAX_GENERAL_SPEED)
		vel.y = Player::MAX_GENERAL_SPEED;
	if (vel.x < -Player::MAX_GENERAL_SPEED)
		vel.x = -Player::MAX_GENERAL_SPEED;
	if (vel.y < -Player::MAX_GENERAL_SPEED)
		vel.y = -Player::MAX_GENERAL_SPEED;

	mario->SetVelocity(vel);
}

std::unique_ptr<MarioMovementState> MarioDieMState::HandleInput(Mario* mario) {
	mario;
	return nullptr;
}

void MarioDieMState::Update(Mario* mario, float dt) { mario, dt; }

std::unique_ptr<MarioMovementState> MarioIdleState::HandleInput(Mario* mario) {
	auto input = mario->GetInput();

	if (input->isLeftDown && !input->isRightDown) {
		return std::make_unique<MarioWalkState>(Direction::Left);
	}
	if (input->isRightDown && !input->isLeftDown) {
		return std::make_unique<MarioWalkState>(Direction::Right);
	}
	if (input->isDownDown && mario->IsGrounded() &&
		mario->GetCurrentPStateName() != SMALL) {
		return std::make_unique<MarioSitState>(GetDirection());
	}
	if (input->isAPressed && mario->IsGrounded()) {
		return std::make_unique<MarioJumpState>(GetDirection());
	}
	if (input->isBPressed && mario->GetCurrentPStateName() == RACCOON) {
		return std::make_unique<MarioSweepState>(GetDirection());
	}

	return nullptr;
}

void MarioIdleState::Update(Mario* mario, float dt) {
	Vector2 vel = mario->GetVelocity();

	if (vel.x < 0) {
		vel.x += Player::ACCEL_FRIC * dt;
		if (vel.x > 0)
			vel.x = 0;
	}
	else if (vel.x > 0) {
		vel.x -= Player::ACCEL_FRIC * dt;
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

	if (input->isBPressed && mario->GetCurrentPStateName() == RACCOON) {
		return std::make_unique<MarioSweepState>(GetDirection());
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

		if (absDX < Player::MAX_WALK_SPEED) {
			vel.x += (int)hitDir * Player::ACCEL_NORMAL * dt;
		}
		// Slow down if exceeding walk speed (when transitioning from run to walk)
		else if (absDX > Player::MAX_WALK_SPEED && isGrounded) {
			vel.x -= (int)hitDir * Player::ACCEL_FRIC * dt;
		}
	}
	// No direction input, apply friction
	else if (isGrounded) {
		if (vel.x < 0) {
			vel.x += Player::ACCEL_FRIC * dt;
			if (vel.x > 0)
				vel.x = 0;
		}
		else if (vel.x > 0) {
			vel.x -= Player::ACCEL_FRIC * dt;
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
		if (isPMeterFull && mario->GetCurrentPStateName() == RACCOON) {
			Log(__FUNCTION__, "");
			return std::make_unique<MarioFlyState>(GetDirection());
		}
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
		input->isRightDown && input->isLeftDown && input->isBDown ||
		!input->isBDown) {
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
		if (absDX < Player::MAX_RUN_SPEED) {
			vel.x += (int)hitDir * Player::ACCEL_NORMAL * dt;
		}
		// Maintain run speed
		else if (absDX > Player::MAX_RUN_SPEED && isGrounded) {
			// No slowing down in run state unless switching directions
			if ((vel.x > 0 && hitDir == Direction::Right) ||
				(vel.x < 0 && hitDir == Direction::Left)) {
				// Maintain current speed
				if (isPMeterFull && absDX < Player::MAX_SPRINT_SPEED) {
					// continue accelerating when pmeter full and
					vel.x += (int)hitDir * Player::ACCEL_NORMAL * dt;
				}
			}
		}
		else {
			// Slow down if changing direction
			vel.x -= (vel.x > 0 ? 1 : -1) * Player::ACCEL_FRIC * dt;
		}
	}
	// No direction input, apply friction
	else if (isGrounded) {
		if (vel.x < 0) {
			vel.x += Player::ACCEL_FRIC * dt;
			if (vel.x > 0)
				vel.x = 0;
		}
		else if (vel.x > 0) {
			vel.x -= Player::ACCEL_FRIC * dt;
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
	vel.x -= Player::ACCEL_SKID * (int)m_lastDir * dt;

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

	if (input->isAPressed && mario->GetCurrentPStateName() == RACCOON) {
		return std::make_unique<MarioHoverState>(GetDirection());
	}

	if (std::abs(vel.y) < 5.f) {
		if (mario->IsGrounded() && absVelX < Player::MAX_WALK_SPEED ||
			mario->IsGrounded() && absVelX >= Player::MAX_WALK_SPEED &&
			!input->isBDown) {
			return std::make_unique<MarioWalkState>(GetDirection());
		}
		if (mario->IsGrounded() && absVelX >= Player::MAX_WALK_SPEED &&
			input->isBDown) {
			return std::make_unique<MarioRunState>(GetDirection());
		}
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
			input->isBDown ? Player::MAX_RUN_SPEED : Player::MAX_WALK_SPEED;

		// Apply air control - same speed and accel as walk
		vel.x += (int)hitDir * Player::ACCEL_NORMAL * dt;

		if (std::abs(vel.x) > speedCap) {
			vel.x -= (vel.x > 0 ? 1 : -1) * Player::ACCEL_FRIC * dt;
		}
	}

	if (vel.y > 0 && !m_isFalling &&
		!HeadUpDisplay::GetInstance()->IsPMeterFull() &&
		mario->GetCurrentPStateName() != "small") {
		m_isFalling = true;
		SetAnimation(mario, mario->GetCurrentMStateAnimValue() +
			mario->GetCurrentPStateAnimValue() +
			ID_ANIM_MARIO_JUMP_ALT);
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
		vel.x += Player::ACCEL_FRIC * dt;
		if (vel.x > 0)
			vel.x = 0;
	}
	else if (vel.x > 0) {
		vel.x -= Player::ACCEL_FRIC * dt;
		if (vel.x < 0)
			vel.x = 0;
	}

	mario->SetVelocity(vel);

	MarioMovementState::Update(mario, dt);
}

MarioHoldState::MarioHoldState(Direction dir, Entity* heldEntity)
	: MarioMovementState(dir), m_heldEntity(heldEntity) {
	m_jumpTimer = 0.0f;
	m_dirChangeState = dir == Direction::Left ? 0.f : m_dirChangeTimeSpan;

	m_newDir = dir;

	Vector2 size = heldEntity->GetSize();
	m_heldEntityOffset = Vector2(size.x / 2.f * (int)dir, -size.y / 2.f);

	// Log(LOG_INFO, "Initialized");
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

		mario->SetVelocity(Vector2(vel.x, Player::JUMP_FORCE[speedIndex]));
		SetAnimation(mario, mario->GetCurrentMStateAnimValue() +
			mario->GetCurrentPStateAnimValue() +
			ID_ANIM_MARIO_HOLD_JUMP);
	}
	else if (isGrounded && m_isJumping && m_jumpTimer > m_jumpDebounce) {
		Log(LOG_INFO, "MarioHoldState::HandleInput: Landed");
		m_isJumping = false;
		SetAnimation(mario, mario->GetCurrentMStateAnimValue() +
			mario->GetCurrentPStateAnimValue() +
			ID_ANIM_MARIO_HOLD);
	}

	if (vel.x != 0 && !m_isWalking && isGrounded) {
		m_isWalking = true;
		SetAnimation(mario, mario->GetCurrentMStateAnimValue() +
			mario->GetCurrentPStateAnimValue() +
			ID_ANIM_MARIO_HOLD);
	}
	else if (vel.x == 0 && m_isWalking && isGrounded) {
		m_isWalking = false;
		Log(__FUNCTION__, "Stopped walking");
		SetAnimation(mario, mario->GetCurrentMStateAnimValue() +
			mario->GetCurrentPStateAnimValue());
	}

	if (!input->isBDown) {
		return std::make_unique<MarioKickState>(GetDirection(), m_heldEntity);
	}

	return nullptr;
}

void MarioHoldState::Update(Mario* mario, float dt) {
	auto input = mario->GetInput();

	Troopa* shell = dynamic_cast<Troopa*>(m_heldEntity);

	if (shell && shell->GetState() != TroopaState::SHELL_IDLE) {
		m_isInterrupted = true;
		return;
	}

	Vector2 vel = mario->GetVelocity();
	bool isGrounded = mario->IsGrounded();
	bool isPMeterFull = HeadUpDisplay::GetInstance()->IsPMeterFull();
	// Get current direction input
	bool isLeftDown = input->isLeftDown;
	bool isRightDown = input->isRightDown;

	if (m_isJumping)
		m_jumpTimer += dt;

	if (isLeftDown != isRightDown) {
		Direction hitDir = isLeftDown ? Direction::Left : Direction::Right;
		m_newDir = hitDir;
		float absDX = std::abs(vel.x);

		if (isGrounded) {
			// changing direction sequence

			// Accelerate up to run speed
			if (absDX < Player::MAX_RUN_SPEED) {
				vel.x += (int)hitDir * Player::ACCEL_NORMAL * dt;
			}
			// Maintain run speed
			else if (absDX > Player::MAX_RUN_SPEED && isGrounded) {
				// No slowing down in run state unless switching directions
				if ((vel.x > 0 && hitDir == Direction::Right) ||
					(vel.x < 0 && hitDir == Direction::Left)) {
					// Maintain current speed
					if (isPMeterFull && absDX < Player::MAX_SPRINT_SPEED) {
						// continue accelerating when pmeter full and
						vel.x += (int)hitDir * Player::ACCEL_NORMAL * dt;
					}
				}
			}
		}
		else {
			// Apply air control - same speed and accel as walk
			vel.x += (int)hitDir * Player::ACCEL_NORMAL * dt;

			if (std::abs(vel.x) > Player::MAX_RUN_SPEED) {
				vel.x -= (vel.x > 0 ? 1 : -1) * Player::ACCEL_FRIC * dt;
			}
		}
	}
	// No direction input, apply friction
	else if (isGrounded) {
		if (vel.x < 0) {
			vel.x += Player::ACCEL_FRIC * dt;
			if (vel.x > 0)
				vel.x = 0;
		}
		else if (vel.x > 0) {
			vel.x -= Player::ACCEL_FRIC * dt;
			if (vel.x < 0)
				vel.x = 0;
		}
	}

	// not on ground, apply gravity
	if (!isGrounded) {
		float gravity = (vel.y < -2.0f * 60.0f && input->isADown)
			? Player::ACCEL_GRAVITY_SLOW
			: Player::ACCEL_GRAVITY_FAST;
		vel.y += gravity * dt;
	}

	if (vel.x > Player::MAX_RUN_SPEED)
		vel.x = Player::MAX_RUN_SPEED;
	if (vel.x < -Player::MAX_RUN_SPEED)
		vel.x = -Player::MAX_RUN_SPEED;
	if (vel.y > Player::MAX_GENERAL_SPEED)
		vel.y = Player::MAX_GENERAL_SPEED;
	if (vel.y < -Player::MAX_GENERAL_SPEED)
		vel.y = -Player::MAX_GENERAL_SPEED;

	if (m_newDir == Direction::Left && m_dirChangeState > 0.f) {
		m_dirChangeState -= dt;
	}
	else if (m_newDir == Direction::Right &&
		m_dirChangeState < m_dirChangeTimeSpan) {
		m_dirChangeState += dt;
	}

	int state =
		(int)std::floor(std::clamp(m_dirChangeState, 0.f, m_dirChangeTimeSpan) /
			m_dirChangeTimeSpan * 4);
	if (state != m_dirState) {
		m_dirState = state;
		switch (m_dirState) {
		case 0:
			SetAnimation(mario, mario->GetCurrentMStateAnimValue() +
				mario->GetCurrentPStateAnimValue());
			mario->SetDirection(-1);
			m_heldEntity->GetAnimator()->ResetDepth();
			m_dir = Direction::Left;
			break;
		case 1:
			SetAnimation(mario, mario->GetCurrentMStateAnimValue() +
				mario->GetCurrentPStateAnimValue() +
				ID_ANIM_MARIO_HOLD_FRONT);
			m_heldEntity->GetAnimator()->SetDepth(0.f);
			m_dir = Direction::Left;
			break;
		case 2:
			SetAnimation(mario, mario->GetCurrentMStateAnimValue() +
				mario->GetCurrentPStateAnimValue() +
				ID_ANIM_MARIO_HOLD_FRONT);
			m_heldEntity->GetAnimator()->SetDepth(0.f);
			m_dir = Direction::Right;
			break;
		case 3:
			SetAnimation(mario, mario->GetCurrentMStateAnimValue() +
				mario->GetCurrentPStateAnimValue());
			mario->SetDirection(1);
			m_heldEntity->GetAnimator()->ResetDepth();
			m_dir = Direction::Right;
			break;
		}
	}

	mario->SetVelocity(vel);
	m_heldEntity->SetPosition(mario->GetPosition() +
		GetHeldEntityOffset(mario, state));

	Troopa* troopas = dynamic_cast<Troopa*>(m_heldEntity);

	if (troopas && troopas->GetState() == TroopaState::SHELL_IDLE) {
		CheckHeldShellCollisions(mario, troopas, dt);
	}
}

void MarioHoldState::CheckHeldShellCollisions(Mario* mario, Troopa* shell,
	float dt) {

	Vector2 shellPos = shell->GetPosition();
	Vector2 shellSize = shell->GetSize();

	Rectangle shellRect;
	shellRect.x = (long)(shellPos.x - shellSize.x / 2);
	shellRect.y = (long)(shellPos.y - shellSize.y / 2);
	shellRect.width = (long)shellSize.x;
	shellRect.height = (long)shellSize.y;

	std::vector<std::pair<int, int>> cells =
		Collision::GetInstance()->GetEntityCells(shell, dt);

	for (const auto& cell : cells) {
		auto& gridEntities =
			Collision::GetInstance()->GetGrid()[cell.first][cell.second].entities;

		for (Entity* other : gridEntities) {
			Block* block = dynamic_cast<Block*>(other);

			if (other == mario || other == shell || !other->IsActive() ||
				!other->IsCollidable() || block || !other) {
				continue;
			}

			Rectangle otherRect = other->GetCollisionComponent()->GetRectangle();

			if (shellRect.x < otherRect.x + otherRect.width &&
				shellRect.x + shellRect.width > otherRect.x &&
				shellRect.y < otherRect.y + otherRect.height &&
				shellRect.y + shellRect.height > otherRect.y) {
				CollisionResult fakeResult;
				fakeResult.collided = true;
				fakeResult.collidedWith = other;

				other->Die(DyingType::BONKED);
				shell->Die(DyingType::BONKED);
				EffectManager::GetInstance()->CreateBonkEffect(other->GetPosition());
			}
		}
	}
}

Vector2 MarioHoldState::GetHeldEntityOffset(Mario* mario, const int& state) {
	Vector2 size = m_heldEntity->GetSize();
	Vector2 mSize = mario->GetSize();

	// state / 4.0f * (1 - (-1)) + (-1), get direction transition lerp value
	// TODO: FIX THIS LOGIC
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
			vel.x += Player::ACCEL_FRIC * dt;
			if (vel.x > 0)
				vel.x = 0;
		}
		else if (vel.x > 0) {
			vel.x -= Player::ACCEL_FRIC * dt;
			if (vel.x < 0)
				vel.x = 0;
		}
	}

	mario->SetVelocity(vel);
	MarioMovementState::Update(mario, dt);
}

std::unique_ptr<MarioMovementState> MarioHoverState::HandleInput(Mario* mario) {
	auto input = mario->GetInput();
	auto vel = mario->GetVelocity();
	bool isGrounded = mario->IsGrounded();

	if (input->isAPressed) {
		if (m_hoverTimer <= Player::HOVER_TIMER_THRESHOLD) {
			m_isHovering = true;
			mario->SetAnimation(
				mario->GetCurrentPStateAnimValue() + ID_ANIM_MARIO_HOVER, true);
		}
		m_hoverTimer = 0.0f;
	}

	if (isGrounded) {
		return std::make_unique<MarioWalkState>(GetDirection());
	}

	return nullptr;
}

void MarioHoverState::Update(Mario* mario, float dt) {
	m_hoverTimer += dt;
	auto input = mario->GetInput();
	Vector2 vel = mario->GetVelocity();

	// Handle horizontal movement in air
	bool isLeftDown = input->isLeftDown;
	bool isRightDown = input->isRightDown;

	if (isLeftDown != isRightDown) {
		Direction hitDir = isLeftDown ? Direction::Left : Direction::Right;

		float speedCap =
			input->isBDown ? Player::MAX_RUN_SPEED : Player::MAX_WALK_SPEED;

		// Apply air control - same speed and accel as walk
		vel.x += (int)hitDir * Player::ACCEL_NORMAL * dt;

		if (std::abs(vel.x) > speedCap) {
			vel.x -= (vel.x > 0 ? 1 : -1) * Player::ACCEL_FRIC * dt;
		}
	}

	// Handle slowed descending
	if (m_isHovering) {
		if (vel.y > Player::HOVER_MAX_FALL_SPEED) {
			vel.y = Player::HOVER_MAX_FALL_SPEED;
		}
		if (m_hoverTimer > Player::HOVER_TIMER_LENGTH) {
			m_isHovering = false;
			SetAnimation(mario, mario->GetCurrentPStateAnimValue() +
				ID_ANIM_MARIO_JUMP + ID_ANIM_MARIO_JUMP_ALT);
		}
	}

	mario->SetVelocity(vel);
	MarioMovementState::Update(mario, dt);
}

std::unique_ptr<MarioMovementState> MarioFlyState::HandleInput(Mario* mario) {
	auto input = mario->GetInput();
	bool isGrounded = mario->IsGrounded();

	if (isGrounded) {
		return std::make_unique<MarioWalkState>(GetDirection());
	}

	if (input->isADown && m_isFlying) {
		mario->SetVelocity(Vector2(mario->GetVelocity().x, -Player::FLY_UP_SPEED));
	}

	if (input->isAPressed && m_isFlying) {
		if (m_flapTimer <= Player::FLY_FLAP_TIMER_THRESHOLD) {
			mario->SetAnimation(
				mario->GetCurrentPStateAnimValue() + ID_ANIM_MARIO_FLAP, true);
		}
		m_flapTimer = 0.0f;
	}

	return nullptr;
}

void MarioFlyState::Update(Mario* mario, float dt) {
	mario, dt;
	if (m_isFlying) m_flyTimer += dt;
	m_flapTimer += dt;

	auto input = mario->GetInput();
	Vector2 vel = mario->GetVelocity();

	// Handle horizontal movement in air
	bool isLeftDown = input->isLeftDown;
	bool isRightDown = input->isRightDown;

	if (isLeftDown != isRightDown) {
		Direction hitDir = isLeftDown ? Direction::Left : Direction::Right;

		float speedCap = Player::MAX_RUN_SPEED;

		// Apply air control - same speed and accel as walk
		vel.x += (int)hitDir * Player::ACCEL_NORMAL * dt;

		if (std::abs(vel.x) > speedCap) {
			vel.x -= (vel.x > 0 ? 1 : -1) * Player::ACCEL_FRIC * dt;
		}
	}

	if (m_flyTimer <= Player::FLY_TIMER_LENGTH) {
		//fly
	}
	else {
		m_isFlying = false;
		SetAnimation(mario, mario->GetCurrentPStateAnimValue() +
			mario->GetCurrentMStateAnimValue());
	}

	mario->SetVelocity(vel);
	MarioMovementState::Update(mario, dt);
}

std::unique_ptr<MarioMovementState> MarioSweepState::HandleInput(Mario* mario) {
	mario;

	if (m_sweepTimer > Player::SWEEP_TIMER_LENGTH) {
		return std::make_unique<MarioIdleState>(GetDirection());
	}

	return nullptr;
}

void MarioSweepState::Update(Mario* mario, float dt) {
	Vector2 vel = mario->GetVelocity();
	bool isGrounded = mario->IsGrounded();
	auto input = mario->GetInput();
	bool isLeftDown = input->isLeftDown;
	bool isRightDown = input->isRightDown;
	bool isPMeterFull = HeadUpDisplay::GetInstance()->IsPMeterFull();

	m_sweepTimer += dt;

	if (m_sweepTimer > 0.2f) {
		mario->GetAnimator()->SetFlipHorizontal(!m_originalFlipState);
	}
	if (m_sweepTimer > 0.4f) {
		mario->GetAnimator()->SetFlipHorizontal(m_originalFlipState);
	}
	Sweep(mario, dt);

	if (isLeftDown != isRightDown) {
		Direction hitDir = isLeftDown ? Direction::Left : Direction::Right;
		float absDX = std::abs(vel.x);

		// Accelerate up to run speed
		if (absDX < Player::MAX_RUN_SPEED) {
			vel.x += (int)hitDir * Player::ACCEL_NORMAL * dt;
		}
		// Maintain run speed
		else if (absDX > Player::MAX_RUN_SPEED && isGrounded) {
			// No slowing down in run state unless switching directions
			if ((vel.x > 0 && hitDir == Direction::Right) ||
				(vel.x < 0 && hitDir == Direction::Left)) {
				// Maintain current speed
				if (isPMeterFull && absDX < Player::MAX_SPRINT_SPEED) {
					// continue accelerating when pmeter full and
					vel.x += (int)hitDir * Player::ACCEL_NORMAL * dt;
				}
			}
		}
		else {
			// Slow down if changing direction
			vel.x -= (vel.x > 0 ? 1 : -1) * Player::ACCEL_FRIC * dt;
		}
	}
	// No direction input, apply friction
	else if (isGrounded) {
		if (vel.x < 0) {
			vel.x += Player::ACCEL_FRIC * dt;
			if (vel.x > 0)
				vel.x = 0;
		}
		else if (vel.x > 0) {
			vel.x -= Player::ACCEL_FRIC * dt;
			if (vel.x < 0)
				vel.x = 0;
		}
	}

	MarioMovementState::Update(mario, dt);
}

void MarioSweepState::Sweep(Mario* mario, float dt) const {
	Vector2 pos = mario->GetPosition();
	float rayLength = 16.f;

	// Get cells around the entity
	std::vector<std::pair<int, int>> cells =
		Collision::GetInstance()->GetEntityCells(mario, dt);

	for (const auto& cell : cells) {
		for (Entity* other : Collision::GetInstance()
			->GetGrid()[cell.first][cell.second]
			.entities) {
			// Skip if it's the same entity or not a static object (potential hit)
			if (other != mario && !other->IsStatic() && other->IsActive() &&
				other->IsCollidable()) {
				Rectangle otherRect = other->GetCollisionComponent()->GetRectangle();
				Vector2 rayStart = Vector2(pos.x, pos.y + 6.f);
				Vector2 rayEnd = rayStart;
				if (m_sweepTimer < Player::SWEEP_TIMER_HIT_LEFT)
					rayEnd += Vector2(rayLength, 0.f);
				else {
					rayEnd += Vector2(-rayLength, 0.f);
				}
				Vector2 contactPoint, contactNormal;
				float contactTime;

				if (Collision::GetInstance()->RayVsRect(rayStart, rayEnd, otherRect,
					contactPoint, contactNormal,
					contactTime) &&
					contactTime < 1.0f) {
					// kill hit entity
					Enemy* enemy = dynamic_cast<Enemy*>(other);
					if (enemy) {
						enemy->Die(DyingType::BONKED);
						EffectManager::GetInstance()->CreateBonkEffect(
							other->GetPosition());
					}
				}
			}
		}
	}
}