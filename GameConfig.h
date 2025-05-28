#pragma once
namespace GameConfig
{
    namespace Mario
    {
        constexpr float WALK_SPEED = 120.0f;
        constexpr float RUN_SPEED = 180.0f;
        constexpr float STANDARD_JUMP_FORCE = -3.5f * 60.0f;
        constexpr float GRAVITY_SLOW = 1.0f / 16.0f * 60.0f * 60.0f;   // Slow gravity when holding jump
        constexpr float GRAVITY_FAST = 5.0f / 16.0f * 60.0f * 60.0f;   // Fast gravity when not holding jump
        constexpr float BOUNCE_FORCE = -300.0f;
    }

    namespace Blocks {
        namespace QuestionBlock {
            constexpr float BUMP_TIME = 1.f;
            constexpr float BUMP_VEL = 20.f;
        }
    }

    namespace Enemies
    {
        constexpr float DEATH_STOMP_ANI_TIME = 0.5f;
        constexpr float DEATH_BONK_ANI_TIME = 2.0f;
        constexpr float DEATH_BOUNCE_VELOCITY = 200.0f;
        namespace Goomba
        {
            constexpr float WALK_SPEED = 30.0f;
            constexpr float COLLISION_DISABLE_TIME = 0.5f;
            // ...more Goomba stats
        }

        namespace Koopa
        {
            // Koopa specific constants
        }

        namespace Troopas
        {
            constexpr float WALK_SPEED = 30.0f;
            constexpr float SHELL_SPEED = 130.0f;
            constexpr float BOUNCE_FORCE = 200.0f;
        }
        // ...other enemies
    }

    namespace World
    {
        // World/level related constants
    }
    
    namespace Physics
    {
        constexpr float GRAVITY = 9.8f * 60.0f;
        // ...more physics constants
    }
}