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
	RectCollider(const IVec2 topLeftOffset, const IVec2 size) : topLeftPointOffset(topLeftOffset), size(size) {};

	IVec2 topLeftPointOffset;
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

inline Vec2 getColliderPosition(const Vec2 position, const RectCollider collider)
{
	return { position.x + collider.topLeftPointOffset.x, position.y + collider.topLeftPointOffset.y };
}

inline bool aabb(Vec2 positionA, Vec2 positionB, const RectCollider a, const RectCollider b)
{
	Vec2 aTopLeftWithOffset = getColliderPosition(positionA, a);
	Vec2 bTopLeftWithOffset = getColliderPosition(positionB, b);

	return aTopLeftWithOffset.x < bTopLeftWithOffset.x + b.size.x &&	// Collision on Left of a and right of b
		aTopLeftWithOffset.x + a.size.x > bTopLeftWithOffset.x &&		// Collision on Right of a and left of b
		aTopLeftWithOffset.y < bTopLeftWithOffset.y + b.size.y &&		// Collision on Top of a and Bottom of b
		aTopLeftWithOffset.y + a.size.y > bTopLeftWithOffset.y;			// Collision on Bottom of a and Top of b
}