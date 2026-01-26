#include "renderingComponents.h"

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
	case ROUND_SOFT_LIGHT_SPRITE:
		atlasOffset = { 139, 0 };
		size = { 133, 129 };
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
		atlasOffset = { 499, 256 };
		size = { 60, 36 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case CHARACTER_FALL_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 376, 295 };
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
		atlasOffset = { 517, 408 };
		size = { 180, 36 };
		numberOfFrames = 3;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case CHARACTER_WEAPON_GOLF_JUMP_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 502, 369 };
		size = { 60, 36 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case CHARACTER_WEAPON_GOLF_FALL_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 379, 408 };
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
	case GANGSTER_SMALL_HURT_ONE_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 79, 486 };
		size = { 60, 36 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case GANGSTER_SMALL_HURT_ONE_RECOVER_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 162, 486 };
		size = { 120, 36 };
		numberOfFrames = 2;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case GANGSTER_SMALL_HURT_TWO_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 314, 486 };
		size = { 180, 36 };
		numberOfFrames = 3;
		collidertopLeftPointOffset = { 22, 10 };
		break;
	case GANGSTER_SMALL_HURT_TWO_RECOVER_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 517, 486 };
		size = { 60, 36 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 22, 10 };
		break;
	case GANGSTER_SMALL_CRAWL_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 580, 486 };
		size = { 120, 36 };
		numberOfFrames = 2;
		collidertopLeftPointOffset = { 22, 10 };
		break;
	case GANGSTER_SMALL_DEAD_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 703, 486 };
		size = { 60, 36 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 22, 10 };
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
		D_ASSERT(false, "Unkown sprite type");
		return;
	}

	this->sprite = sprite;
}
