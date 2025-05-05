#pragma once

// Entities IDs
constexpr auto ID_ENT_MARIO                         = 0;
constexpr auto ID_ENT_BOOMERANG_BRO                 = 1;
constexpr auto ID_ENT_GREEN_PARATROOPA_HOPPER       = 2;
constexpr auto ID_ENT_GREEN_TROOPA                  = 3;
constexpr auto ID_ENT_RED_TROOPA                    = 4;
constexpr auto ID_ENT_GOOMBA                        = 5;
constexpr auto ID_ENT_PARAGOOMBA                    = 6;
constexpr auto ID_ENT_GREEN_PIRANHA                 = 7;
constexpr auto ID_ENT_GREEN_PIRANHA_SPIT_FIRE       = 8;
constexpr auto ID_ENT_RED_PIRANHA_SPIT_FIRE         = 9;
constexpr auto ID_ENT_GROUND                        = 10;

constexpr auto ID_ENT_BUSH                          = 11;

constexpr auto ID_ENT_SCREW_BLOCK                   = 25;

constexpr auto ID_ENT_CLOUD                         = 15;
constexpr auto ID_ENT_BG_CLOUD_BIG                  = 16;

constexpr auto ID_ENT_BG_LIL_CLOUD                  = 17;
constexpr auto ID_ENT_BG_WIDE_CLOUD                 = 18;
constexpr auto ID_ENT_BG_GOAL                       = 19;
constexpr auto ID_ENT_GOAL_CARD                     = 20;
constexpr auto ID_ENT_QUESTION_BOX                  = 21;

constexpr auto ID_ENT_GRASS_BLOCK                   = 23;
constexpr auto ID_ENT_BRICK                         = 24;
constexpr auto ID_ENT_SCREW_BLOCK_HOVER_PLATFORM           = 26;
constexpr auto ID_ENT_PIPE                          = 27;
constexpr auto ID_ENT_COIN                          = 28;

//Entities' animation IDs
constexpr auto ID_ANIM_MARIO_SMALL                  = 100;
constexpr auto ID_ANIM_MARIO_BIG                    = 101;
constexpr auto ID_ANIM_MARIO_RACCOON                = 102;

constexpr auto ID_ANIM_MARIO_IDLE                   = 10;
constexpr auto ID_ANIM_MARIO_WALK                   = 20;
constexpr auto ID_ANIM_MARIO_RUN                    = 30;
constexpr auto ID_ANIM_MARIO_SKID                   = 40;
constexpr auto ID_ANIM_MARIO_JUMP                   = 50;
constexpr auto ID_ANIM_MARIO_SIT                    = 60;
constexpr auto ID_ANIM_MARIO_FLY                    = 70;
constexpr auto ID_ANIM_MARIO_HOLD                   = 80;

constexpr auto ID_SPRITE_GROUND_TOP_LEFT            = 1000;
constexpr auto ID_SPRITE_GROUND_TOP_MID				= 1010;
constexpr auto ID_SPRITE_GROUND_TOP_RIGHT			= 1020;
constexpr auto ID_SPRITE_GROUND_BOT_LEFT			= 1030;
constexpr auto ID_SPRITE_GROUND_BOT_MID				= 1040;
constexpr auto ID_SPRITE_GROUND_BOT_RIGHT			= 1050;
constexpr auto ID_SPRITE_INDEPENDENT_PLATFORM		= 1060;

// cactus
constexpr auto ID_SPRITE_BUSH_LITTLE				= 1100;

constexpr auto ID_SPRITE_BUSH_LEFT_SIDE				= 1110;
constexpr auto ID_SPRITE_BUSH_RIGHT_SIDE			= 1120;
constexpr auto ID_SPRITE_BUSH_MID   				= 1130;

constexpr auto ID_SPRITE_BUSH_OUTER_TOP_LEFT    	= 1140;
constexpr auto ID_SPRITE_BUSH_OUTER_TOP_RIGHT		= 1150;

constexpr auto ID_SPRITE_BUSH_INNER_TOP_LEFT		= 1160;
constexpr auto ID_SPRITE_BUSH_INNER_TOP_RIGHT		= 1170;

constexpr auto ID_SPRITE_END_BUSH_LEFT_SIDE	        = 1101;
constexpr auto ID_SPRITE_END_BUSH_RIGHT_SIDE	    = 1111;
constexpr auto ID_SPRITE_END_BUSH_TOP_LEFT	        = 1121;
constexpr auto ID_SPRITE_END_BUSH_TOP_RIGHT 	    = 1131;

//platforms
/// pink
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_MID_LEFT	       = 2510;
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_MID_RIGHT	       = 2520;
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_MID_MID		   = 2530;
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_TOP_LEFT		   = 2540;
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_TOP_RIGHT	       = 2550;
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_TOP_MID		   = 2560;
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_BOT_LEFT		   = 2570;
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_BOT_RIGHT	       = 2580;
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_BOT_MID		   = 2590;


/// blue
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_MID_LEFT         = 2511;
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_MID_RIGHT        = 2521;
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_MID_MID          = 2531;
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_TOP_LEFT         = 2541;
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_TOP_RIGHT        = 2551;
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_TOP_MID          = 2561;
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_BOT_LEFT         = 2571;
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_BOT_RIGHT        = 2581;
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_BOT_MID          = 2591;


/// green
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_MID_LEFT        = 2512;
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_MID_RIGHT       = 2522;
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_MID_MID         = 2532;
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_TOP_LEFT        = 2542;
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_TOP_RIGHT       = 2552;
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_TOP_MID         = 2562;
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_BOT_LEFT        = 2572;
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_BOT_RIGHT       = 2582;
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_BOT_MID         = 2592;

/// white
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_MID_LEFT        = 2513;
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_MID_RIGHT       = 2523;
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_MID_MID         = 2533;
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_TOP_LEFT        = 2543;
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_TOP_RIGHT       = 2553;
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_TOP_MID         = 2563;
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_BOT_LEFT        = 2573;
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_BOT_RIGHT       = 2583;
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_BOT_MID         = 2593;

/// shade
constexpr auto ID_SPRITE_SCREW_BLOCK_SHADE_LEFT            = 2514;
constexpr auto ID_SPRITE_SCREW_BLOCK_SHADE_TOP             = 2524;
constexpr auto ID_SPRITE_SCREW_BLOCK_SHADE_BOT_LEFT        = 2534;
constexpr auto ID_SPRITE_SCREW_BLOCK_SHADE_TOP_LEFT        = 2544;
constexpr auto ID_SPRITE_SCREW_BLOCK_SHADE_TOP_RIGHT       = 2554;

// end of shade

//Clouds
constexpr auto ID_SPRITE_CLOUD_TOP_LEFT			= 1500;
constexpr auto ID_SPRITE_CLOUD_TOP_MID			= 1510;
constexpr auto ID_SPRITE_CLOUD_TOP_RIGHT		= 1520;
constexpr auto ID_SPRITE_CLOUD_BOT_LEFT			= 1530;
constexpr auto ID_SPRITE_CLOUD_BOT_MID			= 1540;
constexpr auto ID_SPRITE_CLOUD_BOT_RIGHT		= 1550;

constexpr auto ID_SPRITE_CLOUD_FLOWER	    	= 1501;
constexpr auto ID_SPRITE_CLOUD_STAR			    = 1502;
constexpr auto ID_SPRITE_CLOUD_MUSHROOM		    = 1503;

