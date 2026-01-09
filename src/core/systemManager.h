#pragma once

#include "lib.h"
#include "constants.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include <stdint.h>

class ECSLevel;
class MovementComponent;
class TransformComponent;
class Entity;
class SDL_Texture;

// This needs to be the very first system to run, to ensure positions are saved for rendering interpolation
// For more information, read Gaffer Fix Your Timestep
class SavePreviousPositionSystem
{
public:
	void update();
};

class RenderingSystem
{
public:
	void createLightsBuffers();
	void render(float renderAlpha);
private:
	SDL_Texture* loadAtlas(AtlasType type);
	SDL_Texture* getTargetLightsBuffer(LayerType layer);

	void computeLightsAtLayer(LayerType layer);
	void renderSpritesAtLayer(LayerType layer, float renderAlpha);
	void renderLightsAtLayer(LayerType layer);

	SDL_FRect _src;
	SDL_FRect _dest;

	SDL_Texture* _loadedAtlasFiles[k_maxAtlasFiles]{ nullptr, nullptr };
	SDL_Texture* _backLightsBuffer = nullptr;
	SDL_Texture* _frontLightsBuffer = nullptr;
};

class AnimationSystem
{
public:
	void update();
};

class MovementSystem
{
public:
	void update();

	// General functions
	void processHorizontalMovement(Entity* self);
	void processVerticalMovement(Entity* self);
	bool willCollideWithLevelGeometryAtPosition(Entity* self, const Vec2 positionToCheck);

	// Main character specific
	void processMainCharacterMovement();
	void handleCoyoteTime(MovementComponent* movementComponent, bool wasGrounded);
};

class DebugSystem
{
public:
	void render();
	void debugRect(Vec2 position, RectCollider collider, SDL_Color color);
	void debugLine(Vec2 start, Vec2 end, SDL_Color color);
	void debugPoint(Vec2 position, SDL_Color color);
};

struct CrossHairSmear
{
	Entity* entity = nullptr;
	Vec2 directionToMove = { 0,0 };
	float moveSpeed = 4.f;
};

class CrosshairSystem
{
public:
	void update();
	void render();

	inline static float _resetSpritetimer = k_invalidTime;
	static void crosshairMeleeHitFeedback(Vec2 hitLocation);

	inline static CrossHairSmear _crosshairSmear;
};

class AttackingSystem
{
public:
	void update();
	
	// Player attacks
	void handleMainCharacterAttack();
	bool hasPlayerAlreadyAttackedEntity(int32_t entityId);
	void registerPlayerAttackToEntity(Entity* entity);
	void clearEntitiesPlayerAttacked();
	int32_t _entitiesPlayerAttackedForCurrentAttack[10];

	// Cleared at the beginning of each frame
	// This is the only place where we use RectCollider.topLeftPointOffset as a world position
	inline static RectCollider s_attackCollisionsToDebugThisFrame[10];

	void addColliderToDebugList(Vec2 position, RectCollider collider)
	{
#ifndef RELEASE_BUILD
		for (RectCollider& col : s_attackCollisionsToDebugThisFrame)
		{
			if (!col.isValidCollider())
			{
				Vec2 colliderStartingPosition = getColliderPosition(position, collider);
				col.topLeftPointOffset = { (int32_t)colliderStartingPosition.x, (int32_t)colliderStartingPosition.y };
				col.size = collider.size;
				return;
			}
		}

		D_LOG(ERROR, "addColliderToDebugList(): Colliders to debug is full, can't debug more");

#endif // !RELEASE_BUILD
	}

	void clearDebugCollisions()
	{
		for (RectCollider& col : s_attackCollisionsToDebugThisFrame)
		{
			col.invalidate();
		}
	}
};