#pragma once

#include "core/componentManager.h"
#include "core/lib.h"
#include <stdint.h>

struct TransformComponent : BaseComponent
{
	Vec2 position;
	Vec2 previousPosition;
};

struct SpriteComponent : BaseComponent
{
	void setupWithOffsetAndSize(IVec2 offset, IVec2 size)
	{
		this->offset = offset;
		this->size = size;
	}

	IVec2 offset;
	IVec2 size;
};

struct MovementComponent : BaseComponent
{
	float velocity = 0.f;
};