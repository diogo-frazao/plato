#pragma once

#include "core/componentManager.h"
#include "core/lib.h"
#include <stdint.h>
#include "SDL3/SDL_pixels.h"

struct TransformComponent
{
	Vec2 position;
	Vec2 previousPosition;
	Vec2 scale = { 1.f, 1.f };
};

struct AnimationData
{
	uint32_t millisecondsToChangeToNextFrame = 70;
	uint32_t millisecondsToLoop = 70;
	bool loopAnimation = false;

#pragma region Changed by Animation System at runtime

	// If the current animation playing has finished
	bool finishedPlayingAnimation = false;
	// For animated sprites, this changes based on the current frame
	int32_t currentFrame = 0;
	// Time in milliseconds since the lat frame started. Used to change between animation frames
	float millisecondsSinceLastFrame = 0.f;

#pragma endregion
};

struct SpriteComponent
{
	void setSpriteData(SpriteType sprite);
	void setupSpriteForLayer(SpriteType sprite, LayerType layer)
	{
		setSpriteData(sprite);
		this->layer = layer;
	}

	void setupAnimationForLayer(SpriteType sprite, LayerType layer, bool loop, uint32_t millisecondsToChangeToNextFrame, uint32_t millisecondsToLoop)
	{
		setAnimationToPlayIfNotPlaying(sprite, loop, millisecondsToChangeToNextFrame, millisecondsToLoop);
		this->layer = layer;
	}

	void setAnimationToPlayIfNotPlaying(SpriteType targetAnimation, bool loop, uint32_t millisecondsToChangeToNextFrame, uint32_t millisecondsToLoop)
	{
		if (this->sprite != targetAnimation)
		{
			this->setSpriteData(targetAnimation);

			this->animationData.millisecondsSinceLastFrame = 0;
			this->animationData.currentFrame = 0;
			this->animationData.finishedPlayingAnimation = false;
			this->animationData.millisecondsToChangeToNextFrame = millisecondsToChangeToNextFrame;
			this->animationData.millisecondsToLoop = millisecondsToLoop;
			this->animationData.loopAnimation = loop;
		}
	}

	// TODO: Improve if needed. Currently all animated sprites are expected to be on a single row

	// Starting offset on the atlas
	IVec2 atlasOffset;
	// The size of each sprite (or just one if non animated)
	IVec2 size;
	// Atlas the sprite(s) belong to
	AtlasType atlas = GAME_ATLAS;
	// Sprite type
	SpriteType sprite = INVALID;
	// The layer where this sprite is rendered on
	LayerType layer = BEHIND_CHAR_LAYER;
	// Color used to render this sprite
	SDL_Color color = { 255, 255, 255, 255 };
	// 0 for non animated sprites, X for the number of animations
	int32_t numberOfFrames = 0;

	// Container for the animation data of this sprite (speed, should loop, etc)
	AnimationData animationData;

	bool flipX = false;
};

struct RectColliderComponent
{
	RectCollider collider;
	// This is what actors will check collision against when moving
	bool isLevelGeometry = false;
};

enum MovementState
{
	IDLE_STATE,
	TAKE_OFF_STATE,
	RUNNING_STATE,
	SLOWDOWN_STATE,
	JUMPING_STATE,
	FALLING_STATE
};

struct MovementComponent
{
	// Max horizontal speed while moving
	float maxHorizontalSpeed;
	// How fast the character reaches maxHorizontalSpeed while moving
	float runAcceleration;
	// How fast the character will come to stop when not moving
	float friction;

	// How fast the character will come to stop when not moving and in the air
	float airFriction;

	// Max vertical speed while falling
	float maxVerticalSpeed;
	// How fast the character reaches maxVerticalSpeed while falling
	float gravity;
	// How high the character jumps
	float jumpSpeed;

	Vec2 currentSpeed;
	// How much is left from one frame to the other. We only move when it's whole pixels
	Vec2 remainder;

	// How many seconds we can still jump after leaving a platform
	float coyoteTime;

	bool isGrounded = false;

	// How many seconds have passed since we walked off a platform. Jumping doesn't count.
	float timeSinceLeftPlatform = k_invalidId;

	MovementState movementState;

	const char* getMovementStateAsString()
	{
		switch (movementState)
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
		}
	}
};