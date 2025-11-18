#include "core/systemManager.h"
#include "core/ecsLevel.h"
#include "renderingComponents.h"
#include "core/constants.h"
#include "core/input.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>

SDL_Texture* RenderingSystem::loadAtlas(AtlasType type)
{
	SDL_Texture* atlas = _loadedAtlasFiles[type];
	if (atlas)
	{
		return atlas;
	}

	static const std::string artPath = "art/";
	std::string atlasFilePath = RESOURCES_PATH + artPath;
	SDL_ScaleMode scaleMode = SDL_SCALEMODE_NEAREST;

	switch (type)
	{
		case GAME:
			atlasFilePath += "atlas.png";
			break;
		case LIGHTS:
			atlasFilePath += "lights_atlas.png";
			scaleMode = SDL_SCALEMODE_LINEAR;
			break;
		default:
			D_ASSERT(false, "Unknown atlas type to load");
			return nullptr;
	}

	SDL_Texture* texture = IMG_LoadTexture(s_renderer, atlasFilePath.c_str());
	if (!texture)
	{
		D_ASSERT(false, "Failed to load atlas texture. Error %s", SDL_GetError());
		return nullptr;
	}

	SDL_SetTextureScaleMode(texture, scaleMode);
	_loadedAtlasFiles[type] = texture;

	return texture;
}

void RenderingSystem::createLightsBuffers()
{
	if (!_backLightsBuffer)
	{
		_backLightsBuffer = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, k_baseGameWidth, k_baseGameHeight);
	}

	if (!_backLightsBuffer)
	{
		D_ASSERT(false, "Lights buffer doesnt' exist, can't apply lighting: %s", SDL_GetError());
		return;
	}

	if (!_frontLightsBuffer)
	{
		_frontLightsBuffer = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, k_baseGameWidth, k_baseGameHeight);
	}

	if (!_frontLightsBuffer)
	{
		D_ASSERT(false, "Lights buffer doesnt' exist, can't apply lighting: %s", SDL_GetError());
		return;
	}
}

void RenderingSystem::render(float renderAlpha)
{
	static SDL_FRect src;
	static SDL_FRect dest;

	// Start drawing to lightsBuffer, make it all black, and set its blend mode to additive
	SDL_SetRenderTarget(s_renderer, _backLightsBuffer);
	SDL_SetTextureBlendMode(_backLightsBuffer, SDL_BLENDMODE_ADD);
	SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 255);
	SDL_RenderClear(s_renderer);

	// Draw each light
	SDL_Texture* lightsTexture = loadAtlas(LIGHTS);
	SDL_SetTextureColorMod(lightsTexture, 255, 255, 255);

	src.x = 45;
	src.y = 0;
	src.w = 93;
	src.h = 90;

	dest.x = 50;
	dest.y = 50;
	dest.w = 93;
	dest.h = 90;

	SDL_RenderTexture(s_renderer, lightsTexture, &src, &dest);
	SDL_SetTextureColorMod(lightsTexture, 255, 255, 255);

	SDL_SetRenderTarget(s_renderer, _frontLightsBuffer);
	SDL_SetTextureBlendMode(_frontLightsBuffer, SDL_BLENDMODE_ADD);
	SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 255);
	SDL_RenderClear(s_renderer);

	src.x = 0;
	src.y = 0;
	src.w = 45;
	src.h = 67;

	dest.x = 0;
	dest.y = 0;
	dest.w = 45;
	dest.h = 67;

	SDL_RenderTexture(s_renderer, lightsTexture, &src, &dest);

	SDL_SetRenderTarget(s_renderer, nullptr);

	for (Entity& entity : getAllEntities())
	{
		//TODO: Consider moving this somewhere else since it's needed for every system
		if (entity.id == k_invalidId)
		{
			continue;
		}

		if (!entityHasComponent<SpriteComponent>(entity) || 
			!entityHasComponent<TransformComponent>(entity) || entity.id == 1)
		{
			continue;
		}

		SDL_Texture* atlasTexture = loadAtlas(GAME);
		SpriteComponent* spriteComponent = getComponentFromEntity<SpriteComponent>(entity);
		TransformComponent* transformComponent = getComponentFromEntity<TransformComponent>(entity);

		Vec2 interpolatedPosition = lerp(transformComponent->previousPosition, transformComponent->position, renderAlpha);

		src.x = spriteComponent->offset.x;
		src.y = spriteComponent->offset.y;
		src.w = spriteComponent->size.x;
		src.h = spriteComponent->size.y;

		dest.x = interpolatedPosition.x;
		dest.y = interpolatedPosition.y;
		dest.w = spriteComponent->size.x;
		dest.h = spriteComponent->size.y;

		SDL_RenderTexture(s_renderer, atlasTexture, &src, &dest);
	}

	SDL_SetTextureBlendMode(_backLightsBuffer, SDL_BLENDMODE_ADD);
	SDL_RenderTexture(s_renderer, _backLightsBuffer, nullptr, nullptr);

	for (Entity& entity : getAllEntities())
	{
		//TODO: Consider moving this somewhere else since it's needed for every system
		if (entity.id == k_invalidId)
		{
			continue;
		}

		if (!entityHasComponent<SpriteComponent>(entity) ||
			!entityHasComponent<TransformComponent>(entity) || entity.id != 1)
		{
			continue;
		}

		SDL_Texture* atlasTexture = loadAtlas(GAME);
		SpriteComponent* spriteComponent = getComponentFromEntity<SpriteComponent>(entity);
		TransformComponent* transformComponent = getComponentFromEntity<TransformComponent>(entity);

		Vec2 interpolatedPosition = lerp(transformComponent->previousPosition, transformComponent->position, renderAlpha);

		src.x = spriteComponent->offset.x;
		src.y = spriteComponent->offset.y;
		src.w = spriteComponent->size.x;
		src.h = spriteComponent->size.y;

		dest.x = interpolatedPosition.x;
		dest.y = interpolatedPosition.y;
		dest.w = spriteComponent->size.x;
		dest.h = spriteComponent->size.y;

		SDL_RenderTexture(s_renderer, atlasTexture, &src, &dest);
	}

	SDL_SetTextureBlendMode(_frontLightsBuffer, SDL_BLENDMODE_ADD);
	SDL_RenderTexture(s_renderer, _frontLightsBuffer, nullptr, nullptr);
}

void DebugCollidersSystem::render()
{
#ifndef RELEASE_BUILD
	if (!s_debugCollidersEnabled)
	{
		return;
	}

	for (Entity& player : getAllEntities())
	{
		if (player.id == k_invalidId)
		{
			continue;
		}

		if (!entityHasComponent<MovementComponent>(player))
		{
			continue;
		}

		bool foundCollisionWithWorld = false;

		//TODO: Expand later, since this only debugs player vs world collisions
		for (Entity& possibleCollider : getAllEntities())
		{
			if (possibleCollider.id == k_invalidId || player.id == possibleCollider.id)
			{
				continue;
			}

			if (!entityHasComponent<RectColliderComponent>(possibleCollider) ||
				!entityHasComponent<TransformComponent>(possibleCollider))
			{
				continue;
			}

			RectCollider& playerRectCollider = getComponentFromEntity<RectColliderComponent>(player)->collider;
			RectCollider& rectColliderB = getComponentFromEntity<RectColliderComponent>(possibleCollider)->collider;

			Vec2& playerPosition = getComponentFromEntity<TransformComponent>(player)->position;
			Vec2& positionB = getComponentFromEntity<TransformComponent>(possibleCollider)->position;


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
	SDL_FRect debugRect{ colliderPosition.x, colliderPosition.y, (float)collider.size.x, (float)collider.size.y };
	SDL_RenderRect(s_renderer, &debugRect);
	SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 1);
}

#pragma region Movement Systems

void SavePreviousPositionSystem::update()
{
	for (Entity& entity : getAllEntities())
	{
		if (entity.id == k_invalidId)
		{
			continue;
		}

		if (!entityHasComponent<TransformComponent>(entity))
		{
			continue;
		}

		auto* transform = getComponentFromEntity<TransformComponent>(entity);
		transform->previousPosition = transform->position;
	}
}

void CharacterMovementSystem::update()
{
	for (Entity& entity : getAllEntities())
	{
		if (entity.id == k_invalidId)
		{
			continue;
		}

		if (!entityHasComponent<MovementComponent>(entity) || !entityHasComponent<TransformComponent>(entity))
		{
			continue;
		}

		Entity& character = entity;
		auto* transformComponent = getComponentFromEntity<TransformComponent>(character);
		auto* movementComponent = getComponentFromEntity<MovementComponent>(character);

		bool isMovingHorizontally = isKeyDown(SDL_SCANCODE_D) || isKeyDown(SDL_SCANCODE_A);
		bool wantsToChangeDirection = (isKeyDown(SDL_SCANCODE_D) && movementComponent->currentSpeed.x < 0.f) ||
			(isKeyDown(SDL_SCANCODE_A) && movementComponent->currentSpeed.x > 0.f);

		float horizontalSpeedMultiplier = 1.f;

		if (!movementComponent->isGrounded)
		{
			horizontalSpeedMultiplier = 0.75f;
		}
		else
		{
			if (wantsToChangeDirection)
			{
				horizontalSpeedMultiplier = 3.f;
			}
		}

		if (isKeyDown(SDL_SCANCODE_Q))
		{
			transformComponent->previousPosition = Vec2(0, 0);
			transformComponent->position = Vec2(0, 0);
			movementComponent->currentSpeed.x = 0;
			movementComponent->currentSpeed.y = 0;
		}

		if (isKeyDown(SDL_SCANCODE_D))
		{
			movementComponent->currentSpeed.x = approach(movementComponent->currentSpeed.x, movementComponent->maxHorizontalSpeed,
				movementComponent->runAcceleration * horizontalSpeedMultiplier * k_deltaTime);
		}

		if (isKeyDown(SDL_SCANCODE_A))
		{
			movementComponent->currentSpeed.x = approach(movementComponent->currentSpeed.x, -movementComponent->maxHorizontalSpeed,
				movementComponent->runAcceleration * horizontalSpeedMultiplier * k_deltaTime);
		}

		if (!isMovingHorizontally)
		{
			movementComponent->currentSpeed.x = approach(movementComponent->currentSpeed.x, 0, movementComponent->friction * k_deltaTime);
		}

		if (isKeyDown(SDL_SCANCODE_SPACE) && movementComponent->isGrounded)
		{
			movementComponent->currentSpeed.y = -movementComponent->jumpSpeed;
			movementComponent->isGrounded = false;
		}

		if (!movementComponent->isGrounded)
		{
			movementComponent->currentSpeed.y = approach(movementComponent->currentSpeed.y, movementComponent->maxVerticalSpeed, movementComponent->gravity * k_deltaTime);
		}

		processHorizontalMovement(&character);
		processVerticalMovement(&character);
	}
}

void CharacterMovementSystem::processVerticalMovement(Entity* self)
{
	auto* transformComponent = getComponentFromEntity<TransformComponent>(*self);
	auto* movementComponent = getComponentFromEntity<MovementComponent>(*self);

	movementComponent->remainder.y += movementComponent->currentSpeed.y;
	int32_t pixelsToMove = round(movementComponent->remainder.y);

	if (pixelsToMove == 0)
	{
		// Even if we're not moving, check if we're grounded
		Vec2 positionToCheck = { transformComponent->position.x, transformComponent->position.y + 1 };
		if (willCollideWithSolidAtPosition(self, positionToCheck))
		{
			movementComponent->isGrounded = true;
		}
		else
		{
			movementComponent->isGrounded = false;
		}

		return;
	}

	movementComponent->remainder.y -= pixelsToMove;
	int8_t movementDirection = sign(pixelsToMove);

	while (pixelsToMove != 0)
	{
		// We need to check collision one pixel below/above before actually moving
		Vec2 positionToCheck = { transformComponent->position.x, transformComponent->position.y + movementDirection };
		if (!willCollideWithSolidAtPosition(self, positionToCheck))
		{
			transformComponent->position.y += movementDirection;
			pixelsToMove -= movementDirection;
		}
		else
		{
			bool isFalling = movementComponent->currentSpeed.y > 0.f;
			if (isFalling)
			{
				movementComponent->isGrounded = true;
			}

			movementComponent->currentSpeed.y = 0;
			return;
		}
	}
}

void CharacterMovementSystem::processHorizontalMovement(Entity* self)
{
	auto* transformComponent = getComponentFromEntity<TransformComponent>(*self);
	auto* movementComponent = getComponentFromEntity<MovementComponent>(*self);

	movementComponent->remainder.x += movementComponent->currentSpeed.x;
	int32_t pixelsToMove = round(movementComponent->remainder.x);

	if (pixelsToMove == 0)
	{
		return;
	}

	movementComponent->remainder.x -= pixelsToMove;
	int8_t movementDirection = sign(pixelsToMove);

	while (pixelsToMove != 0)
	{
		// We need to check collision one pixel in front before actually moving
		Vec2 positionToCheck = { transformComponent->position.x + movementDirection, transformComponent->position.y };

		if (!willCollideWithSolidAtPosition(self, positionToCheck))
		{
			transformComponent->position.x += movementDirection;
			pixelsToMove -= movementDirection;
		}
		else
		{
			movementComponent->currentSpeed.x = 0;
			return;
		}
	}
}

bool CharacterMovementSystem::willCollideWithSolidAtPosition(Entity* self, const Vec2 positionToCheck)
{
	RectCollider& selfRectCollider = getComponentFromEntity<RectColliderComponent>(*self)->collider;

	for (Entity& entity : getAllEntities())
	{
		if (entity.id == k_invalidId || entity.id == self->id)
		{
			continue;
		}

		if (!entityHasComponent<RectColliderComponent>(entity) ||
			!entityHasComponent<TransformComponent>(entity))
		{
			continue;
		}

		if (!getComponentFromEntity<RectColliderComponent>(entity)->isSolid)
		{
			continue;
		}

		Vec2& levelGeometryPosition = getComponentFromEntity<TransformComponent>(entity)->position;
		RectCollider& levelGeometryCollider = getComponentFromEntity<RectColliderComponent>(entity)->collider;

		if (aabb(positionToCheck, levelGeometryPosition, selfRectCollider, levelGeometryCollider))
		{
			return true;
		}
	}

	return false;
}

#pragma endregion