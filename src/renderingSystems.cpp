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
	}
}

void DebugCollidersSystem::render(ECSLevel* currentLevel, float renderAlpha)
{
#ifndef RELEASE_BUILD
	if (!s_debugCollidersEnabled)
	{
		return;
	}

	for (Entity& player : currentLevel->getAllEntities())
	{
		if (player.id == k_invalidId)
		{
			continue;
		}

		if (!currentLevel->entityHasComponent<MovementComponent>(player))
		{
			continue;
		}

		bool foundCollisionWithWorld = false;

		//TODO: Expand later, since this only debugs player vs world collisions
		for (Entity& possibleCollider : currentLevel->getAllEntities())
		{
			if (possibleCollider.id == k_invalidId || player.id == possibleCollider.id)
			{
				continue;
			}

			if (!currentLevel->entityHasComponent<RectColliderComponent>(possibleCollider) ||
				!currentLevel->entityHasComponent<TransformComponent>(possibleCollider))
			{
				continue;
			}

			RectCollider& playerRectCollider = currentLevel->getComponentFromEntity<RectColliderComponent>(player)->collider;
			RectCollider& rectColliderB = currentLevel->getComponentFromEntity<RectColliderComponent>(possibleCollider)->collider;

			Vec2& playerPosition = currentLevel->getComponentFromEntity<TransformComponent>(player)->position;
			Vec2& positionB = currentLevel->getComponentFromEntity<TransformComponent>(possibleCollider)->position;


			if (aabb(playerPosition, positionB, playerRectCollider, rectColliderB))
			{
				debugRect(playerPosition, playerRectCollider, { 0, 255, 0, 255 });
				debugRect(positionB, rectColliderB, { 0, 255, 0, 255 });
				foundCollisionWithWorld = true;
			}
			else
			{
				if (!foundCollisionWithWorld)
				{
					debugRect(playerPosition, playerRectCollider, { 255, 255, 0, 255 });
				}
				debugRect(positionB, rectColliderB, { 255, 255, 0, 255 });
			}
		}
	}

#endif // RELEASE_BUILD
}

void DebugCollidersSystem::debugRect(Vec2 position, RectCollider collider, SDL_Color color)
{
	SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, color.a);
	Vec2 colliderPosition = getColliderPosition(position, collider);
	SDL_FRect debugRect{ colliderPosition.x, colliderPosition.y, collider.size.x, collider.size.y };
	SDL_RenderRect(s_renderer, &debugRect);
	SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 1);
}