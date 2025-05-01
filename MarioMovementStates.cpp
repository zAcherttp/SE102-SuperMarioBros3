#include "pch.h"
#include "MarioMovementStates.h"
#include "Debug.h"
#include "AssetIDs.h"

#include "Mario.h"

using namespace DirectX;
using Keys = DirectX::Keyboard;
using KBState = DirectX::Keyboard::State;
using KBSTracker = DirectX::Keyboard::KeyboardStateTracker;

#define UP			Keys::W
#define DOWN		Keys::S
#define LEFT		Keys::A
#define RIGHT		Keys::D

#define START		Keys::I

#define B_BTN			Keys::J
#define A_BTN			Keys::K

constexpr auto DIR_LEFT  = -1;
constexpr auto DIR_RIGHT = 1;

constexpr auto PLAYER_TOPWALKSPEED	= 67.5f;			//Highest X velocity considered as Player "walking"
constexpr auto PLAYER_TOPRUNSPEED	= 105.f;			//Highest X velocity when Player runs
constexpr auto PLAYER_TOPPOWERSPEED = 142.5f;			//Highest X velocity hit when Player is at full "power"
constexpr auto PLAYER_MAXSPEED		= 150.f;			//Player's maximum speed
constexpr auto PLAYER_JUMP			= 142.5f;			//Player's root Y velocity for jumping (further adjusted a bit by Player_SpeedJumpInc)
constexpr auto PLAYER_TAILWAG_YVEL	= 37.5f;			//The Y velocity that the tail wag attempts to lock you at
constexpr auto PLAYER_FLY_YVEL		= 67.5f;			//The velocity the Player flies at

constexpr auto PLAYER_ACCELERATION	= 52.5f;			//Player normal accel
constexpr auto PLAYER_FRICTION		= 52.5f;			//Player Friction accel
constexpr auto PLAYER_FRICTION_SKID	= 120.f;			//Player Friction skid


void MarioMovementState::Update(Mario* mario, float dt)
{
	//apply universal forces on mario
	float velX = mario->GetVelocity().x;
	m_dir = (velX == 0) ? m_dir : ((velX > 0) ? 1 : -1);
	mario->GetAnimator()->SetFlipHorizontal(m_dir > 0);
	mario->SetPosition(mario->GetPosition() + mario->GetVelocity() * dt);
}

MarioMovementState* MarioIdleState::HandleInput(Mario* mario, KBState* kbState, KBSTracker* kbsTracker)
{
	if (kbsTracker->IsKeyPressed(LEFT) || kbState->IsKeyDown(LEFT)) {
		return new MarioWalkState(DIR_LEFT);
	}
	if (kbsTracker->IsKeyPressed(RIGHT) || kbState->IsKeyDown(RIGHT)) {
		return new MarioWalkState(DIR_RIGHT);
	}
	if (kbsTracker->IsKeyPressed(DOWN)) {
		return new MarioSitState(m_dir);
	}
	if (kbsTracker->IsKeyPressed(A_BTN)) {
		Mario* m = (Mario*)mario;
		if (m->IsGrounded()) {
			return new MarioJumpState(m_dir);
		}
	}

	return nullptr;
}

void MarioIdleState::Update(Mario* mario, float dt)
{
	//call animator to render corresponding sprite and also pass in direction to flip accordingly
	MarioMovementState::Update(mario, dt);
}

void MarioIdleState::Enter(Mario* mario)
{  
	//switch sprite/ kill mario/ decrease health etc.
	// later on powerup state will add/ subtract its anim ids, currently default to small mario
	mario->SetAnimId(mario->GetAnimId() + ID_ANIM_MARIO_IDLE);
	mario->SetAnimation(mario->GetAnimId(), true);

	Log(__FUNCTION__, "Mario idle, anim id: " + std::to_string(mario->GetAnimId()));
}

void MarioIdleState::Exit(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() - ID_ANIM_MARIO_IDLE);
}

MarioMovementState* MarioWalkState::HandleInput(Mario* mario, KBState* kbState, KBSTracker* kbsTracker)
{
	m_isLeftPressed = kbState->IsKeyDown(LEFT);
	m_isRightPressed = kbState->IsKeyDown(RIGHT);

	if (kbsTracker->IsKeyPressed(LEFT)) {
		if (GetDirection() == DIR_RIGHT && mario->IsGrounded()) {
			return new MarioSkidState(DIR_RIGHT);
		}
	}

	if (kbsTracker->IsKeyPressed(RIGHT)) {
		if (GetDirection() == DIR_LEFT && mario->IsGrounded()) {
			return new MarioSkidState(DIR_LEFT);
		}
	}

	if (kbsTracker->IsKeyPressed(A_BTN)) {
		if (mario->IsGrounded()) {
			return new MarioJumpState(m_dir);
		}
	}

	if (kbsTracker->IsKeyPressed(B_BTN) || kbState->IsKeyDown(B_BTN)) {
		return new MarioRunState(m_dir);
	}

	if (mario->GetVelocity().x == 0) {
		return new MarioIdleState(m_dir);
	}

	return nullptr;
}

void MarioWalkState::Update(Mario* mario, float dt)
{
	// accelerate mario if button is pressed
	if(m_isLeftPressed != m_isRightPressed){
		Vector2 vel = mario->GetVelocity() + Vector2(PLAYER_ACCELERATION, 0) * dt * m_dir;
		if (vel.x > PLAYER_TOPWALKSPEED) vel.x = PLAYER_TOPWALKSPEED;
		if (vel.x < -PLAYER_TOPWALKSPEED) vel.x = -PLAYER_TOPWALKSPEED;
		mario->SetVelocity(vel);
	} else { // else decelerate
		Vector2 vel = mario->GetVelocity();
		if (abs(vel.x) > 0.1f) {
			
			// Calculate new velocity with friction applied
			float newVelX = vel.x - PLAYER_FRICTION * dt * m_dir;
			
			// Check if friction would cause direction change (stop at zero)
			if ((m_dir > 0 && newVelX < 0) || (m_dir < 0 && newVelX > 0)) {
				newVelX = 0;
			}
			
			mario->SetVelocity(Vector2(newVelX, vel.y));
		}
		else {
			mario->SetVelocity(Vector2(0, vel.y));
		}
	}

	MarioMovementState::Update(mario, dt);
}

void MarioWalkState::Enter(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() + ID_ANIM_MARIO_WALK);
	mario->SetAnimation(mario->GetAnimId(), true);
	Log(__FUNCTION__, "Mario walk, anim id: " + std::to_string(mario->GetAnimId()));
}

void MarioWalkState::Exit(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() - ID_ANIM_MARIO_WALK);
}

MarioMovementState* MarioRunState::HandleInput(Mario* mario, KBState* kbState, KBSTracker* kbsTracker)
{
	if (kbsTracker->IsKeyPressed(LEFT)) {
		if (GetDirection() == DIR_RIGHT && mario->IsGrounded()) {
			return new MarioSkidState(DIR_RIGHT);
		}
	}

	if (kbsTracker->IsKeyPressed(RIGHT)) {
		if (GetDirection() == DIR_LEFT && mario->IsGrounded()) {
			return new MarioSkidState(DIR_LEFT);
		}
	}

	if (kbsTracker->IsKeyReleased(B_BTN)) {
		return new MarioWalkState(m_dir);
	}
	if (kbsTracker->IsKeyPressed(A_BTN) && mario->IsGrounded()) {
		return new MarioJumpState(m_dir);
	}
	return nullptr;
}

void MarioRunState::Update(Mario* mario, float dt)
{
	Vector2 vel = mario->GetVelocity() + Vector2(PLAYER_ACCELERATION, 0) * dt * m_dir;
	if (vel.x > PLAYER_TOPRUNSPEED) vel.x = PLAYER_TOPRUNSPEED;
	if (vel.x < -PLAYER_TOPRUNSPEED) vel.x = -PLAYER_TOPRUNSPEED;
	mario->SetVelocity(vel);

	MarioMovementState::Update(mario, dt);
}

void MarioRunState::Enter(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() + ID_ANIM_MARIO_RUN);
	mario->SetAnimation(mario->GetAnimId(), true);
	Log(__FUNCTION__, "Mario run, anim id: " + std::to_string(mario->GetAnimId()));
}

void MarioRunState::Exit(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() - ID_ANIM_MARIO_RUN);
}

MarioMovementState* MarioSkidState::HandleInput(Mario* mario, KBState* kbState, KBSTracker* kbsTracker)
{
	if (kbsTracker->IsKeyPressed(LEFT)) {
		return new MarioWalkState(DIR_LEFT);
	}
	if (kbsTracker->IsKeyPressed(RIGHT)) {
		return new MarioWalkState(DIR_RIGHT);
	}
	if (m_dir * m_lastDir < 0) {
		return new MarioWalkState(m_dir);
	}
	
	return nullptr;
}

void MarioSkidState::Update(Mario* mario, float dt)
{
	mario->SetVelocity(mario->GetVelocity() - Vector2(PLAYER_FRICTION_SKID, 0) * m_lastDir * dt);
	MarioMovementState::Update(mario, dt);
}

void MarioSkidState::Enter(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() + ID_ANIM_MARIO_SKID);
	mario->SetAnimation(mario->GetAnimId(), true);
	Log(__FUNCTION__, "Mario skid, anim id: " + std::to_string(mario->GetAnimId()));
}

void MarioSkidState::Exit(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() - ID_ANIM_MARIO_SKID);
}

MarioMovementState* MarioJumpState::HandleInput(Mario* mario, KBState* kbState, KBSTracker* kbsTracker)
{
	if (kbsTracker->IsKeyPressed(LEFT)) {
		return new MarioWalkState(DIR_LEFT);
	}
	if (kbsTracker->IsKeyPressed(RIGHT)) {
		return new MarioWalkState(DIR_RIGHT);
	}
	return nullptr;
}

void MarioJumpState::Update(Mario* mario, float dt)
{
	// Update Mario's jumping animation and vertical movement
	MarioMovementState::Update(mario, dt);
}

void MarioJumpState::Enter(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() + ID_ANIM_MARIO_JUMP);
	mario->SetAnimation(mario->GetAnimId(), true);
	Log(__FUNCTION__, "Mario jump, anim id: " + std::to_string(mario->GetAnimId()));
}

void MarioJumpState::Exit(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() - ID_ANIM_MARIO_JUMP);
}

MarioMovementState* MarioSitState::HandleInput(Mario* mario, KBState* kbState, KBSTracker* kbsTracker)
{
	if (kbsTracker->IsKeyReleased(DOWN)) {
		return new MarioIdleState(m_dir);
	}
	return nullptr;
}

void MarioSitState::Update(Mario* mario, float dt)
{
	MarioMovementState::Update(mario, dt);
}

void MarioSitState::Enter(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() + ID_ANIM_MARIO_SIT);
	mario->SetAnimation(mario->GetAnimId(), true);
	Log(__FUNCTION__, "Mario sit, anim id: " + std::to_string(mario->GetAnimId()));
}

void MarioSitState::Exit(Mario* mario)
{
	mario->SetAnimId(mario->GetAnimId() - ID_ANIM_MARIO_SIT);
}

int MarioStateBase::GetDirection() const
{
	return m_dir;
}
