#include "pch.h"
#include "Debug.h"
#include "Mario.h"
#include "MarioStateBase.h"

void MarioStateBase::SetAnimation(Mario* mario, int animId) const {
	mario->SetAnimId(animId);
	//Log(__FUNCTION__, "Mario " + GetStateName() + ", anim id: " + std::to_string(mario->GetAnimId()));
}

void MarioStateBase::Enter(Mario* mario) {
	Log("State Machine", "Entering state: " + GetStateName());
	mario;
}
