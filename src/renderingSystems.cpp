#include "core/systemManager.h"
#include "core/ecsLevel.h"
#include "renderingComponents.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>

void DrawSpriteSystem::render(ECSLevel* currentLevel)
{
	for (Entity& entity : currentLevel->getAllEntities())
	{
		if (entity.id == k_invalidId)
		{
			continue;
		}

		if (!currentLevel->entityHasComponent<SpriteComponent>(entity))
		{
			continue;
		}

		//TODO: improve
		static const std::string atlasPath(RESOURCES_PATH "atlas.png");
		static SDL_Texture* texture = IMG_LoadTexture(s_renderer, atlasPath.c_str());
		if (!texture)
		{
			D_ASSERT(false, "Failed to load atlas texture. Error %s", SDL_GetError());
		}
		SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

		SpriteComponent* spriteComponent = currentLevel->getComponentFromEntity<SpriteComponent>(entity);
		SDL_FRect rect;
		rect.x = k_baseGameWidth / 2;
		rect.y = k_baseGameHeight / 2;
		rect.w = spriteComponent->size.x;
		rect.h = spriteComponent->size.y;
		SDL_RenderTexture(s_renderer, texture, nullptr, &rect);
	}
}