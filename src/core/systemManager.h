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
	virtual void update(ECSLevel* currentLevel) {};
	virtual void render(ECSLevel* currentLevel) {};
};

#pragma region Systems

class DrawSpriteSystem : public BaseSystem
{
public:
	void render(ECSLevel* currentLevel) override;
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
	std::array<BaseSystem*, 1> _systems = 
	{ new DrawSpriteSystem()};
};