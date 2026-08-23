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
	case LONG_THIN_LIGHT_SPRITE:
		atlasOffset = { 124, 145 };
		size = { 66, 9 };
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
	case RESTAURANT_BAKED_HIGHLIGHTS_SPRITE:
		atlasOffset = { 611, 214 };
		size = { 521, 78 };
		atlas = GAME_ATLAS;
		break;
	case GOLF_WEAPON_SPRITE:
		atlasOffset = { 355, 28 };
		size = { 8, 11 };
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
	case CHARACTER_WEAPON_PISTOL_IDLE_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 586, 325 };
		size = { 60, 36 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case CHARACTER_WEAPON_PISTOL_TAKEOFF_SPRITE:
	case CHARACTER_WEAPON_PISTOL_RUN_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 649, 325 };
		size = { 360, 36 };
		numberOfFrames = 6;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case CHARACTER_WEAPON_PISTOL_FIRE_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 649, 364 };
		size = { 300, 36 };
		numberOfFrames = 5;
		collidertopLeftPointOffset = { 25, 10 };
		break;
	case CHARACTER_WEAPON_PISTOL_FX_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 952, 364 };
		size = { 300, 36 };
		numberOfFrames = 5;
		break;
	case PISTOL_BULLET_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 470, 40 };
		size = { 27, 1 };
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
	case OSKAR_IDLE_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 1, 486 };
		size = { 60, 36 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 24, 10 };
		break;
	case OSKAR_RUN_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 487, 486 };
		size = { 360, 36 };
		numberOfFrames = 6;
		collidertopLeftPointOffset = { 24, 10 };
		break;
	case OSKAR_ATTACK_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 64, 486 };
		size = { 420, 36 };
		numberOfFrames = 7;
		collidertopLeftPointOffset = { 24, 10 };
		break;
	case OSKAR_SHOT_FALL_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 247, 567 };
		size = { 300, 36 };
		numberOfFrames = 5;
		collidertopLeftPointOffset = { 22, 10 };
		break;
	case OSKAR_SHOT_RECOVER_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 793, 567 };
		size = { 240, 36 };
		numberOfFrames = 4;
		collidertopLeftPointOffset = { 22, 10 };
		break;
	case OSKAR_FALL_DEATH_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 550, 567 };
		size = { 240, 36 };
		numberOfFrames = 4;
		collidertopLeftPointOffset = { 22, 10 };
		break;
	case DARWIN_PLACEHOLDER_SPRITE:
		atlas = GAME_ATLAS;
		atlasOffset = { 339, 2 };
		size = { 13, 18 };
		numberOfFrames = 1;
		collidertopLeftPointOffset = { 0, 0 };
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
