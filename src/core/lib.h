#pragma once

#include <stdint.h>

struct IVec2
{
	int32_t x;
	int32_t y;
};

struct Vec2
{
	Vec2() = default;
	Vec2(float x, float y) : x(x), y(y) {}

	float x;
	float y;
};

inline uint64_t max(uint64_t value, uint64_t max)
{
	if (value > max)
	{
		return max;
	}

	return value;
}

inline float lerp(float a, float b, float time)
{
	return a + (b - a) * time;
}

inline Vec2 lerp(Vec2 a, Vec2 b, float time)
{
	return Vec2(lerp(a.x, b.x, time), lerp(a.y, b.y, time));
}