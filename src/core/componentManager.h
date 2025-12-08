#pragma once

#include "core/entityManager.h"
#include <stdint.h>
#include <array>
#include "constants.h"
#include <typeinfo>
#include <optional>

#define BITSHIFT(x) 1 << x

// Shared between all components
inline uint32_t getUniqueIndex()
{
	static uint32_t index = 0;
	return index++;
}

// Each component has its own unique Id
template<typename T>
uint32_t getUniqueIdForComponent()
{
	static uint32_t componentIndex = getUniqueIndex();
	return componentIndex;
}

template<typename T>
struct ComponentArray
{
	std::array<T, k_maxNumberOfEntities> _array;
	uint32_t _uniqueId;
};

// Called every time we want to access a component.
// If it's the first time, it will allocate an array for maxEntities and calculate its unique id
// It it's not the first time, it just returns a reference to the initially created.
template<typename T>
ComponentArray<T>& getUniqueComponentArrayForComponent()
{
	static ComponentArray<T> componentArray;
	uint32_t uniqueId = getUniqueIdForComponent<T>();
	componentArray._uniqueId = uniqueId;
	return componentArray;
}

//TODO: Think about the lifetime of this between levels. If everything is static and lazy loaded, it will leave as long as the app is running
// It's important to mention that maybe this is not that bad. We just allocate components as we need them, and through levels we just clear the data of the arrays, 
// without destroying/recreating them

class ComponentManager
{
public:

	template<typename T>
	bool entityHasComponent(Entity& entity)
	{
		if (entity.id == k_invalidId)
		{
			D_LOG(ERROR, "entityHasComponent(): Skipped %s since entity has invalid id", typeid(T).name());
			return false;
		}

		uint32_t componentId = getUniqueIdForComponent<T>();

		bool doesEntityHaveComponent = entity.componentBitmask & BITSHIFT(componentId);
		return doesEntityHaveComponent;
	}

	template<typename T>
	T* addComponentToEntity(Entity& entity)
	{
		if (entity.id == k_invalidId)
		{
			D_LOG(ERROR, "addComponentToEntity(): Skipped %s since entity has invalid id", typeid(T).name());
			return nullptr;
		}

		ComponentArray<T>& componentArray = getUniqueComponentArrayForComponent<T>();
		if (entityHasComponent<T>(entity))
		{
			D_LOG(WARNING, "addComponentToEntity(): Skipped since entity %i already has component %s", entity.id, typeid(T).name());
			return static_cast<T*>(&(componentArray._array[entity.id]));
		}

		entity.componentBitmask |= BITSHIFT(componentArray._uniqueId);
		return static_cast<T*>(&(componentArray._array[entity.id]));
	}

	template<typename T>
	void removeComponentFromEntity(Entity& entity)
	{
		if (entity.id == k_invalidId)
		{
			D_LOG(ERROR, "removeComponentFromEntity(): Skipped %s since entity has invalid id", typeid(T).name());
			return;
		}

		if (!entityHasComponent<T>(entity))
		{
			D_LOG(WARNING, "removeComponentFromEntity(): Skipped %s since entity %i doesn't have the component", typeid(T).name(), entity.id);
			return;
		}

		uint32_t componentUniqueId = getUniqueIdForComponent<T>();
		entity.componentBitmask &= ~(BITSHIFT(componentUniqueId));
	}

	// Tthis must be called only if the entity has the component! Otherwise will lead to UB
	template<typename T>
	T* getComponentFromEntity(Entity& entity)
	{
		if (entity.id == k_invalidId)
		{
			D_LOG(ERROR, "getComponentFromEntity(): Skipped %s since entity has invalid id", typeid(T).name());
			return nullptr;
		}

		ComponentArray<T>& componentArray = getUniqueComponentArrayForComponent<T>();
		return static_cast<T*>(&(componentArray._array[entity.id]));
	}
};