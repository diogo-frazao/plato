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

void InputMovementSystem::update(ECSLevel* currentLevel, float deltaTime)
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

		if (isKeyDown(SDL_SCANCODE_D))
		{
			auto* movementComponent = currentLevel->getComponentFromEntity<MovementComponent>(entity);
			auto* transformComponent = currentLevel->getComponentFromEntity<TransformComponent>(entity);
			transformComponent->position.x += movementComponent->velocity * deltaTime;
		}

		if (isKeyDown(SDL_SCANCODE_A))
		{
			auto* movementComponent = currentLevel->getComponentFromEntity<MovementComponent>(entity);
			auto* transformComponent = currentLevel->getComponentFromEntity<TransformComponent>(entity);
			transformComponent->position.x -= movementComponent->velocity * deltaTime;
		}
	}
}