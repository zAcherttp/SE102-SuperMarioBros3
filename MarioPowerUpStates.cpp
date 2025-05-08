#include "pch.h"
#include "AssetIDs.h"
#include "Mario.h"
#include "MarioStateFactory.h"
#include "MarioPowerUpStates.h"

std::string MarioSmallState::GetStateName() const { return "small"; }
std::string MarioSuperState::GetStateName() const { return "super"; }
std::string MarioRaccoonState::GetStateName() const { return "raccoon"; }

int MarioSmallState::GetStateAnimValue() const { return ID_ANIM_MARIO_SMALL; }
int MarioSuperState::GetStateAnimValue() const { return ID_ANIM_MARIO_SUPER; }
int MarioRaccoonState::GetStateAnimValue() const { return ID_ANIM_MARIO_RACCOON; }

Vector2 MarioSmallState::GetStateSizeOffset() const { return Vector2(12, 15); }
Vector2 MarioSuperState::GetStateSizeOffset() const { return Vector2(12, 15); }
Vector2 MarioRaccoonState::GetStateSizeOffset() const { return Vector2(12, 15); }

void MarioPowerUpState::Enter(Mario* mario)
{
    mario->SetSize(mario->GetCurrentMStateSizeOffset() + mario->GetCurrentPStateSizeOffset());
    SetAnimation(mario, mario->GetCurrentMStateAnimValue() + mario->GetCurrentPStateAnimValue());
}

void MarioPowerUpState::Exit(Mario* mario)
{
    mario->SetSize(mario->GetCurrentMStateSizeOffset() - mario->GetCurrentPStateSizeOffset());
    SetAnimation(mario, mario->GetCurrentMStateAnimValue() - mario->GetCurrentPStateAnimValue());
}

MarioPowerUpState* MarioSmallState::HandleInput(Mario* mario)
{
    // do absolutely nothing
    return nullptr;
}

void MarioSmallState::Update(Mario* mario, float dt)
{
    // also do absolutely nothing
}

MarioPowerUpState* MarioSuperState::HandleInput(Mario* mario)
{
    // do absolutely nothing
    return nullptr;
}

void MarioSuperState::Update(Mario* mario, float dt)
{
    // also do absolutely nothing
}

MarioPowerUpState* MarioRaccoonState::HandleInput(Mario* mario)
{
    // do absolutely nothing
    return nullptr;
}

void MarioRaccoonState::Update(Mario* mario, float dt)
{
    // also do absolutely nothing
}