#pragma once

#include "entityManager.h"
#include "componentManager.h"
#include "systemManager.h"

class ECSLevel
{
public:
	inline void start()
	{
		_renderingSystem.createLightsBuffers();
	}

	inline void update()
	{
		_savePositionSystem.update();
		_characterMovementSystem.update();
	}
	
	inline void render(float renderAlpha)
	{
		_renderingSystem.render(renderAlpha);
		_debugCollidersSystem.render();
	}

	EntityManager _entityManager;
	ComponentManager _componentManager;

	SavePreviousPositionSystem _savePositionSystem;
	RenderingSystem _renderingSystem;
	CharacterMovementSystem _characterMovementSystem;
	DebugCollidersSystem _debugCollidersSystem;
};

class LevelManager
{
public:
	inline static ECSLevel* s_currentLevel = nullptr;
};

inline Entity& addEntity()
{
	return LevelManager::s_currentLevel->_entityManager.addEntity();
}

template<typename T>
bool entityHasComponent(Entity& entity)
{
	return LevelManager::s_currentLevel->_componentManager.entityHasComponent<T>(entity);
}

template<typename T>
T* addComponentToEntity(Entity& entity)
{
	return LevelManager::s_currentLevel->_componentManager.addComponentToEntity<T>(entity);
}

template<typename T>
void removeComponentFromEntity(Entity& entity)
{
	return LevelManager::s_currentLevel->_componentManager.removeComponentFromEntity<T>(entity);
}

template<typename T>
T* getComponentFromEntity(Entity& entity)
{
	return LevelManager::s_currentLevel->_componentManager.getComponentFromEntity<T>(entity);
}

inline std::array<Entity, k_maxNumberOfEntities>& getAllEntities()
{
	return LevelManager::s_currentLevel->_entityManager._entities;
}