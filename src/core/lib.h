#pragma once

#include <stdint.h>

struct IVec2
{
	IVec2() = default;
	IVec2(int32_t x, int32_t y) : x(x), y(y) {};

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

struct RectCollider
{
	RectCollider() = default;
	RectCollider(const IVec2 topLeft, const IVec2 size) : topLeftPoint(topLeft), size(size) {};

	IVec2 topLeftPoint;
	IVec2 size;
};

inline uint64_t min(uint64_t value, uint64_t max)
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

inline bool aabb(const RectCollider a, const RectCollider b)
{
	return a.topLeftPoint.x < b.topLeftPoint.x + b.size.x &&	// Collision on Left of a and right of b
		a.topLeftPoint.x + a.size.x > b.topLeftPoint.x &&		// Collision on Right of a and left of b
		a.topLeftPoint.y < b.topLeftPoint.y + b.size.y &&		// Collision on Top of a and Bottom of b
		a.topLeftPoint.y + a.size.y > b.topLeftPoint.y;			// Collision on Bottom of a and Top of b
}