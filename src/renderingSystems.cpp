#include "core/systemManager.h"
#include "core/ecsLevel.h"
#include "renderingComponents.h"
#include "core/constants.h"
#include "core/input.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>

SDL_Texture* loadAtlasTexture()
{
	const std::string atlasPath = RESOURCES_PATH + k_atlasFilePath;

	SDL_Texture* texture = IMG_LoadTexture(s_renderer, atlasPath.c_str());
	if (!texture)
	{
		D_ASSERT(false, "Failed to load atlas texture. Error %s", SDL_GetError());
		return nullptr;
	}

	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	return texture;
}

void DrawSpriteSystem::render(ECSLevel* currentLevel, float renderAlpha)
{
	static SDL_FRect src;
	static SDL_FRect target;

	for (Entity& entity : currentLevel->getAllEntities())
	{
		//TODO: Consider moving this somewhere else since it's needed for every system
		if (entity.id == k_invalidId)
		{
			continue;
		}

		if (!currentLevel->entityHasComponent<SpriteComponent>(entity) || 
			!currentLevel->entityHasComponent<TransformComponent>(entity))
		{
			continue;
		}

		static SDL_Texture* atlasTexture = loadAtlasTexture();
		SpriteComponent* spriteComponent = currentLevel->getComponentFromEntity<SpriteComponent>(entity);
		TransformComponent* transformComponent = currentLevel->getComponentFromEntity<TransformComponent>(entity);

		Vec2 interpolatedPosition = lerp(transformComponent->previousPosition, transformComponent->position, renderAlpha);

		src.x = spriteComponent->offset.x;
		src.y = spriteComponent->offset.y;
		src.w = spriteComponent->size.x;
		src.h = spriteComponent->size.y;

		target.x = interpolatedPosition.x;
		target.y = interpolatedPosition.y;
		target.w = spriteComponent->size.x;
		target.h = spriteComponent->size.y;

		SDL_RenderTexture(s_renderer, atlasTexture, &src, &target);
	}
}

void SavePreviousPositionSystem::update(ECSLevel* currentLevel, float deltaTime)
{
	for (Entity& entity : currentLevel->getAllEntities())
	{
		if (entity.id == k_invalidId)
		{
			continue;
		}

		if (!currentLevel->entityHasComponent<TransformComponent>(entity))
		{
			continue;
		}

		auto* transform = currentLevel->getComponentFromEntity<TransformComponent>(entity);
		transform->previousPosition = transform->position;
	}
}

void CharacterMovementSystem::update(ECSLevel* currentLevel, float deltaTime)
{
	for (Entity& entity : currentLevel->getAllEntities())
	{
		if (entity.id == k_invalidId)
		{
			continue;
		}

		if (!currentLevel->entityHasComponent<MovementComponent>(entity) || !currentLevel->entityHasComponent<TransformComponent>(entity))
		{
			continue;
		}

		Entity& character = entity;
		auto* transformComponent = currentLevel->getComponentFromEntity<TransformComponent>(entity);

		if (wasKeyPressedThisFrame(SDL_SCANCODE_R))
		{
			transformComponent->position.x += 0.5f;
		}

		if (isKeyDown(SDL_SCANCODE_D))
		{
			auto* movementComponent = currentLevel->getComponentFromEntity<MovementComponent>(entity);
			transformComponent->position.x += movementComponent->velocity * deltaTime;
		}

		if (isKeyDown(SDL_SCANCODE_A))
		{
			auto* movementComponent = currentLevel->getComponentFromEntity<MovementComponent>(entity);
			transformComponent->position.x -= movementComponent->velocity * deltaTime;
		}

		if (isKeyDown(SDL_SCANCODE_W))
		{
			auto* movementComponent = currentLevel->getComponentFromEntity<MovementComponent>(entity);
			transformComponent->position.y -= movementComponent->velocity * deltaTime;
		}

		if (isKeyDown(SDL_SCANCODE_S))
		{
			auto* movementComponent = currentLevel->getComponentFromEntity<MovementComponent>(entity);
			transformComponent->position.y += movementComponent->velocity * deltaTime;
		}

		//TODO: Create definition of solids and moveable. Solids have their rectcolliders define when they are created. 
		// Moveable are set on update to match transform, ALWAYS after the movement code.
		RectCollider playerCollider;
		playerCollider.topLeftPoint = IVec2(static_cast<int32_t>(transformComponent->position.x), static_cast<int32_t>(transformComponent->position.y));
		playerCollider.size = { 14, 14 };

		for (Entity& collider : currentLevel->getAllEntities())
		{
			if (collider.id == k_invalidId)
			{
				continue;
			}

			if (character.id == collider.id)
			{
				continue;
			}

			if (!currentLevel->entityHasComponent<RectColliderComponent>(collider))
			{
				continue;
			}

			auto* rectCollider = currentLevel->getComponentFromEntity<RectColliderComponent>(collider);
			DebugCollidersSystem::DTO dto{ playerCollider, rectCollider->collider, {255, 255, 0, 255} };
			currentLevel->debugCollisionBetweenRects(dto);
			if (aabb(playerCollider, rectCollider->collider))
			{
				D_LOG(MINI, "Inside");
			}
			else
			{
				D_LOG(MINI, "Outside");
			}
		}
	}
}

void DebugCollidersSystem::debugCollisionBetweenRects(const DebugCollidersSystem::DTO& dto)
{
#ifndef RELEASE_BUILD
	_collidersToDebug[0] = dto;
#endif // RELEASE_BUILD
}

void DebugCollidersSystem::render(ECSLevel* currentLevel, float renderAlpha)
{
#ifndef RELEASE_BUILD
	if (!s_debugCollidersEnabled)
	{
		return;
	}

	static constexpr SDL_Color k_collidingColor = { 0, 255, 0, 255 };
	for (DTO& dto : _collidersToDebug)
	{
		if (aabb(dto.a, dto.b))
		{
			debugRect(dto.a, k_collidingColor);
			debugRect(dto.b, k_collidingColor);
		}
		else
		{
			debugRect(dto.a, dto.color);
			debugRect(dto.b, dto.color);
		}
	}
#endif // RELEASE_BUILD
}

void DebugCollidersSystem::debugRect(RectCollider a, SDL_Color color)
{
	SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, color.a);
	SDL_FRect debugRect{ (float)a.topLeftPoint.x, (float)a.topLeftPoint.y, (float)a.size.x, (float)a.size.y };
	SDL_RenderRect(s_renderer, &debugRect);
	SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 1);
}