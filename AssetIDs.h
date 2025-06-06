#pragma once

// Entities IDs
constexpr auto ID_ENT_MARIO = 0;
constexpr auto ID_ENT_BOOMERANG_BRO = 1;
constexpr auto ID_ENT_GREEN_PARATROOPA_HOPPER = 2;
constexpr auto ID_ENT_GREEN_TROOPA = 3;
constexpr auto ID_ENT_RED_TROOPA = 4;
constexpr auto ID_ENT_GOOMBA = 5;
constexpr auto ID_ENT_PARAGOOMBA = 6;
constexpr auto ID_ENT_BITE_PIRANHA = 7;
constexpr auto ID_ENT_RED_PIRANHA_SPIT_FIRE = 9;
constexpr auto ID_ENT_GROUND = 10;
constexpr auto ID_ENT_BUSH = 11;
constexpr auto ID_ENT_WINGS = 12;
constexpr auto ID_ENT_BULLET = 13;

constexpr auto ID_ENT_BOOMERANG = 14;

constexpr auto ID_ENT_CLOUD = 15;
constexpr auto ID_ENT_SKY_PLATFORM = 16;

constexpr auto ID_ENT_BLACK_BACKGROUND = 17;

constexpr auto ID_ENT_BG_WIDE_CLOUD = 18;
constexpr auto ID_ENT_BG_GOAL = 19;
constexpr auto ID_ENT_END_PORTAL = 22;
constexpr auto ID_ENT_LUCKY_BLOCK = 21;

constexpr auto ID_ENT_GRASS_BLOCK = 23;
constexpr auto ID_ENT_BRICK = 24;
constexpr auto ID_ENT_SCREW_BLOCK = 25;
constexpr auto ID_ENT_SCREW_BLOCK_HOVER_PLATFORM = 26;
constexpr auto ID_ENT_PIPE = 27;
constexpr auto ID_ENT_COIN = 28;

constexpr auto ID_ENT_EFFECT = 29;

constexpr auto ID_ENT_MUSHROOM = 30;
constexpr auto ID_ENT_FIRE_LEAF = 31;
constexpr auto ID_ENT_HIDDEN_BUTTON = 32;
constexpr auto ID_ENT_SECRET_WALL = 33;

constexpr auto ID_ENT_MOVING_PLATFORM = 34;

constexpr auto ID_ENT_HIT_BOX = 35;

// Entities' animation IDs
constexpr auto ID_ANIM_MARIO_SMALL = 100000;
constexpr auto ID_ANIM_MARIO_SUPER = 200000;
constexpr auto ID_ANIM_MARIO_RACCOON = 300000;
constexpr auto ID_ANIM_MARIO_DIE = 400000;

constexpr auto ID_ANIM_MARIO_IDLE = 10;
constexpr auto ID_ANIM_MARIO_WALK = 20;
constexpr auto ID_ANIM_MARIO_SPRINT = 30;
constexpr auto ID_ANIM_MARIO_SKID = 40;
constexpr auto ID_ANIM_MARIO_JUMP = 50;
constexpr auto ID_ANIM_MARIO_JUMP_ALT = 1;
constexpr auto ID_ANIM_MARIO_POWERJUMP = 60;
constexpr auto ID_ANIM_MARIO_SIT = 70;
constexpr auto ID_ANIM_MARIO_FLY = 80;
constexpr auto ID_ANIM_MARIO_HOLD_IDLE = 90;
constexpr auto ID_ANIM_MARIO_HOLD = 1;
constexpr auto ID_ANIM_MARIO_HOLD_JUMP = 2;
constexpr auto ID_ANIM_MARIO_HOLD_FRONT = 3;
constexpr auto ID_ANIM_MARIO_KICK = 100;
constexpr auto ID_ANIM_MARIO_HOVER = 110;
constexpr auto ID_ANIM_MARIO_SWEEP = 120;

// single bonked frame animID = 110 so _HOVER is fine
constexpr auto ID_ANIM_MARIO_SUPER_BONKED = 110;

// Green Troopa
constexpr auto ID_ANIM_GREEN_TROOPAS_WALK = 300;
constexpr auto ID_ANIM_GREEN_TROOPAS_SHELL = 310;
constexpr auto ID_ANIM_GREEN_TROOPAS_SHELL_SLIDE = 320;
constexpr auto ID_ANIM_GREEN_TROOPAS_REVIVE_SLOW = 330;
constexpr auto ID_ANIM_GREEN_TROOPAS_REVIVE_FAST = 340;

// Red Troopa
constexpr auto ID_ANIM_RED_TROOPAS_WALK = 400;
constexpr auto ID_ANIM_RED_TROOPAS_SHELL = 410;
constexpr auto ID_ANIM_RED_TROOPAS_SHELL_SLIDE = 420;
constexpr auto ID_ANIM_RED_TROOPAS_REVIVE_SLOW = 430;
constexpr auto ID_ANIM_RED_TROOPAS_REVIVE_FAST = 440;

// Goomba
constexpr auto ID_ANIM_GOOMBA_WALK = 500;
constexpr auto ID_ANIM_GOOMBA_DIE = 510;

// Paragoomba
constexpr auto ID_ANIM_PARAGOOMBA_WALK = 600;
constexpr auto ID_ANIM_PARAGOOMBA_DIE = 610;

// Wings
constexpr auto ID_ANIM_WINGS_FLAP = 1200;
constexpr auto ID_ANIM_WINGS_FLAP_UP = 1210;   // Up position
constexpr auto ID_ANIM_WINGS_FLAP_DOWN = 1220; // Down position

// Bullet
constexpr auto ID_ANIM_BULLET = 1300;

// Boomerang Bro
constexpr auto ID_ANIM_BOOMERANG_BRO_THROW = 100;
constexpr auto ID_ANIM_BOOMERANG_BRO_WALK = 110;
constexpr auto ID_ANIM_BOOMERANG_BRO_DIE = 120;

// Effects
constexpr auto ID_ANIM_EFFECT_BONK = 2900;
constexpr auto ID_ANIM_EFFECT_PIRANHA_DIE = 2910;
constexpr auto ID_ANIM_EFFECT_POINT_100 = 2920;
constexpr auto ID_ANIM_EFFECT_POINT_200 = 2921;
constexpr auto ID_ANIM_EFFECT_POINT_400 = 2922;
constexpr auto ID_ANIM_EFFECT_POINT_800 = 2923;
constexpr auto ID_ANIM_EFFECT_POINT_1000 = 2924;
constexpr auto ID_ANIM_EFFECT_POINT_2000 = 2925;
constexpr auto ID_ANIM_EFFECT_POINT_4000 = 2926;
constexpr auto ID_ANIM_EFFECT_POINT_8000 = 2927;

constexpr auto ID_ANIM_EFFECT_COIN = 2930;
constexpr auto ID_ANIM_EFFECT_SMOKE = 2940;
constexpr auto ID_ANIM_EFFECT_ONE_UP = 2950;

// Mushroom
constexpr auto ID_ANIM_MUSHROOM = 3000;
constexpr auto ID_ANIM_MUSHROOM_GREEN = 3010;

// Fire Leaf
constexpr auto ID_ANIM_FIRE_LEAF = 3100;

// Red Piranha
constexpr auto ID_ANIM_RED_PIRANHA_LOOK_DOWN = 900;
constexpr auto ID_ANIM_RED_PIRANHA_LOOK_UP = 910;
constexpr auto ID_ANIM_RED_PIRANHA_LOOK_DOWN_OPEN = 920;
constexpr auto ID_ANIM_RED_PIRANHA_LOOK_UP_OPEN = 930;

// Green Piranha
constexpr auto ID_ANIM_GREEN_PIRANHA_LOOK_DOWN = 901;
constexpr auto ID_ANIM_GREEN_PIRANHA_LOOK_UP = 911;
constexpr auto ID_ANIM_GREEN_PIRANHA_LOOK_DOWN_OPEN = 921;
constexpr auto ID_ANIM_GREEN_PIRANHA_LOOK_UP_OPEN = 931;

// Bite Piranha
constexpr auto ID_ANIM_BITE_PIRANHA = 700;

// Brick
constexpr auto ID_ANIM_BRICK = 2400;
constexpr auto ID_ANIM_BRICK_COIN = 2410;
constexpr auto ID_ANIM_BRICK_PARTICLE = 2420;

// Coin
constexpr auto ID_ANIM_COIN = 2800;

// Lucky Block
constexpr auto ID_ANIM_LUCKY_BLOCK = 2100;
constexpr auto ID_ANIM_LUCKY_BLOCK_CLAIMED = 2110;

// End Portal
constexpr auto ID_ANIM_END_PORTAL = 2201;

constexpr auto ID_ANIM_HIDDEN_BUTTON = 3200;
constexpr auto ID_ANIM_HIDDEN_BUTTON_STOMPED = 3210;

// Entities's Sprite IDs

constexpr auto ID_SPRITE_GROUND_TOP_LEFT = 1000;
constexpr auto ID_SPRITE_GROUND_TOP_MID = 1010;
constexpr auto ID_SPRITE_GROUND_TOP_RIGHT = 1020;
constexpr auto ID_SPRITE_GROUND_BOT_LEFT = 1030;
constexpr auto ID_SPRITE_GROUND_BOT_MID = 1040;
constexpr auto ID_SPRITE_GROUND_BOT_RIGHT = 1050;
constexpr auto ID_SPRITE_INDEPENDENT_PLATFORM = 1060;

// cactus
constexpr auto ID_SPRITE_BUSH_LITTLE = 1100;

constexpr auto ID_SPRITE_BUSH_LEFT_SIDE = 1110;
constexpr auto ID_SPRITE_BUSH_RIGHT_SIDE = 1120;
constexpr auto ID_SPRITE_BUSH_MID = 1130;

constexpr auto ID_SPRITE_BUSH_OUTER_TOP_LEFT = 1140;
constexpr auto ID_SPRITE_BUSH_OUTER_TOP_RIGHT = 1150;

constexpr auto ID_SPRITE_BUSH_INNER_TOP_LEFT = 1160;
constexpr auto ID_SPRITE_BUSH_INNER_TOP_RIGHT = 1170;

constexpr auto ID_SPRITE_END_BUSH_LEFT_SIDE = 1101;
constexpr auto ID_SPRITE_END_BUSH_RIGHT_SIDE = 1111;
constexpr auto ID_SPRITE_END_BUSH_TOP_LEFT = 1121;
constexpr auto ID_SPRITE_END_BUSH_TOP_RIGHT = 1131;

// Boomerang
constexpr auto ID_ANIM_BOOMERANG = 1400;

// Screw Block
/// white (color 0)
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_TOP_LEFT = 2500;  // (0,0)
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_TOP_MID = 2501;   // (0,1)
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_TOP_RIGHT = 2502; // (0,2)
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_MID_LEFT = 2503;  // (1,0)
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_MID_MID = 2504;   // (1,1)
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_MID_RIGHT = 2505; // (1,2)
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_BOT_LEFT = 2506;  // (2,0)
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_BOT_MID = 2507;   // (2,1)
constexpr auto ID_SPRITE_SCREW_BLOCK_WHITE_BOT_RIGHT = 2508; // (2,2)

/// pink (color 1)
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_TOP_LEFT = 2510;  // (0,0)
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_TOP_MID = 2511;   // (0,1)
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_TOP_RIGHT = 2512; // (0,2)
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_MID_LEFT = 2513;  // (1,0)
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_MID_MID = 2514;   // (1,1)
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_MID_RIGHT = 2515; // (1,2)
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_BOT_LEFT = 2516;  // (2,0)
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_BOT_MID = 2517;   // (2,1)
constexpr auto ID_SPRITE_SCREW_BLOCK_PINK_BOT_RIGHT = 2518; // (2,2)

/// blue (color 2)
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_TOP_LEFT = 2520;  // (0,0)
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_TOP_MID = 2521;   // (0,1)
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_TOP_RIGHT = 2522; // (0,2)
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_MID_LEFT = 2523;  // (1,0)
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_MID_MID = 2524;   // (1,1)
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_MID_RIGHT = 2525; // (1,2)
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_BOT_LEFT = 2526;  // (2,0)
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_BOT_MID = 2527;   // (2,1)
constexpr auto ID_SPRITE_SCREW_BLOCK_BLUE_BOT_RIGHT = 2528; // (2,2)

/// green (color 3)
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_TOP_LEFT = 2530;  // (0,0)
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_TOP_MID = 2531;   // (0,1)
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_TOP_RIGHT = 2532; // (0,2)
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_MID_LEFT = 2533;  // (1,0)
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_MID_MID = 2534;   // (1,1)
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_MID_RIGHT = 2535; // (1,2)
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_BOT_LEFT = 2536;  // (2,0)
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_BOT_MID = 2537;   // (2,1)
constexpr auto ID_SPRITE_SCREW_BLOCK_GREEN_BOT_RIGHT = 2538; // (2,2)

/// shade (keeping these unchanged since they don't follow the same pattern)
constexpr auto ID_SPRITE_SCREW_BLOCK_SHADE_LEFT = 2550;
constexpr auto ID_SPRITE_SCREW_BLOCK_SHADE_TOP = 2551;
constexpr auto ID_SPRITE_SCREW_BLOCK_SHADE_BOT_LEFT = 2552;
constexpr auto ID_SPRITE_SCREW_BLOCK_SHADE_TOP_LEFT = 2553;
constexpr auto ID_SPRITE_SCREW_BLOCK_SHADE_TOP_RIGHT = 2554;

// Clouds
constexpr auto ID_SPRITE_CLOUD_TOP_LEFT = 1500;
constexpr auto ID_SPRITE_CLOUD_TOP_MID = 1510;
constexpr auto ID_SPRITE_CLOUD_TOP_RIGHT = 1520;
constexpr auto ID_SPRITE_CLOUD_BOT_LEFT = 1530;
constexpr auto ID_SPRITE_CLOUD_BOT_MID = 1540;
constexpr auto ID_SPRITE_CLOUD_BOT_RIGHT = 1550;

constexpr auto ID_SPRITE_CLOUD_FLOWER = 1501;
constexpr auto ID_SPRITE_CLOUD_STAR = 1502;
constexpr auto ID_SPRITE_CLOUD_MUSHROOM = 1503;

constexpr auto ID_SPRITE_CLOUD_SMALL = 1560;
constexpr auto ID_SPRITE_CLOUD_BIG_LEFT = 1570;
constexpr auto ID_SPRITE_CLOUD_BIG_RIGHT = 1571;
/// Pipe
constexpr auto ID_SPRITE_PIPE_TOP_LEFT = 2700;
constexpr auto ID_SPRITE_PIPE_TOP_RIGHT = 2710;
constexpr auto ID_SPRITE_PIPE_BOT_LEFT = 2720;
constexpr auto ID_SPRITE_PIPE_BOT_RIGHT = 2730;
constexpr auto ID_SPRITE_BONUS_PIPE_TOP_LEFT = 2740;
constexpr auto ID_SPRITE_BONUS_PIPE_TOP_RIGHT = 2750;
constexpr auto ID_SPRITE_BONUS_PIPE_BOT_LEFT = 2760;
constexpr auto ID_SPRITE_BONUS_PIPE_BOT_RIGHT = 2770;

// Sky Platform
constexpr auto ID_SPRITE_SKY_PLATFORM = 1600;

// Black Background
constexpr auto ID_SPRITE_BLACK_BACKGROUND_SOLID = 1700;
constexpr auto ID_SPRITE_BLACK_BACKGROUND_BARRIER = 1710;

// end portal
constexpr auto ID_SPRITE_END_PORTAL_TOP_LEFT = 2200;
constexpr auto ID_SPRITE_END_PORTAL_TOP_MID = 2210;
constexpr auto ID_SPRITE_END_PORTAL_TOP_RIGHT = 2220;
constexpr auto ID_SPRITE_END_PORTAL_MID_LEFT = 2230;
constexpr auto ID_SPRITE_END_PORTAL_MID_MID = 2240;
constexpr auto ID_SPRITE_END_PORTAL_MID_RIGHT = 2250;
constexpr auto ID_SPRITE_END_PORTAL_BOT_LEFT = 2260;
constexpr auto ID_SPRITE_END_PORTAL_BOT_MID = 2270;
constexpr auto ID_SPRITE_END_PORTAL_BOT_RIGHT = 2280;

constexpr auto ID_SPRITE_HUD_BOARD = 2900;
constexpr auto ID_SPRITE_HUD_BOARD_BACKGROUND = 2901;
constexpr auto ID_SPRITE_HUD_PMETER_ARROW_EMPTY = 2902;
constexpr auto ID_SPRITE_HUD_PMETER_ARROW_FULL = 2903;
constexpr auto ID_SPRITE_HUD_PMETER_BADGE_EMPTY = 2904;
constexpr auto ID_SPRITE_HUD_PMETER_BADGE_FULL = 2905;
constexpr auto ID_SPRITE_HUD_M_BADGE = 2906;
constexpr auto ID_SPRITE_HUD_L_BADGE = 2907;
constexpr auto ID_SPRITE_HUD_PAUSE = 2908;
constexpr auto ID_SPRITE_HUD_FIREFLOWER = 2909;
constexpr auto ID_SPRITE_HUD_MUSHROOM = 2910;
constexpr auto ID_SPRITE_HUD_STAR = 2911;
constexpr auto ID_SPRITE_HUD_POWERUP_SLOT = 2912;

// floating platforms
constexpr auto ID_SPRITE_MOVING_PLATFORM_LEFT = 3200;
constexpr auto ID_SPRITE_MOVING_PLATFORM_MID = 3201;
constexpr auto ID_SPRITE_MOVING_PLATFORM_RIGHT = 3202;

// Grass
constexpr auto ID_SPRITE_GRASS_TOP_LEFT = 2300;
constexpr auto ID_SPRITE_GRASS_TOP_MID = 2310;
constexpr auto ID_SPRITE_GRASS_TOP_RIGHT = 2320;
constexpr auto ID_SPRITE_GRASS_BOT_LEFT = 2330;
constexpr auto ID_SPRITE_GRASS_BOT_MID = 2340;
constexpr auto ID_SPRITE_GRASS_BOT_RIGHT = 2350;

// Secret Wall
constexpr auto ID_SPRITE_SECRET_WALL = 3300;