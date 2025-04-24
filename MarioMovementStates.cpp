#include "pch.h"
#include "MarioMovementStates.h"

#include "Mario.h"

using namespace DirectX;
using Keys = DirectX::Keyboard::Keys;

#define UP Keys::W
#define DOWN Keys::S
#define LEFT Keys::A
#define RIGHT Keys::D

#define START Keys::I

#define B Keys::J
#define A Keys::K


//State* MarioIdleState::OnKeyDown(Entity* mario, int keyCode) {
//	Mario* m = (Mario*)mario;
//	switch (keyCode)
//	{
//	case Keys::W:
//		break;
//	case Keys::A:
//		return new MarioWalkState(-1);
//		break;
//	case Keys::S:
//		return new MarioSitState(m_dir);
//		break;
//	case Keys::D:
//		return new MarioWalkState(1);
//		break;
//	case Keys::J:
//		break;
//	case Keys::K:
//		if (m->IsGrounded())// and jump debounce span
//		{
//			return new MarioJumpState(m_dir);
//		}
//		break;
//	default:
//		break;
//	}
//}

State* MarioIdleState::HandleInput(KBState::KeyboardStateTracker* kbState)
{
	if (kbState->IsKeyPressed(UP)) {
		return new MarioWalkState(Exit(), -1);
	}
	if (kbState->IsKeyPressed(RIGHT)) {
		return new MarioWalkState(Exit(), 1);
	}
	if (kbState->IsKeyPressed(DOWN)) {
		return new MarioSitState(Exit(), m_dir);
	}
	if (kbState->IsKeyPressed(B)) {
		if (GetMario()->IsGrounded()) {
			return new MarioJumpState(Exit(), m_dir);
		}
	}
}

void MarioIdleState::Update(float dt)
{
	//call animator to render corresponding sprite and also pass in direction to flip accordingly
}

void MarioIdleState::Enter()
{
	//switch sprite/ kill mario/ decrease health etc.
}


//State* MarioWalkState::OnKeyDown(Entity* mario, int keyCode) {
//	Mario* m = (Mario*)mario;
//	switch (keyCode)
//	{
//	case Keys::W:
//		break;
//	case Keys::A:
//		if (m_dir == 1 && m->IsGrounded())
//			return new MarioSkidState(1);
//		break;
//	case Keys::S:
//		break;
//	case Keys::D:
//		if (m_dir == -1 && m->IsGrounded())
//			return new MarioSkidState(-1);
//		break;
//	case Keys::J:
//		return new MarioRunState(m_dir);
//		break;
//	case Keys::K:
//		if (m->IsGrounded())// and jump debounce span
//		{
//			return new MarioJumpState(m_dir);
//		}
//		break;
//	default:
//		break;
//	}
//}
//State* MarioWalkState::OnKeyUp(Entity* mario, int keyCode) {
//	Mario* m = (Mario*)mario;
//	switch (keyCode)
//	{
//	case Keys::W:
//		break;
//	case Keys::A:
//		if (m->IsGrounded())
//			return new MarioIdleState(m_dir);
//		break;
//	case Keys::S:
//		break;
//	case Keys::D:
//		if (m->IsGrounded())
//			return new MarioIdleState(m_dir);
//		break;
//	case Keys::J:
//		break;
//	case Keys::K:
//		break;
//	default:
//		break;
//	}
//}
//
//
//State* MarioRunState::OnKeyDown(Entity* mario, int keyCode) {
//	Mario* m = (Mario*)mario;
//	switch (keyCode)
//	{
//	case Keys::W:
//		break;
//	case Keys::A:
//		if (m_dir == 1 && m->IsGrounded())
//			return new MarioSkidState(1);
//		break;
//	case Keys::S:
//		break;
//	case Keys::D:
//		if (m_dir == -1 && m->IsGrounded())
//			return new MarioSkidState(-1);
//		break;
//	case Keys::J:
//		break;
//	case Keys::K:
//		if (m->IsGrounded())// and jump debounce span
//		{
//			return new MarioJumpState(m_dir);
//		}
//		break;
//	default:
//		break;
//	}
//}
////State* MarioRunState::OnKeyUp(Entity* mario, int keyCode) {
////	return nullptr;
////}
//
//
//State* MarioSkidState::OnKeyDown(Entity* mario, int keyCode) {
//	Mario* m = (Mario*)mario;
//	switch (keyCode)
//	{
//	case Keys::W:
//		break;
//	case Keys::A:
//		return new MarioWalkState(-1);
//		break;
//	case Keys::S:
//		break;
//	case Keys::D:
//		return new MarioWalkState(1);
//		break;
//	case Keys::J:
//		break;
//	case Keys::K:
//		if (m->IsGrounded())// and jump debounce span
//		{
//			return new MarioJumpState(m_dir);
//		}
//		break;
//	default:
//		break;
//	}
//}
////State* MarioSkidState::OnKeyUp(Entity* mario, int keyCode) {
////	return nullptr;
////}
//
//State* MarioJumpState::OnKeyDown(Entity* mario, int keyCode) {
//	Mario* m = (Mario*)mario;
//	switch (keyCode)
//	{
//	case Keys::W:
//		break;
//	case Keys::A:
//		return new MarioWalkState(-1);
//		break;
//	case Keys::S:
//		break;
//	case Keys::D:
//		return new MarioWalkState(1);
//		break;
//	case Keys::J:
//		break;
//	case Keys::K:
//		break;
//	default:
//		break;
//	}
//}
////State* MarioJumpState::OnKeyUp(Entity* mario, int keyCode) {
////	return nullptr;
////};
//
//State* MarioSitState::OnKeyDown(Entity* mario, int keyCode) {
//	Mario* m = (Mario*)mario;
//	switch (keyCode)
//	{
//	case Keys::W:
//		break;
//	case Keys::A:
//		break;
//	case Keys::S:
//		break;
//	case Keys::D:
//		break;
//	case Keys::J:
//		break;
//	case Keys::K:
//		if (m->IsGrounded())// and jump debounce span
//		{
//			return new MarioJumpState(m_dir);
//		}
//		break;
//	default:
//		break;
//	}
//}
//State* MarioSitState::OnKeyUp(Entity* mario, int keyCode) {
//	Mario* m = (Mario*)mario;
//	switch (keyCode)
//	{
//	case Keys::W:
//		break;
//	case Keys::A:
//		break;
//	case Keys::S:
//		if (m->IsGrounded())
//			return new MarioIdleState(m_dir);
//		break;
//	case Keys::D:
//		break;
//	case Keys::J:
//		break;
//	case Keys::K:
//		break;
//	default:
//		break;
//	}
//}

Mario* MarioStateBase::GetMario()
{
	return dynamic_cast<Mario*>(m_entity);
}

int MarioStateBase::GetDirection() const
{
	return m_dir;
}
