#include "pch.h"
#include "MarioMovementStates.h"

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
	OutputDebugString(L"Mario has entered the idle state.\n");  
}

void MarioIdleState::Exit()
{
	//clean up velocity/ acceleration etc.
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
	OutputDebugString(L"Mario has entered the walk state.\n");
}

void MarioWalkState::Exit()
{
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
	OutputDebugString(L"Mario has entered the run state.\n");
}

void MarioRunState::Exit()
{
	// Clean up running-specific properties
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
	OutputDebugString(L"Mario has entered the skid state.\n");
}

void MarioSkidState::Exit()
{
	// Clean up skidding-specific properties
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
	OutputDebugString(L"Mario has entered the jump state.\n");
}

void MarioJumpState::Exit()
{
	// Clean up jumping-specific properties
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
	// Update Mario's sitting animation
}

void MarioSitState::Enter(Mario* mario)
{
	OutputDebugString(L"Mario has entered the sit state.\n");
}

void MarioSitState::Exit()
{
	// Clean up sitting-specific properties
}

int MarioStateBase::GetDirection() const
{
	return m_dir;
}
