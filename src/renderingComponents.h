#pragma once

#include "core/componentManager.h"
#include <stdint.h>

struct IVec2
{
	int32_t x;
	int32_t y;
};

struct SpriteComponent : BaseComponent
{
	void setupWithOffsetAndSize(IVec2 offset, IVec2 size)
	{
		this->offset = offset;
		this->size = size;
	}

	const char* spriteName = k_atlasFilePath;
	IVec2 offset;
	IVec2 size;
};