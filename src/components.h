#pragma once

#include "core/componentManager.h"
#include "core/lib.h"
#include <stdint.h>
#include "SDL3/SDL_pixels.h"

struct TransformComponent
{
	// This may not correspond to where an entity is drawn. Ex: The player sprite has padding on all sides.
	Vec2 position;
	Vec2 previousPosition;
	Vec2 scale = { 1.f, 1.f };

	// If entity is created with a default pos addEntity(pos), startingPosition is assigned to that.
	Vec2 startingPosition;

	// Used every frame to reset the current scale back to {1,1}. Changed at runtime to have slow/fast scale effects
	float resetScaleLerp = 1.f;
	bool useDynamicScale = false;

	// Used for inspector debugging. Only needs to print the variables exposed to the inspector.
	void writeComponentDataToBuffer(const char* entityName, char* buffer, size_t bufferSize, size_t* currentWriteByte)
	{
		int bytesWritten = snprintf(buffer + *currentWriteByte, bufferSize - *currentWriteByte,
			"auto* t = getComponentFromEntity<TransformComponent>(%s);\nt->position = { %.2ff, %.2ff };\nt->scale = { %.2ff, %.2ff };\n",
			entityName, position.x, position.y, scale.x, scale.y);

		if (bytesWritten > 0)
		{
			*currentWriteByte += bytesWritten;
		}
	}
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

	void setLayer(LayerType layer)
	{
		this->layer = layer;
	}

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
			this->animationData.millisecondsToLoop = millisecondsToLoop;
			this->animationData.loopAnimation = loop;
		}

		// Allow changing animation speed at runtime
		this->animationData.millisecondsToChangeToNextFrame = millisecondsToChangeToNextFrame;
	}

	// Size of the current sprite
	IVec2 getCurrentSize()
	{
		if (numberOfFrames == 0)
		{
			return size;
		}

		return { (this->size.x / this->numberOfFrames), size.y };
	}

	// TODO: Improve if needed. Currently all animated sprites are expected to be on a single row

	// Starting offset on the atlas
	IVec2 atlasOffset;
	// The total size the sprite (or just one if non animated)
	IVec2 size;
	// Atlas the sprite(s) belong to
	AtlasType atlas = GAME_ATLAS;
	// Sprite type
	SpriteType sprite = INVALID_SPRITE;
	// The layer where this sprite is rendered on
	LayerType layer = BEHIND_LIGHTS_LAYER;
	// Color used to render this sprite
	SDL_Color color = { 255, 255, 255, 255 };
	// 0 for non animated sprites, X for the number of animations
	int32_t numberOfFrames = 0;

	// Container for the animation data of this sprite (speed, should loop, etc)
	AnimationData animationData;

	// To have more control per sprite basis, allow having a collider offset override.
	// This will override the RectColliderComponent collider.topLeftPointOffset of the entity
	IVec2 collidertopLeftPointOffset = { 0,0 };

	// Whether the sprite is horizontally flipped or not.
	bool flipX = false;

	// If true, ignores the camera position. (Ex: 0,0 will always be drawn at top left. Usually used for UI)
	bool drawnAtScreenSpace = false;

	// Rotation of the sprite in degrees
	float rotation = 0.f;

	// Point where the sprite rotates around
	RotationPivot rotationPivotType = DEFAULT_CENTER_ROTATION;

	// Used for inspector debugging. Only needs to print the variables exposed to the inspector.
	void writeComponentDataToBuffer(const char* entityName, char* buffer, size_t bufferSize, size_t* currentWriteByte)
	{
		int bytesWritten = snprintf(buffer + *currentWriteByte, bufferSize - *currentWriteByte, 
			"auto* s = getComponentFromEntity<SpriteComponent>(%s);\ns->color = {% i,% i,% i,% i};\ns->setupSpriteForLayer(%i, %i);\n",
			entityName, color.r, color.g, color.b, color.a, (int)sprite, (int)layer);

		if (bytesWritten > 0)
		{
			*currentWriteByte += bytesWritten;
		}
	}
};

struct RectColliderComponent
{
	RectCollider collider;
	// This is what actors will check collision against when moving
	bool isLevelGeometry = false;
};

struct MovementComponent
{
	SpriteType movementAnimations[k_maxNumberOfMovementAnimations];

	void setupMovementAnimations(SpriteType animations[k_maxNumberOfMovementAnimations])
	{
		for (uint8_t i = 0; i < k_maxNumberOfMovementAnimations; ++i)
		{
			movementAnimations[i] = animations[i];
		}
	}

	// Max horizontal speed while moving
	float maxHorizontalSpeed = 1.5f;
	// How fast the character reaches maxHorizontalSpeed while moving
	float runAcceleration = 6.f;
	// How fast the character will come to stop when not moving
	float friction = 10.f;

	// How fast the character will come to stop when not moving and in the air
	float airFriction = 2.f;

	// Max vertical speed while falling
	float maxVerticalSpeed = 3.7f;
	// How fast the character reaches maxVerticalSpeed while falling
	float gravity = 8.68f;
	// How high the character jumps
	float jumpSpeed = 3.3f;

	Vec2 currentSpeed;
	// How much is left from one frame to the other. We only move when it's whole pixels
	Vec2 remainder;

	// How many seconds we can still jump after leaving a platform
	float coyoteTime = 0.12f;

	bool isGrounded = false;

	// How many seconds have passed since we walked off a platform. Jumping doesn't count.
	float timeSinceLeftPlatform = k_invalidId;

	// Data needed for other components to to things
	bool isMovingOnFloor = false;
};

enum WeaponType
{
	NO_WEAPON_TYPE,
	GOLF_WEAPON_TYPE,
	ROSTOV_WEAPON_PISTOL_TYPE,
	WEAPON_TYPE_COUNT,
};

struct AttackingComponent
{
	// Deal Damage related

	enum WeaponType weaponInHand = NO_WEAPON_TYPE;

	// Updated as the entity gets damaged.
	enum AttackType lastDamageType = NO_ATTACK;

	// Receive Damage related

	// If true, instead of crawling, the entity will be on the same spot. Used for cutscenes when we will talk with someone as we are about to kill them
	bool shouldWaitToDie = false;

	// Whether or not this entity can be attacked/damaged
	bool canBeAttacked = true;

	// If damageCounter is equal or greater than this, the character will fall and go to HURT_STATE_TWO
	uint8_t numberOfHitsToFall = 2;

	// How many times this entity was attacked
	uint8_t damageCounter = 0;

	// After this time (in sec), character will play hurt one recover, and go back to idle at the end.
	float timeToRecoverFromHurtOneState = 2.f;

	// After this time (in sec), character will play hurt two recover.
	float timeToRecoverFromHurtTwoState = 1.f;

	// When hurt two recover starts playing, After this time (in sec), the character will start crawling.
	float timeToStartCrawling = 0.5f;

	float recoverTimer = k_invalidTime;
};