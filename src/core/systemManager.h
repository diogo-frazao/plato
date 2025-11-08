#pragma once

#include "lib.h"
#include "SDL3/SDL_pixels.h"

class ECSLevel;

// This needs to be the very first system to run, to ensure positions are saved for rendering interpolation
// For more information, read Gaffer Fix Your Timestep
class SavePreviousPositionSystem
{
public:
	void update(ECSLevel* currentLevel, float deltaTime);
};

class DrawSpriteSystem
{
public:
	void render(ECSLevel* currentLevel, float renderAlpha);
};

class CharacterMovementSystem
{
public:
	void update(ECSLevel* currentLevel, float deltaTime);
};

class DebugCollidersSystem
{
public:
	struct DTO
	{
		RectCollider a;
		RectCollider b;
		SDL_Color color;
	};

	void render(ECSLevel* currentLevel, float renderAlpha);
	void debugCollisionBetweenRects(const DTO& dto);
	void debugRect(RectCollider a, SDL_Color color);
	DTO _collidersToDebug[10];
};