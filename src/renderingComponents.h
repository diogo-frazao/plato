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
};

struct MovementComponent
{
	int velocity = 0;
};