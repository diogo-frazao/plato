#pragma once

#include <stdint.h>
#include <array>
#include "log.h"
#include "constants.h"
#include "lib.h"

struct Entity
{
#ifndef RELEASE_BUILD
	char* debugName = { 0 };
#endif 

	int32_t id = k_invalidId;
	int32_t componentBitmask = 0;
	EntityState entityState = NO_STATE;
};

static Entity s_invalidEntity;

class EntityManager
{
public:
	inline Entity& addEntity()
	{
		if (_lastValidEntityId >= k_maxNumberOfEntities - 1)
		{
			D_ASSERT(false, "addEntity(): Max number of entities reached. Increase k_maxNumberOfEntities");
			return s_invalidEntity;
		}

		int32_t indexForNewEntity = ++_lastValidEntityId;
		_entities[indexForNewEntity].id = indexForNewEntity;
		_entities[indexForNewEntity].componentBitmask = 0;

		return _entities[indexForNewEntity];
	}

	inline void clearEntityComponentBitmask(Entity& entity)
	{
		if (entity.id == k_invalidId)
		{
			D_LOG(ERROR, "clearEntityComponentBitmask(): Skipped since entity has invalid id");
			return;
		}

		_entities[entity.id].componentBitmask = 0;
	}

	std::array<Entity, k_maxNumberOfEntities> _entities;
	int32_t _lastValidEntityId = k_invalidId;
};