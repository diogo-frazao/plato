#pragma once

#include "core/entityManager.h"
#include "core/componentManager.h"
#include "core/systemManager.h"

#include "renderingComponents.h"
#include "core/lib.h"
#include "core/input.h"

struct Camera
{
	// 0 = will never move, 1 = exactly at target pos
	float followTargetRatio = 0.f;
	int32_t minX = 0;
	int32_t maxX = 0;
	Vec2 targetPosition;
	Vec2 position = { 0,0 };
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
	MovementSystem _characterMovementSystem;
	DebugSystem _debugCollidersSystem;
	CrosshairSystem _crosshairSystem;
	AttackingSystem _attackingSystem;
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
	getComponentFromEntity<TransformComponent>(entity)->previousPosition = position;
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

inline Vec2 convertScreenToWorldPosition(Vec2 posInScreenSpace)
{
	if (posInScreenSpace.x > k_baseGameWidth || posInScreenSpace.y > k_baseGameHeight)
	{
		D_ASSERT(false, "convertScreenToWorldPosition(): The position passed is not in screen space");
	}

	Vec2 cameraPosition = LevelManager::getCurrentLevel()->_levelCamera.position;

	return { posInScreenSpace.x + cameraPosition.x,
			 posInScreenSpace.y + cameraPosition.y};
}