#pragma once

#include "lib.h"
#include "constants.h"
#include "SDL3/SDL_pixels.h"
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
	SDL_Texture* _loadedAtlasFiles[k_maxAtlasFiles]{ nullptr, nullptr };
	SDL_Texture* _backLightsBuffer = nullptr;
	SDL_Texture* _frontLightsBuffer = nullptr;
};

class CharacterMovementSystem
{
public:
	void update();
	void processHorizontalMovement(Entity* self);
	void processVerticalMovement(Entity* self);
	bool willCollideWithSolidAtPosition(Entity* self, const Vec2 positionToCheck);
};

class DebugCollidersSystem
{
public:
	void render();
	void debugRect(Vec2 position, RectCollider collider, SDL_Color color);
};