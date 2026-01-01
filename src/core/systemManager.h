#pragma once

#include "lib.h"
#include "constants.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include <stdint.h>

class ECSLevel;
class MainCharacterMovementComponent;
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

class CharacterMovementSystem
{
public:
	void update();
	void processHorizontalMovement(Entity* self);
	void processVerticalMovement(Entity* self);
	bool willCollideWithLevelGeometryAtPosition(Entity* self, const Vec2 positionToCheck);

	void handleCoyoteTime(MainCharacterMovementComponent* movementComponent, bool wasGrounded);
};

class DebugSystem
{
public:
	void render();
	void debugRect(Vec2 position, RectCollider collider, SDL_Color color);
	void debugLine(Vec2 start, Vec2 end, SDL_Color color);
	void debugPoint(Vec2 position, SDL_Color color);
};