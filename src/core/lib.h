#pragma once

#include <stdint.h>
#include <cmath>
#include "log.h"

enum EntityState
{
	NO_STATE,

	// Locomotion
	IDLE_STATE,
	TAKE_OFF_STATE,
	RUNNING_STATE,
	SLOWDOWN_STATE,
	JUMPING_STATE,
	FALLING_STATE,

	// Attack
	ATTACKING_STATE,
	HURT_STATE
};

inline const char* getEntityStateAsString(EntityState state)
{
	switch (state)
	{
	case IDLE_STATE:
		return "Idle";
	case TAKE_OFF_STATE:
		return "Takeoff";
	case RUNNING_STATE:
		return "Running";
	case SLOWDOWN_STATE:
		return "Slowdown";
	case JUMPING_STATE:
		return "Jumping";
	case FALLING_STATE:
		return "Falling";
	case ATTACKING_STATE:
		return "Attacking";
	case HURT_STATE:
		return "Hurt";
	}

	return "INVALID";
}

enum SpriteType
{
	INVALID,
	TODO_REMOVE_BG_SPRITE,
	TODO_REMOVE_FG_SPRITE,
	ROUND_LIGHT_SPRITE,
	STREET_LAMP_LIGHT_SPRITE,
	ROUND_SOFT_LIGHT_SPRITE,
	TODO_REMOVE_LEVEL_GEOMETRY_SPRITE,
	TODO_TEMOVE_INVISIBLE_SPRITE,

	// Main Character
	CHARACTER_IDLE_SPRITE,
	CHARACTER_TAKEOFF_SPRITE,
	CHARACTER_RUN_SPRITE,
	CHARACTER_JUMP_SPRITE,
	CHARACTER_FALL_SPRITE,

	// Main Character Golf weapon
	CHARACTER_WEAPON_GOLF_IDLE_SPRITE,
	CHARACTER_WEAPON_GOLF_TAKEOFF_SPRITE,
	CHARACTER_WEAPON_GOLF_RUN_SPRITE,
	CHARACTER_WEAPON_GOLF_SLOWDOWN_SPRITE,
	CHARACTER_WEAPON_GOLF_JUMP_SPRITE,
	CHARACTER_WEAPON_GOLF_FALL_SPRITE,
	CHARACTER_WEAPON_GOLF_ATTACK_MIDDLE_SPRITE,

	// Dummy Enemy
	DUMMY_ENEMY_IDLE_SPRITE,
	DUMMY_ENEMY_HURT_SPRITE,

	// Crosshairs
	CROSSHAIR_MELEE_WEAPON_SPRITE,
	CROSSHAIR_MELEE_WEAPON_HIT_SPRITE,

	//FX
	SMEAR_MELEE_ATTACK_SPRITE,
	TURN_PARTICLE_SPRITE,
	TAKEOFF_PARTICLE_SPRITE
};

enum RotationPivot
{
	DEFAULT_CENTER_ROTATION,
	TOP_LEFT_ROTATION
};

enum LayerType
{
	BEHIND_CHAR_LAYER,
	BACK_LIGHTS_LAYER,
	CHARACTER_LAYER,
	IN_FRONT_CHAR_LAYER,
	FRONT_LIGHTS_LAYER,
	LEVEL_GEOMETRY_LAYER,
	CROSSHAIR_LAYER,
};

enum AtlasType
{
	GAME_ATLAS,
	LIGHTS_ATLAS
};

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

	bool isValidCollider() { return size.x > 0.f && size.y > 0.f; }
	void invalidate() { topLeftPointOffset.x = 0; topLeftPointOffset.y = 0; size.x = 0; size.y = 0; }

	IVec2 topLeftPointOffset{ 0,0 };
	IVec2 size{ 0,0 };
};

// In degrees
inline float getAngleBetweenTwoPoints(Vec2 a, Vec2 b)
{
	static constexpr float PI = 3.1415f;

	float deltaY = b.y - a.y;
	float deltaX = b.x - a.x;

	float angleInRadians = atan2(deltaY, deltaX);
	return angleInRadians * 180 / PI;
}

inline bool isTimerOngoing(float timer)
{
	return timer >= 0.f;
}

inline void startTimer(float& timer)
{
	timer = 0.f;
}

inline void invalidateTimer(float& timer)
{
	timer = -1;
}

inline int32_t clamp(int32_t value, int32_t min, int32_t max)
{
	if (value < min)
	{
		return min;
	}

	if (value > max)
	{
		return max;
	}

	return value;
}

inline int32_t min(int32_t a, int32_t b)
{
	if (a > b)
	{
		return b;
	}

	return a;
}

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

inline float max(float a, float b)
{
	if (a > b)
	{
		return a;
	}

	return b;
}

inline float min(float a, float b)
{
	if (a < b)
	{
		return a;
	}

	return b;
}

inline float approach(float current, float target, float increase)
{
	if (current < target)
	{
		return min(current + increase, target);
	}

	return max(current - increase, target);
}

inline int8_t sign(int32_t amount)
{
	return (amount >= 0) ? 1 : -1;
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