#pragma once

#include "core/entityManager.h"
#include "core/componentManager.h"
#include "core/systems.h"

#include "components.h"
#include "core/lib.h"
#include "core/input.h"

#include <SDL3/SDL_rect.h>

// Systems Globals
inline EntityManager s_entityManager;
inline ComponentManager s_componentManager;

inline SavePreviousPositionSystem s_savePositionSystem;
inline RenderingSystem s_renderingSystem;
inline AnimationSystem s_animationSystem;
inline MovementSystem s_characterMovementSystem;
inline DebugSystem s_debugCollidersSystem;
inline CrosshairSystem s_crosshairSystem;
inline CombatSystem s_attackingSystem;
inline UISystem s_uiSystem;

template<typename T>
bool entityHasComponent(Entity& entity)
{
	return s_componentManager.entityHasComponent<T>(entity);
}

template<typename T>
T* addComponentToEntity(Entity& entity)
{
	return s_componentManager.addComponentToEntity<T>(entity);
}

template<typename T>
void removeComponentFromEntity(Entity& entity)
{
	return s_componentManager.removeComponentFromEntity<T>(entity);
}

template<typename T>
T* getComponentFromEntity(Entity& entity)
{
	return s_componentManager.getComponentFromEntity<T>(entity);
}

inline Entity& getLastAddedEntity()
{
	return s_entityManager._entities[s_entityManager._lastValidEntityId];
}

inline Entity& addEntity(const char* debugName, Vec2 position = Vec2())
{
	Entity& entity = s_entityManager.addEntity();

	strncpy(entity.debugName, debugName, k_entityMaxNameCharacters);
	auto* t = addComponentToEntity<TransformComponent>(entity);
	t->position = position;
	t->previousPosition = position;
	t->startingPosition = position;
	return entity;
}

inline void clearEntityComponentsBitmask(Entity& entity)
{
	s_entityManager.clearEntityComponentBitmask(entity);
}

inline std::array<Entity, k_maxNumberOfEntities>& getAllEntities()
{
	return s_entityManager._entities;
}

inline Entity& getEntityById(uint32_t id)
{
	return s_entityManager._entities[id];
}

enum CameraShakeType
{
	NO_SHAKE,
	LIGHT_SHAKE,
	LIGHT_MEDIUM_SHAKE,
	MEDIUM_SHAKE,
};

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

	CameraShakeType cameraShakeToPerform = NO_SHAKE;
	float delayToShake = 0.f;
	float delayToShakeTimer = k_invalidTime;

	// Point where the zoom will be focused on. By default it's the center of the screen.
	Vec2 zoomFocusPoint = { k_baseGameWidth / 2, k_baseGameHeight / 2 };

	void doShake(CameraShakeType type, float delay)
	{
		cameraShakeToPerform = type;
		delayToShake = delay;
		startTimer(delayToShakeTimer);
	}
};

enum LevelStages
{
    MARKETING_PHONE_STAGE,
    FIRST_DAD_PHONE_STAGE,
    DARWIN_CONVERSATION_STAGE,
    GANGSTER_CONFRONTATION_STAGE,
	PHONE_CONFRONTATION_STAGE,
	FREE_STAGE,
    LEVEL_STAGES_COUNT,
};
static const char* s_levelStagesString = { "MARKETING_PHONE_STAGE\0FIRST_DAD_PHONE_STAGE\0DARWIN_CONVERSATION_STAGE\0GANGSTER_CONFRONTATION_SAGE\0PHONE_CONFRONTATION_STAGE\0FREE_STAGE\0" };

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

struct PhoneConfrontationStage
{
	float waitForHugoCallTimer = 0.f;
	bool hasHugoPhoneStartedRinging = false;
	float lookAtDarwinTimer = k_invalidTime;
	float askForDieselRepeatTimer = k_invalidTime;
	bool hasDarwinComplainedAboutKillingHugo = false;
	float darwinFinalDialogueTimer = k_invalidTime;
	bool canHugoProvokeDarwin = false;

	void reset()
	{
		waitForHugoCallTimer = 0.f;
		hasHugoPhoneStartedRinging = false;
		lookAtDarwinTimer = k_invalidTime;
		hasDarwinComplainedAboutKillingHugo = false;
		darwinFinalDialogueTimer = k_invalidTime;
		askForDieselRepeatTimer = k_invalidTime;
		canHugoProvokeDarwin = false;
	}
};

class Level
{
public:
	void start();
	void update();
	void imguiRender();
	void render(float renderAlpha);

	Camera _levelCamera;

private:
	LevelStages _currentLevelStage = MARKETING_PHONE_STAGE;
	DarwinConfrontationStageData _darwinConversationStageData;
	GangsterConfrontationStageData _gangsterConfrontationStageData;
	PhoneConfrontationStage _phoneConfrontationStageData;
};

struct LevelManager
{
	inline static uint16_t s_currentLevelIndex = 0;
	inline static Level* _levels[k_maxLevels] = { new Level() };

	inline static Level* getCurrentLevel()
	{
		return _levels[s_currentLevelIndex];
	}
};

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
