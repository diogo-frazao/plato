#pragma once

#include "constants.h"
#include <array>
#include <stdint.h>
#include "entityManager.h"

class ECSLevel;

class BaseSystem
{
public:
	virtual void start(ECSLevel* currentLevel) {};
	virtual void update(ECSLevel* currentLevel, float deltaTime) {};
	virtual void render(ECSLevel* currentLevel, float renderAlpha) {};
};

#pragma region Systems

// This needs to be the very first system to run, to ensure positions are saved for rendering interpolation
// For more information, read Gaffer Fix Your Timestep
class SavePreviousPositionSystem : public BaseSystem
{
	void update(ECSLevel* currentLevel, float deltaTime) override;
};

class DrawSpriteSystem : public BaseSystem
{
public:
	void render(ECSLevel* currentLevel, float renderAlpha) override;
};

class InputMovementSystem : public BaseSystem
{
public:
	void update(ECSLevel* currentLevel, float deltaTime) override;
};

#pragma endregion

class SystemManager
{
public:
	~SystemManager()
	{
		for (int32_t i = _systems.size() - 1; i >= 0; --i)
		{
			delete _systems[i];
			_systems[i] = nullptr;
		}
	}

	// For now, all levels have the same systems
	std::array<BaseSystem*, 3> _systems = 
	{ new SavePreviousPositionSystem(), new DrawSpriteSystem(), new InputMovementSystem() };
};