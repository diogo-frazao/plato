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
	void setupWithOffsetAndSize(IVec2 offset, IVec2 size)
	{
		this->offset = offset;
		this->size = size;
	}

	IVec2 offset;
	IVec2 size;
};

struct RectColliderComponent
{
	RectCollider collider;
	// This is what actors will check collision against when moving
	bool isSolid = false;
};

struct MovementComponent
{
	// Max horizontal speed while moving
	float maxHorizontalSpeed;
	// How fast the character reaches maxHorizontalSpeed while moving
	float runAcceleration;
	// How fast the character will come to stop when not moving
	float friction;

	Vec2 currentSpeed;
	// How much is left from one frame to the other. We only move when it's whole pixels
	Vec2 remainder;
};