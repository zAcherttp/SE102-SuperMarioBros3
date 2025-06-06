#pragma once
namespace GameConstants {
	namespace Player {
		constexpr float BASE_JUMP_FORCE =
			-3.5f * 60.0f; // Fast gravity when not holding jump
		constexpr float BOUNCE_FORCE = -300.0f;

		// Physics constants
		constexpr auto MAX_WALK_SPEED = 1.5f * 60.0f;   // topSpeed when not running
		constexpr auto MAX_RUN_SPEED = 2.5f * 60.0f;    // topSpeed when running
		constexpr auto MAX_SPRINT_SPEED = 3.5f * 60.0f; // topSpeed when sprinting
		constexpr auto MAX_EOL_SPEED =
			1.25f * 60.0f; // topSpeed when in End Of Level zone
		constexpr auto MAX_GENERAL_SPEED = 4.0f * 60.0f; // Maximum allowed speed

		constexpr auto HOLD_THROW_SPEED = 4.0f * 60.0f;

		// Jump force values
		constexpr float JUMP_FORCE[] = { -3.5f * 60.0f, -3.625f * 60.0f, -3.75f * 60.0f,
										-4.0f * 60.0f };

		// Acceleration values
		constexpr auto ACCEL_NORMAL =
			14.0f / 256.0f * 60.0f * 60.0f; // Normal acceleration
		constexpr auto ACCEL_FRIC =
			20.0f / 256.0f * 60.0f * 60.0f; // Friction deceleration
		constexpr auto ACCEL_SKID =
			32.0f / 256.0f * 60.0f * 60.0f; // Skidding deceleration

		// Gravity values
		constexpr auto ACCEL_GRAVITY_SLOW =
			1.0f / 16.0f * 60.0f * 60.0f; // Slow gravity when holding jump
		constexpr auto ACCEL_GRAVITY_FAST =
			5.0f / 16.0f * 60.0f * 60.0f; // Fast gravity when not holding jump

		constexpr float HOVER_TIMER_THRESHOLD = 0.7f;
		constexpr float HOVER_TIMER_LENGTH = 0.5f;
		constexpr float HOVER_MAX_FALL_SPEED = 1.0f * 60.0f;

		constexpr float SWEEP_TIMER_LENGTH = 0.5f;
		constexpr float SWEEP_TIMER_HIT_RIGHT = 0.0f;
		constexpr float SWEEP_TIMER_HIT_LEFT = 0.2f;
	} // namespace Player

	namespace Blocks {
		namespace QuestionBlock {
			constexpr float BUMP_TIME = 1.f;
			constexpr float BUMP_VEL = 20.f;
		} // namespace QuestionBlock
		namespace FloatingPlatform {
			constexpr float ACCEL_FALL = 225.f;
		}
	} // namespace Blocks

	namespace PowerUps {
		namespace Mushroom {
			constexpr float GRAVITY = 9.8f * 60.0f;
		}
	} // namespace PowerUps

	namespace Enemies {
		constexpr float DEATH_STOMP_ANI_TIME = 0.5f;
		constexpr float DEATH_BONK_ANI_TIME = 2.0f;
		constexpr float DEATH_BOUNCE_VELOCITY = 200.0f;
		namespace Goomba {
			constexpr float WALK_SPEED = 30.0f;
			constexpr float COLLISION_DISABLE_TIME = 0.5f;
			constexpr float GRAVITY = 9.8f * 60.0f;
		} // namespace Goomba
		namespace ParaGoomba {
			constexpr float GRAVITY = 9.8f * 60.0f;
			constexpr float SMALL_JUMP_FORCE = -1.5f * 60.0f;
			constexpr float BIG_JUMP_FORCE = -3.5f * 60.0f;
			constexpr float MEDIUM_FLAP_SPEED = 0.17f; 
			constexpr float RAPID_FLAP_SPEED = 0.066f;
			constexpr float RAPID_FLAP_DURATION = 1.0f;
			constexpr float MEDIUM_FLAP_DURATION = 0.2f;
			constexpr float CLOSED_WINGS_DURATION = 0.5f;
			constexpr float FRAME_TIME = 0.15f; 
			constexpr float JUMP_INTERVAL = 0.5f; 
			constexpr int JUMPS_BEFORE_BIG_JUMP = 3;
		}
		namespace Troopas {
			constexpr float WALK_SPEED = 30.0f;
			constexpr float SHELL_SLIDE_SPEED = 210.0f;
			constexpr float BOUNCE_FORCE = 200.0f;
			constexpr float GRAVITY = 9.8f * 60.0f;
			
			// Flying Troopa constants
			constexpr float FLY_HEIGHT = 110.0f;
			constexpr float MAX_FLYING_SPEED = 70.0f;
			constexpr float VERTICAL_ACCELERATION = 100.0f;
			constexpr float WING_FLAP_RATE = 0.1f;
			
			// Size constants
			constexpr float SHELL_SIZE_NORMAL_WIDTH = 16.0f;
			constexpr float SHELL_SIZE_NORMAL_HEIGHT = 16.0f;
			constexpr float SHELL_SIZE_SPECIAL_WIDTH = 10.0f;
			constexpr float SHELL_SIZE_SPECIAL_HEIGHT = 16.0f;
			constexpr float TROOPA_SIZE_WIDTH = 16.0f;
			constexpr float TROOPA_SIZE_HEIGHT = 27.0f;
			
			// Timing constants
			constexpr float REVIVE_TIME = 5.0f;
			constexpr float VIBRATION_TIME = 1.77f;
			constexpr float VIBRATION_INTERVAL = 0.03f;
			constexpr float VIBRATION_AMPLITUDE = 1.0f;
			constexpr int MAX_VIBRATION_COUNT = 20;
			constexpr float FRAME_TIME = 0.15f;
			
			// Position offsets
			constexpr float SHELL_OFFSET_Y = 5.0f;
			constexpr float COLLISION_OFFSET_X = 8.0f;
			constexpr float EDGE_CHECK_OFFSET_Y = 3.0f;
			constexpr float FOOT_OFFSET_DIVISOR = 4.0f; // size.x / 4 and size.y / 2
			constexpr float SIZE_HALF_DIVISOR = 2.0f;
			
			// Raycast constants
			constexpr float RAY_LENGTH = 5.0f;
			constexpr float SWEEP_COLLISION_FORCE = 50.0f;
			constexpr float ENTITY_CELLS_DT = 0.016f;
			
			// Bounce constants
			constexpr float BOUNCE_DAMPING = 0.4f;
			constexpr int MAX_BOUNCE_COUNT = 1;
			
			// Revive phase timing
			constexpr float REVIVE_PHASE_1_TIME = 0.5f;
			constexpr float REVIVE_PHASE_2_TIME = 1.12f;
			
			// Wing constants
			constexpr float WINGS_WIDTH = 16.0f;
			constexpr float WINGS_HEIGHT = 16.0f;
			constexpr float WING_OFFSET_X = 4.0f;
			constexpr float WING_OFFSET_Y = -9.0f;
			
			// Flying physics constants
			constexpr float FLY_DISTANCE_THRESHOLD_FAR = 48.0f;
			constexpr float FLY_DISTANCE_THRESHOLD_NEAR = 28.0f;			constexpr float FLY_ACCELERATION_DIVISOR = 25.0f;
		} // namespace Troopas
		
		namespace BoomerangBro {
			// Boomerang physics constants
			constexpr float HORIZONTAL_SPEED = 220.0f;
			constexpr float MAX_VERTICAL_SPEED = 220.0f;
			constexpr float INITIAL_UPWARD_SPEED = 80.0f;
			constexpr float GRAVITY = 53.0f;
			constexpr float VERTICAL_ACCELERATION = 20.0f;
			constexpr float HORIZONTAL_ACCELERATION = 180.0f;
			
			// Boomerang size constants
			constexpr float BOOMERANG_WIDTH = 16.0f;
			constexpr float BOOMERANG_HEIGHT = 16.0f;
			constexpr float BULLET_WIDTH = 12.0f;
			constexpr float BULLET_HEIGHT = 12.0f;
			
			// Boomerang timing constants
			constexpr float LIFETIME = 10.0f;
			constexpr float PHASE_DURATION = 2.54f;
			
			// Movement constants
			constexpr float PATROL_DISTANCE = 48.0f;
			constexpr float BOOMERANG_OFFSET_X = 16.0f;
		} // namespace BoomerangBro
	} // namespace Enemies

	namespace Physics {
		constexpr float GRAVITY = 9.8f * 60.0f;
	}
} // namespace GameConstants

namespace GameStrings {
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
} // namespace GameStrings