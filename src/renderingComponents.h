#pragma once

#include "core/componentManager.h"
#include "core/lib.h"
#include <stdint.h>

struct TransformComponent
{
	Vec2 position;
	Vec2 previousPosition;
	Vec2 scale = { 1.f, 1.f };
};

struct SpriteComponent
{
	void setSpriteData(SpriteType sprite)
	{
		switch (sprite)
		{
		case TODO_REMOVE_BG_SPRITE:
			offset = { 0, 0 };
			size = { 320, 180 };
			atlas = GAME_ATLAS;
			break;
		case CHARACTER_SPRITE:
			offset = { 320, 0 };
			size = { 14, 19 };
			atlas = GAME_ATLAS;
			break;
		case TODO_REMOVE_FG_SPRITE:
			offset = { 0, 225 };
			size = { 320, 23 };
			atlas = GAME_ATLAS;
			break;
		case ROUND_LIGHT_SPRITE:
			offset = { 45, 0 };
			size = { 95, 86 };
			atlas = LIGHTS_ATLAS;
			break;
		case STREET_LAMP_LIGHT_SPRITE:
			offset = { 0, 0 };
			size = { 46, 86 };
			atlas = LIGHTS_ATLAS;
			break;
		case TODO_REMOVE_LEVEL_GEOMETRY_SPRITE:
			offset = { 0, 180 };
			size = { 320, 45 };
			atlas = GAME_ATLAS;
			break;
		case TODO_TEMOVE_INVISIBLE_SPRITE:
			offset = { 336, 0 };
			size = { 8, 8 };
			atlas = GAME_ATLAS;
			break;
		default:
			D_ASSERT(false, "Unkown sprite type");
			return;
		}
	}

	void setupTypeForLayer(SpriteType sprite, LayerType layer)
	{
		setSpriteData(sprite);
		this->layer = layer;
	}

	IVec2 offset;
	IVec2 size;
	AtlasType atlas = GAME_ATLAS;
	SpriteType sprite = CHARACTER_SPRITE;
	LayerType layer = BEHIND_CHAR_LAYER;
	SDL_Color color = { 255, 255, 255, 255 };
};

struct RectColliderComponent
{
	RectCollider collider;
	// This is what actors will check collision against when moving
	bool isLevelGeometry = false;
};

struct MovementComponent
{
	// Max horizontal speed while moving
	float maxHorizontalSpeed;
	// How fast the character reaches maxHorizontalSpeed while moving
	float runAcceleration;
	// How fast the character will come to stop when not moving
	float friction;

	// Max vertical speed while falling
	float maxVerticalSpeed;
	// How fast the character reaches maxVerticalSpeed while falling
	float gravity;
	// How high the character jumps
	float jumpSpeed;

	Vec2 currentSpeed;
	// How much is left from one frame to the other. We only move when it's whole pixels
	Vec2 remainder;

	bool isGrounded = false;
};