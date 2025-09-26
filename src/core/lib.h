#pragma once

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