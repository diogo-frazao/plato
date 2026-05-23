#pragma once

#include "core/entityManager.h"
#include "core/componentManager.h"
#include "core/systems.h"

#include "components.h"
#include "core/lib.h"
#include "core/input.h"

#include <SDL3/SDL_rect.h>

struct Camera
{
	// 0 = will never move, 1 = exactly at target posInScreenSpace
	float followTargetRatio = 0.f;
	int32_t minX = 0;
	int32_t maxX = 0;
	Vec2 targetPosition;
	// Camera is centered on X and Y instead of top left so the zoom zooms from all sides.
	// This means that the initial posInScreenSpace is (180,90), otherwhise something at (0,0) would be on the center of the screen.
	Vec2 position = { k_baseGameWidth / 2, k_baseGameHeight / 2 };
	float zoom = 1.f;

	bool canFollowTarget = true;

	// Point where the zoom will be focused on. By default it's the center of the screen.
	Vec2 zoomFocusPoint = { k_baseGameWidth / 2, k_baseGameHeight / 2 };
};

enum LevelStages
{
    MARKETING_PHONE_STAGE,
    FIRST_DAD_PHONE_STAGE,
    DARWIN_CONVERSATION_STAGE,
    GANGSTER_CONFRONTATION_STAGE,
    LEVEL_STAGES_COUNT,
};
static const char* s_levelStagesString = { "MARKETING_PHONE_STAGE\0FIRST_DAD_PHONE_STAGE\0DARWIN_CONVERSATION_STAGE\0GANGSTER_CONFRONTATION_SAGE" };

struct DarwinConfrontationStageData
{
    bool canMoveFromDoor = false;
	bool canGetNearTable = false;
	bool canGetEvenNearTable = false;
	float waitToAskIfPaHeardUs = k_invalidTime;
	float waitAfterCallEndsTimer = k_invalidTime;
	bool canMoveBack = false;

	void reset()
	{
		canMoveFromDoor = false;
		canGetNearTable = false;
		canGetEvenNearTable = false;
		waitToAskIfPaHeardUs = k_invalidTime;
		waitAfterCallEndsTimer = k_invalidTime;
		canMoveBack = false;
	}
};

struct GangsterConfrontationStageData
{
	bool canDarwinMoveToKitchen = true;
	bool hasStartedConfrontationDialogue = false;
	bool hasToldRostovToNotGetInvolved = false;
	bool rostovHasGrabbedCue = false;
	bool canOskarMoveClose = false;
	bool hasRostovAttackedEnemy = false;

	bool hasHugoHelpedBrother = false;
	bool canHugoReachBrother = false;
	bool canHugoReachRostov = false;
	float waitToCheckIfOskarIsDead = k_invalidTime;
	bool hasRostovAttackedHugo = false;
	bool hasDarwinAskedToNotKillHugo = false;
	bool canDarwinComeClose = false;

	void reset()
	{
		canDarwinMoveToKitchen = true;
		hasStartedConfrontationDialogue = false;
		hasToldRostovToNotGetInvolved = false;
		canOskarMoveClose = false;
		rostovHasGrabbedCue = false;
		hasRostovAttackedEnemy = false;
		hasHugoHelpedBrother = false;
		canHugoReachBrother = false;
		canHugoReachRostov = false;
		waitToCheckIfOskarIsDead = k_invalidTime;
		hasRostovAttackedHugo = false;
		hasDarwinAskedToNotKillHugo = false;
		canDarwinComeClose = false;
	}
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
	UISystem _uiSystem;

private:
	LevelStages _currentLevelStage = MARKETING_PHONE_STAGE;
	DarwinConfrontationStageData _darwinConversationStageData;
	GangsterConfrontationStageData _gangsterConfrontationStageData;
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

inline Entity& getLastAddedEntity()
{
	EntityManager& entityManager = LevelManager::getCurrentLevel()->_entityManager;
	return entityManager._entities[entityManager._lastValidEntityId];
}

inline Entity& addEntity(Vec2 position = Vec2())
{
	Entity& entity = LevelManager::getCurrentLevel()->_entityManager.addEntity();
	auto* t = addComponentToEntity<TransformComponent>(entity);
	t->position = position;
	t->previousPosition = position;
	t->startingPosition = position;
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

// Takes a screen position and converts it to camera/world space.
inline Vec2 convertScreenPositionToCameraSpace(Vec2 posInScreenSpace)
{
	Camera& camera = LevelManager::getCurrentLevel()->_levelCamera;

	if (posInScreenSpace.x > k_baseGameWidth || posInScreenSpace.y > k_baseGameHeight)
	{
		D_LOG(WARNING, "convertToCameraSpace(): Converting screen pos beyond screen bounds");
	}

	Vec2 posInCameraSpace { posInScreenSpace.x + camera.position.x, posInScreenSpace.y + camera.position.y };
	posInCameraSpace.x = posInCameraSpace.x * camera.zoom - camera.zoomFocusPoint.x;
	posInCameraSpace.y = posInCameraSpace.y * camera.zoom - camera.zoomFocusPoint.y;
	return posInCameraSpace;
}

// Expects a worldRect with x,y in world position and w,h as the desired draw size
// Used to render sprites, since they should be drawn taking into account the camera pos and zoom
inline SDL_FRect convertWorldRectToCameraSpace(const SDL_FRect& worldRect)
{
	Camera& camera = LevelManager::getCurrentLevel()->_levelCamera;

	Vec2 posInCameraSpace{ worldRect.x - camera.position.x, worldRect.y - camera.position.y };
	posInCameraSpace.x = posInCameraSpace.x * camera.zoom + camera.zoomFocusPoint.x;
	posInCameraSpace.y = posInCameraSpace.y * camera.zoom + camera.zoomFocusPoint.y;

	Vec2 sizeInCameraSpace{ worldRect.w * camera.zoom, worldRect.h * camera.zoom };

	return { posInCameraSpace.x, posInCameraSpace.y, sizeInCameraSpace.x, sizeInCameraSpace.y };
}
