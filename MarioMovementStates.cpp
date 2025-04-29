#include "pch.h"
#include "MarioMovementStates.h"
#include "Debug.h"
#include "AssetIDs.h"

#include "Mario.h"

using namespace DirectX;
using Keys = DirectX::Keyboard;

#define UP			Keys::W
#define DOWN		Keys::S
#define LEFT		Keys::A
#define RIGHT		Keys::D

#define START		Keys::I

#define B			Keys::J
#define A			Keys::K

constexpr auto DIR_LEFT  = -1;
constexpr auto DIR_RIGHT = 1;

void MarioMovementState::Update(Mario* mario, float dt)
{
	//apply universal forces on mario


	
}

MarioMovementState* MarioIdleState::HandleInput(Mario* mario, KBState::KeyboardStateTracker* kbState)
{
	if (kbState->IsKeyPressed(LEFT)) {
		return new MarioWalkState(DIR_LEFT);
	}
	if (kbState->IsKeyPressed(RIGHT)) {
		return new MarioWalkState(DIR_RIGHT);
	}
	if (kbState->IsKeyPressed(DOWN)) {
		return new MarioSitState(m_dir);
	}
	if (kbState->IsKeyPressed(A)) {
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
	//clean up velocity/ acceleration etc.
	mario->SetAnimId(mario->GetAnimId() - ID_ANIM_MARIO_IDLE);
}

MarioMovementState* MarioWalkState::HandleInput(Mario* mario, KBState::KeyboardStateTracker* kbState)
{
	if (kbState->IsKeyPressed(UP)) {
	}
	else if (kbState->IsKeyReleased(UP)) {
	}

	if (kbState->IsKeyPressed(DOWN)) {
		return new MarioSitState(m_dir);
	}
	else if (kbState->IsKeyReleased(DOWN)) {
	}

	if (kbState->IsKeyPressed(LEFT)) {
		if (GetDirection() == DIR_RIGHT && mario->IsGrounded()) {
			return new MarioSkidState(DIR_RIGHT);
		}
	}
	else if (kbState->IsKeyReleased(LEFT)) {
	}

	if (kbState->IsKeyPressed(RIGHT)) {
		if (GetDirection() == DIR_LEFT && mario->IsGrounded()) {
			return new MarioSkidState(DIR_LEFT);
		}
	}
	else if (kbState->IsKeyReleased(RIGHT)) {
	}

	if (kbState->IsKeyPressed(A)) {
		if (mario->IsGrounded()) {
			return new MarioJumpState(m_dir);
		}
	}
	else if (kbState->IsKeyReleased(A)) {
	}

	if (kbState->IsKeyPressed(B)) {
		return new MarioRunState(m_dir);
	}
	else if (kbState->IsKeyReleased(B)) {
	}

	return nullptr;
}

void MarioWalkState::Update(Mario* mario, float dt)
{
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

MarioMovementState* MarioRunState::HandleInput(Mario* mario, KBState::KeyboardStateTracker* kbState)
{
	if (kbState->IsKeyReleased(B)) {
		return new MarioWalkState(m_dir);
	}
	if (kbState->IsKeyPressed(A) && mario->IsGrounded()) {
		return new MarioJumpState(m_dir);
	}
	return nullptr;
}

void MarioRunState::Update(Mario* mario, float dt)
{
	// Update Mario's running animation and speed
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

MarioMovementState* MarioSkidState::HandleInput(Mario* mario, KBState::KeyboardStateTracker* kbState)
{
	if (kbState->IsKeyPressed(LEFT) && m_dir == DIR_RIGHT) {
		return new MarioWalkState(DIR_LEFT);
	}
	if (kbState->IsKeyPressed(RIGHT) && m_dir == DIR_LEFT) {
		return new MarioWalkState(DIR_RIGHT);
	}
	return nullptr;
}

void MarioSkidState::Update(Mario* mario, float dt)
{
	// Update Mario's skidding animation and deceleration
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

MarioMovementState* MarioJumpState::HandleInput(Mario* mario, KBState::KeyboardStateTracker* kbState)
{
	if (kbState->IsKeyPressed(LEFT)) {
		return new MarioWalkState(DIR_LEFT);
	}
	if (kbState->IsKeyPressed(RIGHT)) {
		return new MarioWalkState(DIR_RIGHT);
	}
	return nullptr;
}

void MarioJumpState::Update(Mario* mario, float dt)
{
	// Update Mario's jumping animation and vertical movement
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

MarioMovementState* MarioSitState::HandleInput(Mario* mario, KBState::KeyboardStateTracker* kbState)
{
	if (kbState->IsKeyReleased(DOWN)) {
		return new MarioIdleState(m_dir);
	}
	return nullptr;
}

void MarioSitState::Update(Mario* mario, float dt)
{
	
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
