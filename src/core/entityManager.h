#pragma once

#include <stdint.h>
#include <array>
#include "log.h"
#include "constants.h"

struct Entity
{
	int32_t id = k_invalidId;
	int32_t componentBitmask = k_invalidId;
};

class EntityManager
{
public:
	inline int32_t addEntity()
	{
		if (_lastValidEntityId >= k_maxNumberOfEntities - 1)
		{
			D_LOG(ERROR, "Max number of entities reached");
			return k_invalidId;
		}

		int32_t indexForNewEntity = ++_lastValidEntityId;
		_entities[indexForNewEntity].id = indexForNewEntity;

		return indexForNewEntity;
	}

private:
	int32_t _lastValidEntityId = k_invalidId;
	std::array<Entity, k_maxNumberOfEntities> _entities;
};