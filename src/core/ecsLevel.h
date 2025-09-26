#pragma once

#include "entityManager.h"
#include "componentManager.h"
#include "systemManager.h"

class ECSLevel
{
public:
	inline void start()
	{
		for (auto& system : _systemManager._systems)
		{
			system->start(this);
		}
	}

	inline void update(float deltaTime)
	{
		for (auto& system : _systemManager._systems)
		{
			system->update(this, deltaTime);
		}
	}
	
	inline void render(float renderAlpha)
	{
		for (auto& system : _systemManager._systems)
		{
			system->render(this, renderAlpha);
		}
	}

	inline Entity& addEntity()
	{
		return _entityManager.addEntity();
	}

	template<typename T>
	bool entityHasComponent(Entity& entity)
	{
		return _componentManager.entityHasComponent<T>(entity);
	}

	template<typename T>
	T* addComponentToEntity(Entity& entity)
	{
		return _componentManager.addComponentToEntity<T>(entity);
	}

	template<typename T>
	void removeComponentFromEntity(Entity& entity)
	{
		return _componentManager.removeComponentFromEntity<T>(entity);
	}

	template<typename T>
	T* getComponentFromEntity(Entity& entity)
	{
		return _componentManager.getComponentFromEntity<T>(entity);
	}

	inline std::array<Entity, k_maxNumberOfEntities>& getAllEntities()
	{
		return _entityManager._entities;
	}

private:
	EntityManager _entityManager;
	ComponentManager _componentManager;
	SystemManager _systemManager;
};