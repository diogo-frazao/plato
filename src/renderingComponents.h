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
			atlasOffset = { 0, 0 };
			size = { 320, 180 };
			atlas = GAME_ATLAS;
			break;
		case CHARACTER_SPRITE:
			atlasOffset = { 320, 0 };
			size = { 14, 19 };
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
		case CHARACTER_IDLE:
			atlasOffset = { 0, 255 };
			size = { 120, 21 };
			atlas = GAME_ATLAS;
			numberOfFrames = 8;
			break;
		case CHARACTER_RUN:
			atlas = GAME_ATLAS;
			atlasOffset = { 0, 276 };
			size = { 60, 21 };
			numberOfFrames = 4;
			break;
		case CHARACTER_RUN_2:
			atlas = GAME_ATLAS;
			atlasOffset = { 0, 297 };
			size = { 90, 21 };
			numberOfFrames = 6;
			break;
		default:
			D_ASSERT(false, "Unkown sprite type");
			return;
		}

		this->sprite = sprite;
	}

	void setupTypeForLayer(SpriteType sprite, LayerType layer)
	{
		setSpriteData(sprite);
		this->layer = layer;
	}

	// TODO: Improve if needed. Currently all animated sprites are expected to be on a single row

	// Starting offset on the atlas
	IVec2 atlasOffset;
	// The size of each sprite (or just one if non animated)
	IVec2 size;
	// Atlas the sprite(s) belong to
	AtlasType atlas = GAME_ATLAS;
	// Sprite type
	SpriteType sprite = CHARACTER_SPRITE;
	// The layer where this sprite is rendered on
	LayerType layer = BEHIND_CHAR_LAYER;
	// Color used to render this sprite
	SDL_Color color = { 255, 255, 255, 255 };
	// 0 for non animated sprites, X for the number of animations
	int32_t numberOfFrames = 0;
	// For animated sprites, this changes based on the current frame
	int32_t currentFrame = 0;
	// Time in seconds since the lat frame started. Used to change between animation frames
	float millisecondsSinceLastFrame = 0.f;

	bool flipX = false;
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

	// How fast the character will come to stop when not moving and in the air
	float airFriction;

	// Max vertical speed while falling
	float maxVerticalSpeed;
	// How fast the character reaches maxVerticalSpeed while falling
	float gravity;
	// How high the character jumps
	float jumpSpeed;

	Vec2 currentSpeed;
	// How much is left from one frame to the other. We only move when it's whole pixels
	Vec2 remainder;

	// How many seconds we can still jump after leaving a platform
	float coyoteTime;

	bool isGrounded = false;

	// How many seconds have passed since we walked off a platform. Jumping doesn't count.
	float timeSinceLeftPlatform = k_invalidId;
};