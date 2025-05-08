#pragma once
namespace GameConfig
{
    namespace Mario
    {
        constexpr float WALK_SPEED = 120.0f;
        constexpr float RUN_SPEED = 180.0f;
        constexpr float STANDARD_JUMP_FORCE = -3.5f * 60.0f;
        constexpr float GRAVITY_SLOW = 6.0f * 60.0f;
        constexpr float GRAVITY_FAST = 12.0f * 60.0f;
    }

    namespace Enemies
    {
        namespace Goomba
        {
            constexpr float WALK_SPEED = 30.0f;
            constexpr float DEATH_ANIMATION_TIME = 0.5f;
            constexpr float COLLISION_DISABLE_TIME = 0.5f;
            // ...more Goomba stats
        }

        namespace Koopa
        {
            // Koopa specific constants
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