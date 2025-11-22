#pragma once

#include "core/componentManager.h"
#include "core/lib.h"
#include <stdint.h>

struct TransformComponent
{
	Vec2 position;
	Vec2 previousPosition;
};

struct SpriteComponent
{
	void setup(IVec2 offset, IVec2 size, LayerType layer, AtlasType atlas = GAME)
	{
		this->offset = offset;
		this->size = size;
		this->atlas = atlas;
		this->layer = layer;
	}

	IVec2 offset;
	IVec2 size;
	AtlasType atlas = GAME;
	LayerType layer = BEHIND_CHAR;
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