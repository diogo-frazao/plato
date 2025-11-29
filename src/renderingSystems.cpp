#include "core/systemManager.h"
#include "level.h"
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
		case GAME_ATLAS:
			atlasFilePath += "atlas.png";
			break;
		case LIGHTS_ATLAS:
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
	computeLightsAtLayer(BACK_LIGHTS_LAYER);
	computeLightsAtLayer(FRONT_LIGHTS_LAYER);

	renderSpritesAtLayer(BEHIND_CHAR_LAYER, renderAlpha);
	renderLightsAtLayer(BACK_LIGHTS_LAYER);
	renderSpritesAtLayer(CHARACTER_LAYER, renderAlpha);
	renderSpritesAtLayer(IN_FRONT_CHAR_LAYER, renderAlpha);
	renderLightsAtLayer(FRONT_LIGHTS_LAYER);
	renderSpritesAtLayer(LEVEL_GEOMETRY_LAYER, renderAlpha);
}

void RenderingSystem::renderSpritesAtLayer(LayerType layer, float renderAlpha)
{
	// Render directly to the window
	SDL_SetRenderTarget(s_renderer, nullptr);

	for (Entity& entity : getAllEntities())
	{
		if (entity.id == k_invalidId)
		{
			continue;
		}

		if (!entityHasComponent<SpriteComponent>(entity) ||
			(getComponentFromEntity<SpriteComponent>(entity)->layer != layer) ||
			!entityHasComponent<TransformComponent>(entity))
		{
			continue;
		}

		SpriteComponent* spriteComponent = getComponentFromEntity<SpriteComponent>(entity);
		TransformComponent* transformComponent = getComponentFromEntity<TransformComponent>(entity);

		Vec2 interpolatedPosition = lerp(transformComponent->previousPosition, transformComponent->position, renderAlpha);

		Vec2 cameraPosition = LevelManager::getCurrentLevel()->_levelCamera.position;

		_src.x = spriteComponent->offset.x;
		_src.y = spriteComponent->offset.y;
		_src.w = spriteComponent->size.x;
		_src.h = spriteComponent->size.y;

		_dest.x = interpolatedPosition.x - cameraPosition.x;
		_dest.y = interpolatedPosition.y;
		_dest.w = spriteComponent->size.x * transformComponent->scale.x;
		_dest.h = spriteComponent->size.y * transformComponent->scale.y;

		SDL_RenderTexture(s_renderer, loadAtlas(spriteComponent->atlas), &_src, &_dest);
	}
}

SDL_Texture* RenderingSystem::getTargetLightsBuffer(LayerType layer)
{
	SDL_Texture* targetBuffer = nullptr;
	switch (layer)
	{
	case BACK_LIGHTS_LAYER:
		targetBuffer = _backLightsBuffer;
		break;
	case FRONT_LIGHTS_LAYER:
		targetBuffer = _frontLightsBuffer;
		break;
	default:
		D_ASSERT(false, "Invalid light layer to compute");
		return nullptr;
	}

	return targetBuffer;
}

void RenderingSystem::renderLightsAtLayer(LayerType layer)
{
	SDL_Texture* targetBuffer = getTargetLightsBuffer(layer);
	SDL_RenderTexture(s_renderer, targetBuffer, nullptr, nullptr);
}

void RenderingSystem::computeLightsAtLayer(LayerType layer)
{
	SDL_Texture* targetBuffer = getTargetLightsBuffer(layer);

	// Start drawing to lightsBuffer, make it all black, and set its blend mode to additive
	SDL_SetRenderTarget(s_renderer, targetBuffer);
	SDL_SetTextureBlendMode(targetBuffer, SDL_BLENDMODE_ADD);
	SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 255);
	SDL_RenderClear(s_renderer);

	// Draw each light
	for (Entity& entity : getAllEntities())
	{
		if (entity.id == k_invalidId)
		{
			continue;
		}

		if (!entityHasComponent<SpriteComponent>(entity) ||
			(getComponentFromEntity<SpriteComponent>(entity)->layer != layer) ||
			!entityHasComponent<TransformComponent>(entity))
		{
			continue;
		}

		auto* spriteComponent = getComponentFromEntity<SpriteComponent>(entity);
		auto* transformComponent = getComponentFromEntity<TransformComponent>(entity);

		SDL_Texture* lightsTexture = loadAtlas(spriteComponent->atlas);
		SDL_SetTextureColorMod(lightsTexture, spriteComponent->color.r, spriteComponent->color.g, spriteComponent->color.b);
		SDL_SetTextureAlphaMod(lightsTexture, spriteComponent->color.a);

		Vec2 cameraPosition = LevelManager::getCurrentLevel()->_levelCamera.position;

		_src.x = spriteComponent->offset.x;
		_src.y = spriteComponent->offset.y;
		_src.w = spriteComponent->size.x;
		_src.h = spriteComponent->size.y;

		_dest.x = transformComponent->position.x - cameraPosition.x;
		_dest.y = transformComponent->position.y;
		_dest.w = spriteComponent->size.x * transformComponent->scale.x;
		_dest.h = spriteComponent->size.y * transformComponent->scale.y;

		SDL_RenderTexture(s_renderer, lightsTexture, &_src, &_dest);
		SDL_SetTextureColorMod(lightsTexture, 255, 255, 255);
		SDL_SetTextureAlphaMod(lightsTexture, 255);
	}
}

void DebugSystem::render()
{
#ifndef RELEASE_BUILD
	if (s_debugGridEnabled)
	{
		for (float x = 0; x < k_baseGameWidth; x += 8.f)
		{
			for (float y = 0; y < k_baseGameHeight; y += 8.f)
			{
				debugLine({ x, 0 }, { x, k_baseGameHeight }, { 255, 0, 255, 255 });
				debugLine({ 0, y }, { k_baseGameWidth, y }, { 255, 0, 255, 255 });
			}
		}
	}

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

void DebugSystem::debugRect(Vec2 position, RectCollider collider, SDL_Color color)
{
	SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, color.a);
	Vec2 colliderPosition = getColliderPosition(position, collider);
	SDL_FRect debugRect{ colliderPosition.x, colliderPosition.y, (float)collider.size.x, (float)collider.size.y };
	SDL_RenderRect(s_renderer, &debugRect);
	SDL_SetRenderDrawColor(s_renderer, 255, 255, 255, 255);
}

void DebugSystem::debugLine(Vec2 start, Vec2 end, SDL_Color color)
{
	SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderLine(s_renderer, start.x, start.y, end.x, end.y);
	SDL_SetRenderDrawColor(s_renderer, 255, 255, 255, 255);
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

float calculateHorizontalSpeedMultiplier(MovementComponent* movementComponent)
{
	float horizontalSpeedMultiplier = 1.f;

	if (!movementComponent->isGrounded)
	{
		horizontalSpeedMultiplier = 0.75f;
	}
	else
	{
		bool wantsToChangeDirection = (isKeyDown(s_moveRightKey) && movementComponent->currentSpeed.x < 0.f) ||
									  (isKeyDown(s_moveLeftKey) && movementComponent->currentSpeed.x > 0.f);
		if (wantsToChangeDirection)
		{
			horizontalSpeedMultiplier = 3.f;
		}
	}

	return horizontalSpeedMultiplier;
}

void CharacterMovementSystem::update()
{
	Entity& character = getEntityById(k_playerEntityId);

	auto* transformComponent = getComponentFromEntity<TransformComponent>(character);
	auto* movementComponent = getComponentFromEntity<MovementComponent>(character);

	//TODO: remove debug to reset player pos
	if (isKeyDown(SDL_SCANCODE_Q))
	{
		transformComponent->previousPosition = Vec2(0, 0);
		transformComponent->position = Vec2(0, 0);
		movementComponent->currentSpeed.x = 0;
		movementComponent->currentSpeed.y = 0;
	}

	bool wasGrounded = movementComponent->isGrounded;
	float horizontalSpeedMultiplier = calculateHorizontalSpeedMultiplier(movementComponent);

	bool isMovingRight = isKeyDown(s_moveRightKey) && !isKeyDown(s_moveLeftKey);
	bool isMovingLeft = isKeyDown(s_moveLeftKey) && !isKeyDown(s_moveRightKey);

	// Right movement
	if (isMovingRight)
	{
		movementComponent->currentSpeed.x = approach(movementComponent->currentSpeed.x, movementComponent->maxHorizontalSpeed,
			movementComponent->runAcceleration * horizontalSpeedMultiplier * k_deltaTime);
	}

	// Left Movement
	if (isMovingLeft)
	{
		movementComponent->currentSpeed.x = approach(movementComponent->currentSpeed.x, -movementComponent->maxHorizontalSpeed,
			movementComponent->runAcceleration * horizontalSpeedMultiplier * k_deltaTime);
	}

	// Jump
	bool canCoyoteJump = (movementComponent->timeSinceLeftPlatform > k_invalidId && movementComponent->timeSinceLeftPlatform <= movementComponent->coyoteTime);
	bool canJump = movementComponent->isGrounded || (canCoyoteJump);
	if (wasKeyPressedThisFrame(s_jumpKey) && canJump)
	{
		movementComponent->currentSpeed.y = -movementComponent->jumpSpeed;
		movementComponent->isGrounded = false;
		movementComponent->timeSinceLeftPlatform = k_invalidId;
	}

	if (wasKeyReleasedThisFrame(s_jumpKey))
	{
		//movementComponent->currentSpeed.y *= 0.5f;
		//movementComponent->gravity *= 0.95f;
	}

	// Friction
	bool isMovingHorizontally = isMovingRight || isMovingLeft;
	if (!isMovingHorizontally)
	{
		movementComponent->currentSpeed.x = approach(movementComponent->currentSpeed.x, 0, movementComponent->friction * k_deltaTime);
	}

	// Gravity
	if (!movementComponent->isGrounded)
	{
		movementComponent->currentSpeed.y = approach(movementComponent->currentSpeed.y, movementComponent->maxVerticalSpeed, movementComponent->gravity * k_deltaTime);
	}

	processHorizontalMovement(&character);
	processVerticalMovement(&character);

	// After vertical movement was processed and isGrounded was updated, check coyoteTime
	handleCoyoteTime(movementComponent, wasGrounded);
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

void CharacterMovementSystem::handleCoyoteTime(MovementComponent* movementComponent, bool wasGrounded)
{
	bool leftPlatformOnThisFrame = (wasGrounded && !movementComponent->isGrounded) && !wasKeyPressedThisFrame(s_jumpKey);
	if (leftPlatformOnThisFrame)
	{
		movementComponent->timeSinceLeftPlatform = 0.f;
	}

	bool isCoyoteTimerActive = movementComponent->timeSinceLeftPlatform > k_invalidId;
	if (!isCoyoteTimerActive)
	{
		return;
	}

	if (!movementComponent->isGrounded)
	{
		movementComponent->timeSinceLeftPlatform += k_deltaTime;
	}
	else
	{
		movementComponent->timeSinceLeftPlatform = k_invalidId;
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

		if (!getComponentFromEntity<RectColliderComponent>(entity)->isLevelGeometry)
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