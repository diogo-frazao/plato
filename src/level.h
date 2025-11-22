#pragma once

#include "core/entityManager.h"
#include "core/componentManager.h"
#include "core/systemManager.h"

class ECSLevel
{
public:
	void start();
	void update();
	void imguiRender();
	void render(float renderAlpha);

	EntityManager _entityManager;
	ComponentManager _componentManager;

	SavePreviousPositionSystem _savePositionSystem;
	RenderingSystem _renderingSystem;
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


inline Entity& addEntity()
{
	return LevelManager::getCurrentLevel()->_entityManager.addEntity();
}

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

inline std::array<Entity, k_maxNumberOfEntities>& getAllEntities()
{
	return LevelManager::getCurrentLevel()->_entityManager._entities;
}

inline Entity& getEntityById(uint32_t id)
{
	return LevelManager::getCurrentLevel()->_entityManager._entities[id];
}