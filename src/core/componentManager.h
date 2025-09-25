#pragma once

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

// Each component has its own implementation
template<typename T>
uint32_t getArrayIndexForComponent()
{
	static uint32_t componentIndex = getUniqueIndex();
	return componentIndex;
}

struct BaseComponent
{
	virtual ~BaseComponent() = default;
};

using ComponentArray = std::array<BaseComponent*, k_maxNumberOfEntities>;

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

		uint32_t componentIndex = getArrayIndexForComponent<T>();

		bool doesEntityHaveComponent = entity.componentBitmask & BITSHIFT(componentIndex);
		return doesEntityHaveComponent;
	}

	//TODO: Pass args
	template<typename T>
	T* addComponentToEntity(Entity& entity)
	{
		if (entity.id == k_invalidId)
		{
			D_LOG(ERROR, "addComponentToEntity(): Skipped %s since entity has invalid id", typeid(T).name());
			return nullptr;
		}

		uint32_t componentIndex = getArrayIndexForComponent<T>();
		if (componentIndex >= k_maxNumberOfComponents)
		{
			D_ASSERT(false, "addComponentToEntity(): There are more components than allowed. Increase k_maxNumberOfComponents");
			return nullptr;
		}

		ComponentArray& component = _allComponents[componentIndex];

		if (entityHasComponent<T>(entity))
		{
			D_LOG(WARNING, "addComponentToEntity(): Skipped since entity %i already has component %s", entity.id, typeid(T).name());
			return static_cast<T*>(component[entity.id]);
		}

		entity.componentBitmask |= BITSHIFT(componentIndex);

		component[entity.id] = new T();
		return static_cast<T*>(component[entity.id]);
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

		uint32_t componentIndex = getArrayIndexForComponent<T>();
		ComponentArray& component = _allComponents[componentIndex];
		
		entity.componentBitmask &= ~(BITSHIFT(componentIndex));

		delete component[entity.id];
		component[entity.id] = nullptr;
	}

	// For performance reasons, this must be called only if the entity has the component!
	template<typename T>
	T* getComponentFromEntity(Entity& entity)
	{
		if (entity.id == k_invalidId)
		{
			D_LOG(ERROR, "getComponentFromEntity(): Skipped %s since entity has invalid id", typeid(T).name());
			return nullptr;
		}

		uint32_t componentIndex = getArrayIndexForComponent<T>();
		ComponentArray& component = _allComponents[componentIndex];
		return static_cast<T*>(component[entity.id]);
	}

	//TODO: When we create the level class, we need to update the bitmask for all entities when we do this
	inline void removeAllComponentsForAllEntities()
	{
		for (ComponentArray& componentArray : _allComponents)
		{
			for (int32_t i = componentArray.size() -1; i >= 0; --i)
			{
				if (componentArray[i])
				{
					delete componentArray[i];
					componentArray[i] = nullptr;
				}
			}
		}
	}

	~ComponentManager()
	{
		removeAllComponentsForAllEntities();
	}

private:
std::array<ComponentArray, k_maxNumberOfComponents> _allComponents;
};