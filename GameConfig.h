#pragma once
namespace GameConstants
{
	namespace Player
	{
		constexpr float BASE_JUMP_FORCE = -3.5f * 60.0f;  // Fast gravity when not holding jump
		constexpr float BOUNCE_FORCE = -300.0f;

		// Physics constants
		constexpr auto MAX_WALK_SPEED = 1.5f * 60.0f; // topSpeed when not running
		constexpr auto MAX_RUN_SPEED = 2.5f * 60.0f;  // topSpeed when running
		constexpr auto MAX_SPRINT_SPEED = 3.5f * 60.0f; // topSpeed when sprinting
		constexpr auto MAX_EOL_SPEED = 1.25f * 60.0f; // topSpeed when in End Of Level zone
		constexpr auto MAX_GENERAL_SPEED = 4.0f * 60.0f; // Maximum allowed speed

		constexpr auto HOLD_THROW_SPEED = 4.0f * 60.0f;

		// Jump force values
		constexpr float JUMP_FORCE[] = { -3.5f * 60.0f, -3.625f * 60.0f, -3.75f * 60.0f, -4.0f * 60.0f };

		// Acceleration values
		constexpr auto ACCEL_NORMAL = 14.0f / 256.0f * 60.0f * 60.0f; // Normal acceleration
		constexpr auto ACCEL_FRIC = 20.0f / 256.0f * 60.0f * 60.0f; // Friction deceleration
		constexpr auto ACCEL_SKID = 32.0f / 256.0f * 60.0f * 60.0f; // Skidding deceleration

		// Gravity values
		constexpr auto ACCEL_GRAVITY_SLOW = 1.0f / 16.0f * 60.0f * 60.0f; // Slow gravity when holding jump
		constexpr auto ACCEL_GRAVITY_FAST = 5.0f / 16.0f * 60.0f * 60.0f; // Fast gravity when not holding jump


		constexpr float HOVER_TIMER_THRESHOLD = 0.7f;
		constexpr float HOVER_TIMER_LENGTH = 0.5f;
		constexpr float HOVER_MAX_FALL_SPEED = 1.0f * 60.0f;

		constexpr float SWEEP_TIMER_LENGTH = 0.5f;
		constexpr float SWEEP_TIMER_HIT_RIGHT = 0.0f;
		constexpr float SWEEP_TIMER_HIT_LEFT = 0.2f;
	}

	namespace Blocks {
		namespace QuestionBlock {
			constexpr float BUMP_TIME = 1.f;
			constexpr float BUMP_VEL = 20.f;
		}
		namespace FloatingPlatform {
			constexpr float ACCEL_FALL = 225.f;
		}
	}

	namespace PowerUps {
		namespace Mushroom {
			constexpr float GRAVITY = 9.8f * 60.0f;
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
			constexpr float GRAVITY = 9.8f * 60.0f;
		}
		namespace ParaGoomba
		{
			constexpr float GRAVITY = 9.8f * 60.0f;
		}

		namespace Koopa
		{
		}

		namespace Troopas
		{
			constexpr float WALK_SPEED = 30.0f;
			constexpr float SHELL_SPEED = 60.0f;
			constexpr float GRAVITY = 9.8f * 60.0f;
		}
	}
}

namespace GameStrings
{
	constexpr const char* RACCOON = "raccoon";
	constexpr const char* SMALL = "small";
	constexpr const char* SUPER = "super";

	constexpr const char* DIE = "die";
	constexpr const char* IDLE = "idle";
	constexpr const char* WALK = "walk";
	constexpr const char* RUN = "run";
	constexpr const char* SKID = "skid";
	constexpr const char* JUMP = "jump";
	constexpr const char* SIT = "sit";
	constexpr const char* HOLD = "hold";
	constexpr const char* KICK = "kick";
	constexpr const char* HOVER = "hover";
	constexpr const char* FLY = "fly";
	constexpr const char* SWEEP = "sweep";
}