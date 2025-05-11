#include "pch.h"
#include "MarioStateBase.h"
#include "Mario.h"

void MarioStateBase::SetAnimation(Mario* mario, int animId) const {
    mario->SetAnimId(animId);
    mario->SetAnimation(mario->GetAnimId(), true);
    //Log(__FUNCTION__, "Mario " + GetStateName() + ", anim id: " + std::to_string(mario->GetAnimId()));
}

void MarioStateBase::Enter(Mario* mario) {
    Log("State Machine", "Entering state: " + GetStateName());
    mario;
}
