#pragma once

#include "core/entityManager.h"
#include "core/componentManager.h"
#include "core/systemManager.h"

#include "renderingComponents.h"
#include "core/lib.h"

struct Camera
{
	// 0 = will never move, 1 = exactly at target pos
	float followTargetRatio = 0.f;
	int32_t minX = 0;
	int32_t maxX = 0;
	Vec2 targetPosition;
	Vec2 position;
};

class ECSLevel
{
public:
	void start();
	void update();
	void imguiRender();
	void render(float renderAlpha);

	Camera _levelCamera;

	EntityManager _entityManager;
	ComponentManager _componentManager;

	SavePreviousPositionSystem _savePositionSystem;
	RenderingSystem _renderingSystem;
	AnimationSystem _animationSystem;
	CharacterMovementSystem _characterMovementSystem;
	DebugSystem _debugCollidersSystem;
};

struct LevelManager
{
	inline static uint16_t s_currentLevelIndex = 0;
	inline static ECSLevel* _levels[k_maxLevels] = { new ECSLevel() };

	inline static ECSLevel* getCurrentLevel()
	{
		return _levels[s_currentLevelIndex];
	}
};


template<typename T>
bool entityHasComponent(Entity& entity)
{
	return LevelManager::getCurrentLevel()->_componentManager.entityHasComponent<T>(entity);
}

template<typename T>
T* addComponentToEntity(Entity& entity)
{
	return LevelManager::getCurrentLevel()->_componentManager.addComponentToEntity<T>(entity);
}

template<typename T>
void removeComponentFromEntity(Entity& entity)
{
	return LevelManager::getCurrentLevel()->_componentManager.removeComponentFromEntity<T>(entity);
}

template<typename T>
T* getComponentFromEntity(Entity& entity)
{
	return LevelManager::getCurrentLevel()->_componentManager.getComponentFromEntity<T>(entity);
}

inline Entity& addEntity(Vec2 position = Vec2())
{
	Entity& entity = LevelManager::getCurrentLevel()->_entityManager.addEntity();
	addComponentToEntity<TransformComponent>(entity)->position = position;
	return entity;
}

inline std::array<Entity, k_maxNumberOfEntities>& getAllEntities()
{
	return LevelManager::getCurrentLevel()->_entityManager._entities;
}

inline Entity& getEntityById(uint32_t id)
{
	return LevelManager::getCurrentLevel()->_entityManager._entities[id];
}