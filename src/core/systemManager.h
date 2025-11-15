#pragma once

#include "lib.h"
#include "SDL3/SDL_pixels.h"

class ECSLevel;
class MovementComponent;
class TransformComponent;
class Entity;

// This needs to be the very first system to run, to ensure positions are saved for rendering interpolation
// For more information, read Gaffer Fix Your Timestep
class SavePreviousPositionSystem
{
public:
	void update();
};

class DrawSpriteSystem
{
public:
	void render(float renderAlpha);
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