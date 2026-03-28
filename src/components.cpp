#include "components.h"

#include "core/log.h"

void SpriteComponent::setSpriteData(SpriteType sprite)
{
	collidertopLeftPointOffset = { 0, 0 };

	switch (sprite)
	{
	case TODO_REMOVE_BG_SPRITE:
		atlasOffset = { 0, 0 };
		size = { 320, 180 };
		atlas = GAME_ATLAS;
		break;
	case TODO_REMOVE_FG_SPRITE:
		atlasOffset = { 0, 225 };
		size = { 320, 23 };
		atlas = GAME_ATLAS;
		break;
	case WHITE_ONE_BY_ONE_SPRITE:
		atlasOffset = { 323, 209 };
		size = { 1, 1 };
		atlas = GAME_ATLAS;
		break;
	case ROUND_LIGHT_SPRITE:
		atlasOffset = { 45, 0 };
		size = { 95, 86 };
		atlas = LIGHTS_ATLAS;
		break;
	case STREET_LAMP_LIGHT_SPRITE:
		atlasOffset = { 0, 0 };
		size = { 46, 86 };
		atlas = LIGHTS_ATLAS;
		break;
	case BIG_ROUND_LIGHT_SPRITE:
		atlasOffset = { 139, 0 };
		size = { 133, 129 };
		atlas = LIGHTS_ATLAS;
		break;
	case ROUND_LOW_QUALITY_LIGHT_SPRITE:
		atlasOffset = { 10, 90 };
		size = { 73, 55 };
		atlas = LIGHTS_ATLAS;
		break;
	case TODO_REMOVE_LEVEL_GEOMETRY_SPRITE:
		atlasOffset = { 0, 180 };
		size = { 320, 45 };
		atlas = GAME_ATLAS;
		break;
	case TODO_TEMOVE_INVISIBLE_SPRITE:
		atlasOffset = { 336, 0 };
		size = { 8, 8 };
		atlas = GAME_ATLAS;
		break;
	case TODO_REMOVE_RESTAURANT_INTERIOR:
		atlasOffset = { 323, 67 };
		size = { 586, 92 };
		atlas = GAME_ATLAS;
		break;
	case TODO_REMOVE_RESTAURANT_FLOOR_SPRITE:
		atlasOffset = { 323, 162 };
		size = { 565, 44 };
		atlas = GAME_ATLAS;
		break;
	case CHARACTER_IDLE_SPRITE:
		atlasOffset = { 1, 256 };
		size = { 480, 36 };
		atlas = GAME_ATLAS;
		numberOfFrames = 8;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case CHARACTER_TAKEOFF_SPRITE:
	case CHARACTER_RUN_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 1, 295 };
		size = { 360, 36 };
		numberOfFrames = 6;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case CHARACTER_JUMP_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 484, 256 };
		size = { 60, 36 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case CHARACTER_FALL_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 364, 295 };
		size = { 120, 36 };
		numberOfFrames = 2;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case CHARACTER_WEAPON_GOLF_IDLE_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 1, 369 };
		size = { 480, 36 };
		numberOfFrames = 8;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case CHARACTER_WEAPON_GOLF_TAKEOFF_SPRITE:
	case CHARACTER_WEAPON_GOLF_RUN_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 1, 408 };
		size = { 360, 36 };
		numberOfFrames = 6;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case CHARACTER_WEAPON_GOLF_SLOWDOWN_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 487, 408 };
		size = { 180, 36 };
		numberOfFrames = 3;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case CHARACTER_WEAPON_GOLF_JUMP_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 484, 369 };
		size = { 60, 36 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case CHARACTER_WEAPON_GOLF_FALL_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 364, 408 };
		size = { 120, 36 };
		numberOfFrames = 2;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case CHARACTER_WEAPON_GOLF_ATTACK_MIDDLE_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 1, 447 };
		size = { 420, 36 };
		numberOfFrames = 7;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case TURN_PARTICLE_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 0, 338 };
		size = { 132, 13 };
		numberOfFrames = 11;
		break;
	case TAKEOFF_PARTICLE_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 0, 352 };
		size = { 133, 12 };
		numberOfFrames = 7;
		break;
	case GANGSTER_SMALL_IDLE_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 1, 486 };
		size = { 60, 36 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 24, 10 };
		break;
	case GANGSTER_SMALL_HURT_TOP_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 307, 528 };
		size = { 120, 36 };
		numberOfFrames = 2;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case GANGSTER_SMALL_HURT_TOP_BOTTOM_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 553, 528 };
		size = { 120, 36 };
		numberOfFrames = 2;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case GANGSTER_SMALL_HURT_TOP_BOTTOM_RECOVER_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 676, 528 };
		size = { 60, 36 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case GANGSTER_SMALL_HURT_TOP_TOP_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 1, 567 };
		size = { 180, 36 };
		numberOfFrames = 3;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case GANGSTER_SMALL_HURT_TOP_TOP_RECOVER_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 184, 567 };
		size = { 60, 36 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case GANGSTER_SMALL_HURT_BOTTOM_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 64, 486 };
		size = { 60, 36 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case GANGSTER_SMALL_HURT_BOTTOM_RECOVER_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 127, 486 };
		size = { 120, 36 };
		numberOfFrames = 2;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case GANGSTER_SMALL_HURT_BOTTOM_BOTTOM_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 250, 486 };
		size = { 180, 36 };
		numberOfFrames = 3;
		collidertopLeftPointOffset = { 22, 10 };
		break;
	case GANGSTER_SMALL_HURT_BOTTOM_BOTTOM_RECOVER_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 433, 486 };
		size = { 60, 36 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 22, 10 };
		break;
	case GANGSTER_SMALL_HURT_BOTTOM_TOP_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 1, 528 };
		size = { 180, 36 };
		numberOfFrames = 3;
		collidertopLeftPointOffset = { 22, 10 };
		break;
	case GANGSTER_SMALL_HURT_BOTTOM_TOP_RECOVER_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 184, 528 };
		size = { 120, 36 };
		numberOfFrames = 2;
		collidertopLeftPointOffset = { 22, 10 };
		break;
	case GANGSTER_SMALL_CRAWL_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 496, 486 };
		size = { 120, 36 };
		numberOfFrames = 2;
		collidertopLeftPointOffset = { 22, 10 };
		break;
	case GANGSTER_SMALL_DEAD_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 619, 486 };
		size = { 60, 36 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 22, 10 };
		break;
	case CELLPHONE_IDLE_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 386, 217 };
		size = { 25, 28 };
		numberOfFrames = 1;
		break;
	case CROSSHAIR_MELEE_WEAPON_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 322, 33 };
		size = { 7, 9 };
		numberOfFrames = 1;
		break;
	case CROSSHAIR_MELEE_WEAPON_HIT_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 322, 45 };
		size = { 7, 9 };
		numberOfFrames = 1;
		break;
	case SMEAR_MELEE_ATTACK_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 322, 57 };
		size = { 320, 7 };
		numberOfFrames = 1;
		break;
	default:
		SpriteType unimplementedSprite = sprite;
		D_ASSERT(false, "Unkown sprite type");
		return;
	}

	this->sprite = sprite;
}
