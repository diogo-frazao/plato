#include "core/systems.h"
#include "level.h"
#include "components.h"
#include "core/constants.h"
#include "core/input.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>
#include <string>

static bool isAmbientColorValid(SDL_FColor color)
{
	static constexpr SDL_Color k_whiteColor = { 255, 255, 255, 255 };
	return color.r != k_whiteColor.r || color.g != k_whiteColor.g || color.b != k_whiteColor.b;
}

static bool isColorValid(SDL_Color color)
{
	static constexpr SDL_Color k_blackColor = { 0, 0, 0, 0 };
	return color.r != k_blackColor.r || color.g != k_blackColor.g || color.b != k_blackColor.b;
}

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
		case FONT_ATLAS:
			atlasFilePath += "font_atlas.png";
			scaleMode = SDL_SCALEMODE_NEAREST;
			break;
		default:
			D_ASSERT(false, "Unknown atlas type to load");
			return nullptr;
	}

	SDL_Surface* surface = SDL_LoadPNG(atlasFilePath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(s_renderer, surface);
	if (!texture)
	{
		D_ASSERT(false, "Failed to load atlas texture. Error %s", SDL_GetError());
		return nullptr;
	}

	SDL_DestroySurface(surface);
	SDL_SetTextureScaleMode(texture, scaleMode);

	_loadedAtlasFiles[type] = texture;

	return texture;
}

void RenderingSystem::reloadAtlas(AtlasType type)
{
	SDL_Texture* atlas = _loadedAtlasFiles[type];
	if (!atlas)
	{
		return;
	}

	_loadedAtlasFiles[type] = nullptr;
	SDL_DestroyTexture(atlas);

	loadAtlas(type);
}

void RenderingSystem::createInFrontOfEverythingBuffer()
{
	if (!_inFrontOfEverythingBuffer)
	{
		_inFrontOfEverythingBuffer = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, k_baseGameWidth, k_baseGameHeight);
	}

	if (!_inFrontOfEverythingBuffer)
	{
		D_ASSERT(false, "_inFrontOfEverythingBuffer doesnt' exist: %s", SDL_GetError());
		return;
	}
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

void RenderingSystem::renderInFrontOfEverythingTexture()
{
	SDL_SetRenderTarget(s_renderer, _inFrontOfEverythingBuffer);
	SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, _inFrontOfEverythingOpacity);
	SDL_RenderClear(s_renderer);
	SDL_SetRenderTarget(s_renderer, nullptr);
	SDL_RenderTexture(s_renderer, _inFrontOfEverythingBuffer, nullptr, nullptr);
}

void RenderingSystem::render(float renderAlpha)
{
	//TODO: If needed improve performance, since every function interates over every entity
	computeLightsAtLayer(BACK_LIGHTS_LAYER, true);
	computeLightsAtLayer(FRONT_LIGHTS_LAYER);

	renderSpritesAtLayer(BEHIND_LIGHTS_LAYER, renderAlpha);
	// Ambient light is applied here because of true. Everything in BEHIND_LIGHTS_LAYER is affected
	renderLightsAtLayer(BACK_LIGHTS_LAYER, true);
	// We reserve this for baked highlights. Fake lights that shouldn't be affected by ambient light
	renderSpritesAtLayer(BAKED_HIGHLIGHTS_LAYER, renderAlpha);
	// Characters layer. Apply ambient light by multiplying the ambient color directly.
	renderSpritesAtLayer(CHARACTERS_LAYER, renderAlpha);
	// Lights. Will affect everything below
	renderLightsAtLayer(FRONT_LIGHTS_LAYER);
	// Level geometry is not affected by lights nor ambient light.
	renderSpritesAtLayer(LEVEL_GEOMETRY_LAYER, renderAlpha);
	// UI Layer, on top of everything and not affected by anything.
	renderSpritesAtLayer(UI_LAYER, renderAlpha);

	renderInFrontOfEverythingTexture();
}

void RenderingSystem::renderCrosshair(float renderAlpha)
{
	renderSpritesAtLayer(CROSSHAIR_LAYER, renderAlpha);
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

		int32_t currentOffsetX = 0;

		int32_t frameSizeX = spriteComponent->size.x;
		int32_t frameSizeY = spriteComponent->size.y;

		bool isAnimatedSprite = spriteComponent->numberOfFrames > 0;
		if (isAnimatedSprite)
		{
			frameSizeX = spriteComponent->getCurrentSize().x;
			currentOffsetX = spriteComponent->animationData.currentFrame * frameSizeX;
		}

		_src.x = spriteComponent->atlasOffset.x + currentOffsetX;
		_src.y = spriteComponent->atlasOffset.y;
		_src.w = frameSizeX;
		_src.h = frameSizeY;

		float scaledWidth = frameSizeX * transformComponent->scale.x;
		float scaledHeight = frameSizeY * transformComponent->scale.y;

		float scaleOffsetX;
		float scaleOffsetY;

		// TODO: Improve. Due to the character's sprite padding, we're hardcoding the Y scale offset
		if (entity.id == k_playerEntityId)
		{
			scaleOffsetX = (frameSizeX - scaledWidth) * 0.5f;
			scaleOffsetY = (frameSizeY - scaledHeight) * 0.8f;
		}
		else
		{
			float xScalePivotMultiplier = 0.f;
			switch (transformComponent->xScalePivot)
			{
			case CENTER_X_SCALE_PIVOT:
				xScalePivotMultiplier = 0.5f;
				break;
			case LEFT_X_SCALE_PIVOT:
				xScalePivotMultiplier = 0.f;
				break;
			case RIGHT_X_SCALE_PIVOT:
				xScalePivotMultiplier = 1.f;
				break;
			}

			scaleOffsetX = (frameSizeX - scaledWidth) * xScalePivotMultiplier;
			scaleOffsetY = (frameSizeY - scaledHeight);
		}

		_dest.x = interpolatedPosition.x + scaleOffsetX;
		_dest.y = interpolatedPosition.y + scaleOffsetY;
		_dest.w = scaledWidth;
		_dest.h = scaledHeight;

		if (!spriteComponent->drawnAtScreenSpace)
		{
			_dest = convertWorldRectToCameraSpace(_dest);
		}

		SDL_Texture* atlas = loadAtlas(spriteComponent->atlas);

		// Apply ambient light directly to characters layer
		if (layer == CHARACTERS_LAYER)
		{
			// Since ambient light is just a rectangle that fills the whole screen, we can just apply it to the characters
			uint8_t r = (uint8_t)((spriteComponent->color.r * _currentAmbientColor.r) / 255.f);
			uint8_t g = (uint8_t)((spriteComponent->color.g * _currentAmbientColor.g) / 255.f);
			uint8_t b = (uint8_t)((spriteComponent->color.b * _currentAmbientColor.b) / 255.f);

			SDL_SetTextureColorMod(atlas, r, g, b);
		}
		else
		{
			SDL_SetTextureColorMod(atlas, spriteComponent->color.r, spriteComponent->color.g, spriteComponent->color.b);
		}

		SDL_SetTextureAlphaMod(atlas, spriteComponent->color.a);

		SDL_FPoint rotationPoint;
		switch (spriteComponent->rotationPivotType)
		{
		case DEFAULT_CENTER_ROTATION:
			rotationPoint.x = _dest.w / 2;
			rotationPoint.y = _dest.h / 2;
			break;
		case TOP_LEFT_ROTATION:
			rotationPoint.x = 0;
			rotationPoint.y = 0;
			break;
		}

		SDL_RenderTextureRotated(s_renderer, atlas, &_src, &_dest, spriteComponent->rotation, &rotationPoint, spriteComponent->flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
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

void RenderingSystem::renderLightsAtLayer(LayerType layer, bool isAffectedByAmbientLight)
{
	SDL_Texture* targetBuffer = getTargetLightsBuffer(layer);
	if (isAffectedByAmbientLight && isAmbientColorValid(_currentAmbientColor))
	{
		SDL_SetTextureBlendMode(targetBuffer, SDL_BLENDMODE_MUL);
	}

	SDL_RenderTexture(s_renderer, targetBuffer, nullptr, nullptr);
}

void RenderingSystem::computeLightsAtLayer(LayerType layer, bool isAffectedByAmbientLight)
{
	SDL_Texture* targetBuffer = getTargetLightsBuffer(layer);

	// Start drawing to lightsBuffer, make it all black, and set its blend mode to additive
	SDL_SetRenderTarget(s_renderer, targetBuffer);
	SDL_SetTextureBlendMode(targetBuffer, SDL_BLENDMODE_ADD);

	if (isAffectedByAmbientLight && isAmbientColorValid(_currentAmbientColor))
	{
		SDL_SetRenderDrawColor(s_renderer, _currentAmbientColor.r, _currentAmbientColor.g, _currentAmbientColor.b, 255);
	}
	else
	{
		SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 255);
	}

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

		_src.x = spriteComponent->atlasOffset.x;
		_src.y = spriteComponent->atlasOffset.y;
		_src.w = spriteComponent->size.x;
		_src.h = spriteComponent->size.y;

		_dest.x = transformComponent->position.x;
		_dest.y = transformComponent->position.y;
		_dest.w = spriteComponent->size.x * transformComponent->scale.x;
		_dest.h = spriteComponent->size.y * transformComponent->scale.y;

		_dest = convertWorldRectToCameraSpace(_dest);

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

	static constexpr SDL_Color k_defaultCollidersColor = { 255, 255, 0, 255 };
	static constexpr SDL_Color k_defaultCollisionDetectedColliderColor = { 0, 255, 0, 255 };

	static constexpr SDL_Color k_attackCollidersColor = { 65, 105, 225, 255 };
	static constexpr SDL_Color k_attackDetectedCollidersColor = { 255, 0, 0, 255 };

	Entity& player = getEntityById(k_playerEntityId);

	bool foundCollision = false;

	// Debug Player vs world
	for (Entity& possibleCollider : getAllEntities())
	{
		if (possibleCollider.id == k_invalidId || player.id == possibleCollider.id)
		{
			continue;
		}

		if (!entityHasComponent<RectColliderComponent>(possibleCollider))
		{
			continue;
		}

		RectCollider& playerRectCollider = getComponentFromEntity<RectColliderComponent>(player)->collider;
		RectCollider& rectColliderB = getComponentFromEntity<RectColliderComponent>(possibleCollider)->collider;

		Vec2& playerPosition = getComponentFromEntity<TransformComponent>(player)->position;
		Vec2& positionB = getComponentFromEntity<TransformComponent>(possibleCollider)->position;

		if (aabb(playerPosition, positionB, playerRectCollider, rectColliderB))
		{
			debugRect(playerPosition, playerRectCollider, k_defaultCollisionDetectedColliderColor);
			debugRect(positionB, rectColliderB, k_defaultCollisionDetectedColliderColor);
			foundCollision = true;
		}
		else
		{
			if (!foundCollision)
			{
				debugRect(playerPosition, playerRectCollider, k_defaultCollidersColor);
			}
			debugRect(positionB, rectColliderB, k_defaultCollidersColor);
		}
	}

	// Debug attack colliders
	for (Entity& possibleCollider : getAllEntities())
	{
		if (possibleCollider.id == k_invalidId || !entityHasComponent<AttackingComponent>(possibleCollider) || !entityHasComponent<RectColliderComponent>(possibleCollider))
		{
			continue;
		}

		auto* t = getComponentFromEntity<TransformComponent>(possibleCollider);
		auto* c = getComponentFromEntity<RectColliderComponent>(possibleCollider);

		for (RectCollider& attackCollider : CombatSystem::s_attackCollisionsToDebugThisFrame)
		{
			// If the attack will land, color attack collider + entity damaged red
			if (aabb({ 0.f, 0.f }, t->position, attackCollider, c->collider))
			{
				debugRect({ 0.f, 0.f }, attackCollider, k_attackDetectedCollidersColor);
				debugRect(t->position, c->collider, k_attackDetectedCollidersColor);
			}
			// If the attack didn't land, don't touch on the entity, but color the attacking colider pink
			else
			{
				debugRect({ 0.f, 0.f }, attackCollider, k_attackCollidersColor);
			}
		}
	}

#endif // RELEASE_BUILD
}

void DebugSystem::debugRect(Vec2 worldPosition, RectCollider collider, SDL_Color color, bool isScreenSpaceRect)
{
	SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, color.a);
	Vec2 colliderPosition = getColliderPosition(worldPosition, collider);

	SDL_FRect debugRect{ colliderPosition.x, colliderPosition.y , (float)collider.size.x, (float)collider.size.y };
	if (!isScreenSpaceRect)
	{
		debugRect = convertWorldRectToCameraSpace(debugRect);
	}

	SDL_RenderRect(s_renderer, &debugRect);
	SDL_SetRenderDrawColor(s_renderer, 255, 255, 255, 255);
}

void DebugSystem::debugLine(Vec2 start, Vec2 end, SDL_Color color)
{
	SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderLine(s_renderer, start.x, start.y, end.x, end.y);
	SDL_SetRenderDrawColor(s_renderer, 255, 255, 255, 255);
}

void DebugSystem::debugPoint(Vec2 position, SDL_Color color)
{
	SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderPoint(s_renderer, position.x, position.y);
	SDL_SetRenderDrawColor(s_renderer, 255, 255, 255, 255);
}

void CrosshairSystem::update()
{
	Entity& crosshair = getEntityById(k_crosshairEntityId);
	auto* s = getComponentFromEntity<SpriteComponent>(crosshair);

	//s->color.a = s_corsshairOpacity;
	s->color.a = 0;

	// Follow mouse
	{
		SDL_HideCursor();
		auto* t = getComponentFromEntity<TransformComponent>(crosshair);
		Vec2 targetPosition = { s_mousePositionThisFrameInScreenSpace.x - 3.5f, s_mousePositionThisFrameInScreenSpace.y - 4 };
		t->position = targetPosition;
	}

	// Reset crosshair sprite
	if (isTimerOngoing(_resetSpritetimer))
	{
		_resetSpritetimer += k_deltaTime;
		if (_resetSpritetimer >= 0.2f)
		{
			s->setSpriteData(CROSSHAIR_MELEE_WEAPON_SPRITE);
			invalidateTimer(_resetSpritetimer);
		}
	}

	// Smear movement and logic
	if (_crosshairSmear.entity)
	{
		auto* t = getComponentFromEntity<TransformComponent>(*_crosshairSmear.entity);
		t->position.x += _crosshairSmear.directionToMove.x * _crosshairSmear.moveSpeed * k_deltaTime;
		t->position.y += _crosshairSmear.directionToMove.y * _crosshairSmear.moveSpeed * k_deltaTime;
	}
}

void CrosshairSystem::crosshairMeleeHitFeedback(Vec2 hitLocation)
{
	// Crosshair hit sprite
	{
		Entity& crosshair = getEntityById(k_crosshairEntityId);
		auto* s = getComponentFromEntity<SpriteComponent>(crosshair);
		s->setSpriteData(CROSSHAIR_MELEE_WEAPON_HIT_SPRITE);

		startTimer(_resetSpritetimer);
	}

	// Smear
	//{
	//	Vec2 smearPosition = { LevelManager::getCurrentLevel()->_levelCamera.worldPosition.x, hitLocation.y };
	//	_crosshairSmear.entity = &addEntity(smearPosition);
	//	auto* sprite = addComponentToEntity<SpriteComponent>(*_crosshairSmear.entity);
	//	sprite->setupSpriteForLayer(SMEAR_MELEE_ATTACK_SPRITE, BEHIND_LIGHTS_LAYER);
	//	sprite->color = { 138, 148, 255, 100 };
	//	sprite->rotationPivotType = TOP_LEFT_ROTATION;

	//	// rotate towards mouse
	//	Vec2 mouseWorldPosition = convertScreenPositionToCameraSpace(s_mousePositionThisFrameInScreenSpace);
	//	float angleToMouse = getAngleBetweenTwoPoints(smearPosition, mouseWorldPosition);
	//	sprite->rotation = angleToMouse;

	//	// set properties to move towards mouse
	//	Vec2 moveDirection = { mouseWorldPosition.x - smearPosition.x, mouseWorldPosition.y - smearPosition.y };
	//	// TODO: normalize
	//	_crosshairSmear.directionToMove = moveDirection;
	//}
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

		if (transform->useDynamicScale)
		{
			transform->scale.x = lerp(transform->scale.x, 1.f, transform->resetScaleLerp);
			transform->scale.y = lerp(transform->scale.y, 1.f, transform->resetScaleLerp);
		}
	}
}

float calculateHorizontalSpeedMultiplier(MovementComponent* movementComponent)
{
	float horizontalSpeedMultiplier = 1.f;

	if (!movementComponent->isGrounded)
	{
		horizontalSpeedMultiplier = 1.f;
	}
	else
	{
		bool wantsToChangeDirection = (isMoveRightKeyDown() && movementComponent->currentSpeed.x < 0.f) ||
									  (isMoveLeftKeyDown() && movementComponent->currentSpeed.x > 0.f);
		if (wantsToChangeDirection)
		{
			horizontalSpeedMultiplier = 4.f;
		}
	}

	return horizontalSpeedMultiplier;
}

void AnimationSystem::update()
{
	for (Entity& entity : getAllEntities())
	{
		if (entity.id == k_invalidId)
		{
			continue;
		}

		if (!entityHasComponent<SpriteComponent>(entity))
		{
			continue;
		}

		auto* sprite = getComponentFromEntity<SpriteComponent>(entity);
		if (sprite->numberOfFrames < 1)
		{
			continue;
		}

		uint32_t milliseondsToChangeToNextFrame = sprite->animationData.millisecondsToChangeToNextFrame;

		bool canChangeLoopSpeed = (sprite->animationData.currentFrame == 0 && sprite->animationData.loopAnimation);
		if (canChangeLoopSpeed)
		{
			milliseondsToChangeToNextFrame = sprite->animationData.millisecondsToLoop;
		}

		if (sprite->animationData.millisecondsSinceLastFrame < milliseondsToChangeToNextFrame)
		{
			sprite->animationData.millisecondsSinceLastFrame += k_targetMillisecondsBetweenFrames;
			continue;
		}

		// If we get here, it's time to move to the next frame
		int32_t lastFrame = sprite->numberOfFrames - 1;

		if (sprite->animationData.currentFrame == lastFrame)
		{
			sprite->animationData.currentFrame = sprite->animationData.loopAnimation ? 0 : lastFrame;
		}
		else
		{
			sprite->animationData.currentFrame++;
			sprite->animationData.currentFrame = min(sprite->animationData.currentFrame, lastFrame);
		}

		sprite->animationData.millisecondsSinceLastFrame = 0.f;

		bool finishedPlayingAnimation = (sprite->animationData.currentFrame == lastFrame && !sprite->animationData.loopAnimation);
		if (finishedPlayingAnimation)
		{
			sprite->animationData.finishedPlayingAnimation = true;
		}
	}
}

void MovementSystem::processMainCharacterMovement()
{
	Entity& player = getEntityById(k_playerEntityId);

	auto* transformComponent = getComponentFromEntity<TransformComponent>(player);
	auto* movementComponent = getComponentFromEntity<MovementComponent>(player);
	auto* spriteComponent = getComponentFromEntity<SpriteComponent>(player);
	auto* attackingComponent = getComponentFromEntity<AttackingComponent>(player);

	bool wasGrounded = movementComponent->isGrounded;
	float horizontalSpeedMultiplier = calculateHorizontalSpeedMultiplier(movementComponent);

	bool isInAllowedStateToMove = player.entityState != ATTACKING_STATE && player.entityState != ON_CUTSCENE_STATE;

	bool isMovingRight = isMoveRightKeyDown() && !isMoveLeftKeyDown() && isInAllowedStateToMove;
	bool isMovingLeft = isMoveLeftKeyDown() && !isMoveRightKeyDown() && isInAllowedStateToMove;

	bool performedJumpThisFrame = false;

	// Right movement
	if (isMovingRight)
	{
		movementComponent->currentSpeed.x = approach(movementComponent->currentSpeed.x, movementComponent->maxHorizontalSpeed,
			movementComponent->runAcceleration * horizontalSpeedMultiplier * k_deltaTime);
		spriteComponent->flipX = false;
	}

	// Left Movement
	if (isMovingLeft)
	{
		movementComponent->currentSpeed.x = approach(movementComponent->currentSpeed.x, -movementComponent->maxHorizontalSpeed,
			movementComponent->runAcceleration * horizontalSpeedMultiplier * k_deltaTime);
		spriteComponent->flipX = true;
	}

	bool canJumpFromCurrentState = player.entityState != ON_CUTSCENE_STATE;

	// Fake jump if we're 2 pixels away from floor or less
	bool canJumpWithoutTouchingFloor = false;
	Vec2 checkGroundBelowPosition{ transformComponent->position.x, transformComponent->position.y + 2 };
	if (canJumpFromCurrentState && wasJumpKeyPressedThisFrame() && !movementComponent->isGrounded && (willCollideWithLevelGeometryAtPosition(&player, checkGroundBelowPosition)))
	{
		canJumpWithoutTouchingFloor = true;
	}

	// Jump
	bool canCoyoteJump = (movementComponent->timeSinceLeftPlatform > k_invalidId && movementComponent->timeSinceLeftPlatform <= movementComponent->coyoteTime);
	bool canJump = movementComponent->isGrounded || canCoyoteJump || canJumpWithoutTouchingFloor;
	if (canJumpFromCurrentState && wasJumpKeyPressedThisFrame() && canJump)
	{
		movementComponent->currentSpeed.y = -movementComponent->jumpSpeed;
		movementComponent->isGrounded = false;
		movementComponent->timeSinceLeftPlatform = k_invalidId;

		performedJumpThisFrame = true;

		// Set horizontal speed to max if we jump while holding the left or right buttons
		if (isMovingRight) { movementComponent->currentSpeed.x = movementComponent->maxHorizontalSpeed; }
		if (isMovingLeft) { movementComponent->currentSpeed.x = -movementComponent->maxHorizontalSpeed; };
	}

	//TODO: Improve since this will reduce also when free falling
	if (canJumpFromCurrentState && wasJumpKeyReleasedThisFrame())
	{
		movementComponent->currentSpeed.y *= 0.5f;
	}

	// Friction
	bool isMovingHorizontally = isMovingRight || isMovingLeft;
	if (!isMovingHorizontally)
	{
		float friction = movementComponent->isGrounded ? movementComponent->friction : movementComponent->airFriction;
		movementComponent->currentSpeed.x = approach(movementComponent->currentSpeed.x, 0, friction * k_deltaTime);
	}

	// Gravity
	if (!movementComponent->isGrounded)
	{
		movementComponent->currentSpeed.y = approach(movementComponent->currentSpeed.y, movementComponent->maxVerticalSpeed, movementComponent->gravity * k_deltaTime);
	}

	processHorizontalMovement(&player);
	processVerticalMovement(&player);

	// After vertical movement was processed and isGrounded was updated, check coyoteTime
	handleCoyoteTime(movementComponent, wasGrounded);

	bool canChangeFromCurrentStateToIdle = player.entityState != ATTACKING_STATE && player.entityState != ON_CUTSCENE_STATE;

	bool isGroundedAndNotMoving = !isMovingHorizontally && movementComponent->isGrounded;
	if (isGroundedAndNotMoving && canChangeFromCurrentStateToIdle)
	{
		if (abs(movementComponent->currentSpeed.x) <= 0.05f)
		{
			player.entityState = IDLE_STATE;
		}
		else
		{
			if (player.entityState != SLOWDOWN_STATE)
			{
				transformComponent->scale.x = 1.15f;
				transformComponent->resetScaleLerp = 0.05f;
			}
			player.entityState = SLOWDOWN_STATE;
		}
	}

	movementComponent->isMovingOnFloor = isMovingHorizontally && movementComponent->isGrounded;
	bool canChangeToTakeOffState = (player.entityState == IDLE_STATE || player.entityState == SLOWDOWN_STATE);
	if (movementComponent->isMovingOnFloor && canChangeToTakeOffState)
	{
		player.entityState = TAKE_OFF_STATE;
		transformComponent->scale.x = 1.3f;
		transformComponent->resetScaleLerp = 1.f;

		Entity& takeoffParticle = addEntity("takeoffParticle", { transformComponent->position.x + 10, transformComponent->position.y + 15 });
		auto* particleTransform = getComponentFromEntity<TransformComponent>(takeoffParticle);
		particleTransform->previousPosition = particleTransform->position;
		particleTransform->scale = { 0.6f, 0.6f };
		addComponentToEntity<SpriteComponent>(takeoffParticle)->setupAnimationForLayer(TAKEOFF_PARTICLE_SPRITE, BEHIND_LIGHTS_LAYER, false, 70, 70);
		getComponentFromEntity<SpriteComponent>(takeoffParticle)->color = { 255, 255, 255, 200 };
	}

	bool canChangeFromTakeOffToRunningState = ((player.entityState == TAKE_OFF_STATE) &&
		spriteComponent->animationData.finishedPlayingAnimation);
	if (canChangeFromTakeOffToRunningState)
	{
		player.entityState = RUNNING_STATE;
	}

	bool canChangeFromFallingToRunState = (player.entityState == FALLING_STATE) && movementComponent->isGrounded && isMovingHorizontally;
	if (canChangeFromFallingToRunState)
	{
		player.entityState = RUNNING_STATE;
	}

	bool changedDirectionThisFrame = (wasMoveRightPressedThisFrame() && movementComponent->currentSpeed.x < 0.f) ||
		(wasMoveLeftPressedThisFrame() && movementComponent->currentSpeed.x > 0.f);
	if (changedDirectionThisFrame)
	{
		Entity& turnParticle = addEntity("turnParticle", { transformComponent->position.x + 12, transformComponent->position.y + 15 });
		auto* particleTransform = getComponentFromEntity<TransformComponent>(turnParticle);
		particleTransform->previousPosition = particleTransform->position;
		particleTransform->scale = { 0.7f, 0.7f };
		addComponentToEntity<SpriteComponent>(turnParticle)->setupAnimationForLayer(TURN_PARTICLE_SPRITE, BEHIND_LIGHTS_LAYER, false, 70, 70);
		getComponentFromEntity<SpriteComponent>(turnParticle)->color = { 255, 255, 255, 200 };
	}

	if (performedJumpThisFrame)
	{
		player.entityState = JUMPING_STATE;
		transformComponent->scale = Vec2(0.8f, 1.5f);
		transformComponent->resetScaleLerp = 0.1f;
	}

	bool canChangeToFallingState = movementComponent->currentSpeed.y > 0.f && player.entityState != ATTACKING_STATE && player.entityState != ON_CUTSCENE_STATE;
	if (canChangeToFallingState)
	{
		player.entityState = FALLING_STATE;
	}

	if (!wasGrounded && movementComponent->isGrounded)
	{
		transformComponent->scale = Vec2(1.4f, 0.6f);
		transformComponent->resetScaleLerp = 0.3f;
	}

	//TODO: Implement random x scale while running, almost like a wave effect

	//TODO: remove later
	static int weaponInHandIndex = 0;
	if (_wasKeyPressedThisFrame(SDL_SCANCODE_K))
	{
		if (++weaponInHandIndex >= WEAPON_TYPE_COUNT)
		{
			weaponInHandIndex = 0;
		}

		getComponentFromEntity<AttackingComponent>(player)->weaponInHand = (WeaponType)weaponInHandIndex;
	}

	// Handle movement animations
	SpriteType animation;
	switch (player.entityState)
	{
	case IDLE_STATE:
	case ON_CUTSCENE_STATE:

		switch (attackingComponent->weaponInHand)
		{
		case NO_WEAPON_TYPE:
			animation = CHARACTER_IDLE_SPRITE;
			break;
		case GOLF_WEAPON_TYPE:
			animation = CHARACTER_WEAPON_GOLF_IDLE_SPRITE;
			break;
		case ROSTOV_WEAPON_PISTOL_TYPE:
			animation = CHARACTER_WEAPON_PISTOL_IDLE_SPRITE;
			break;
		}

		spriteComponent->setAnimationToPlayIfNotPlaying(animation, true, 70, 600);
		break;
	case TAKE_OFF_STATE:
		switch (attackingComponent->weaponInHand)
		{
		case NO_WEAPON_TYPE:
			animation = CHARACTER_TAKEOFF_SPRITE;
			break;
		case GOLF_WEAPON_TYPE:
			animation = CHARACTER_WEAPON_GOLF_TAKEOFF_SPRITE;
			break;
		case ROSTOV_WEAPON_PISTOL_TYPE:
			animation = CHARACTER_WEAPON_PISTOL_TAKEOFF_SPRITE;
			break;
		}

		spriteComponent->setAnimationToPlayIfNotPlaying(animation, false, 60, 60);
		break;
	case RUNNING_STATE:
		switch (attackingComponent->weaponInHand)
		{
		case NO_WEAPON_TYPE:
			animation = CHARACTER_RUN_SPRITE;
			break;
		case GOLF_WEAPON_TYPE:
			animation = CHARACTER_WEAPON_GOLF_RUN_SPRITE;
			break;
		case ROSTOV_WEAPON_PISTOL_TYPE:
			animation = CHARACTER_WEAPON_PISTOL_RUN_SPRITE;
			break;
		}

		spriteComponent->setAnimationToPlayIfNotPlaying(animation, true, 70, 70);
		break;
	case SLOWDOWN_STATE:
		switch (attackingComponent->weaponInHand)
		{
		case NO_WEAPON_TYPE:
			animation = CHARACTER_RUN_SPRITE;
			break;
		case GOLF_WEAPON_TYPE:
			animation = CHARACTER_WEAPON_GOLF_SLOWDOWN_SPRITE;
			break;
		case ROSTOV_WEAPON_PISTOL_TYPE:
			animation = CHARACTER_WEAPON_PISTOL_RUN_SPRITE;
			break;
		}

		spriteComponent->setAnimationToPlayIfNotPlaying(animation, false, 70, 70);
		break;
	case JUMPING_STATE:
		switch (attackingComponent->weaponInHand)
		{
		case NO_WEAPON_TYPE:
			animation = CHARACTER_JUMP_SPRITE;
			break;
		case GOLF_WEAPON_TYPE:
			animation = CHARACTER_WEAPON_GOLF_JUMP_SPRITE;
			break;
		case ROSTOV_WEAPON_PISTOL_TYPE:
			animation = CHARACTER_JUMP_SPRITE;
			break;
		}

		spriteComponent->setAnimationToPlayIfNotPlaying(animation, true, 70, 70);
		break;
	case FALLING_STATE:
		switch (attackingComponent->weaponInHand)
		{
		case NO_WEAPON_TYPE:
			animation = CHARACTER_FALL_SPRITE;
			break;
		case GOLF_WEAPON_TYPE:
			animation = CHARACTER_WEAPON_GOLF_FALL_SPRITE;
			break;
		case ROSTOV_WEAPON_PISTOL_TYPE:
			animation = CHARACTER_FALL_SPRITE;
			break;
		}

		spriteComponent->setAnimationToPlayIfNotPlaying(animation, false, 70, 70);
		break;
	}
}

void MovementSystem::update()
{
	for (Entity& entity : getAllEntities())
	{
		if (entity.id == k_invalidId)
		{
			continue;
		}

		if (entity.id == k_playerEntityId)
		{
			processMainCharacterMovement();
			continue;
		}

		if (!entityHasComponent<MovementComponent>(entity))
		{
			continue;
		}

		auto* movementComponent = getComponentFromEntity<MovementComponent>(entity);

		// Gravity
		if (!movementComponent->isGrounded)
		{
			movementComponent->currentSpeed.y = approach(movementComponent->currentSpeed.y, movementComponent->maxVerticalSpeed, movementComponent->gravity * k_deltaTime);
		}

		// Friction
		if(!movementComponent->isMovingOnFloor)
		{
			float friction = movementComponent->isGrounded ? movementComponent->friction : movementComponent->airFriction;
			movementComponent->currentSpeed.x = approach(movementComponent->currentSpeed.x, 0, friction * k_deltaTime);
		}

		processHorizontalMovement(&entity);
		processVerticalMovement(&entity);

		if (!isEntityInCombatState(entity.entityState))
		{
			if (!movementComponent->isGrounded)
			{
				if (movementComponent->currentSpeed.y > 0.f)
				{
					entity.entityState = FALLING_STATE;
				}
			}
			else
			{
				bool canChangeToIdle = entity.entityState == RUNNING_STATE || entity.entityState == FALLING_STATE;
				if (canChangeToIdle && abs(movementComponent->currentSpeed.x) <= 0.05f)
				{
					entity.entityState = IDLE_STATE;
				}
			}
		}

		//TODO: quando voltar: fix animation speed for hurt 2 + hitbox for hurt two
		//TODO: Fix hardcoded sprites
		switch (entity.entityState)
		{
		case IDLE_STATE:
			getComponentFromEntity<SpriteComponent>(entity)->setAnimationToPlayIfNotPlaying(movementComponent->movementAnimations[0], true, 70, 70);
		}
	}
	
}

void MovementSystem::processVerticalMovement(Entity* self)
{
	auto* transformComponent = getComponentFromEntity<TransformComponent>(*self);
	auto* movementComponent = getComponentFromEntity<MovementComponent>(*self);

	movementComponent->remainder.y += movementComponent->currentSpeed.y;
	int32_t pixelsToMove = round(movementComponent->remainder.y);

	if (pixelsToMove == 0)
	{
		// Even if we're not moving, check if we're grounded
		Vec2 positionToCheck = { transformComponent->position.x, transformComponent->position.y + 1 };
		if (willCollideWithLevelGeometryAtPosition(self, positionToCheck))
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
		if (!willCollideWithLevelGeometryAtPosition(self, positionToCheck))
		{
			transformComponent->position.y += movementDirection;
			pixelsToMove -= movementDirection;
			movementComponent->isGrounded = false;
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

void MovementSystem::processHorizontalMovement(Entity* self)
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

		if (!willCollideWithLevelGeometryAtPosition(self, positionToCheck))
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

void MovementSystem::handleCoyoteTime(MovementComponent* movementComponent, bool wasGrounded)
{
	bool leftPlatformOnThisFrame = (wasGrounded && !movementComponent->isGrounded) && !wasJumpKeyReleasedThisFrame();
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

bool MovementSystem::willCollideWithLevelGeometryAtPosition(Entity* self, const Vec2 positionToCheck)
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

#pragma region Combat System

void CombatSystem::handleProjectileHitDetection(Entity* projectileEntity)
{
	auto* projectile = getComponentFromEntity<ProjectileComponent>(*projectileEntity);
	auto* transform = getComponentFromEntity<TransformComponent>(*projectileEntity);
	auto* collider = getComponentFromEntity<RectColliderComponent>(*projectileEntity);

	for (Entity& targetEntity : getAllEntities())
	{
		if (targetEntity.id == k_invalidId ||
			targetEntity.id == projectile->ownerEntityId)
		{
			continue;
		}

		// Right now bullets only hit other attacking components
		// TODO: Maybe expand so we can stop bullets by firing ours against theirs.,
		if (!entityHasComponent<AttackingComponent>(targetEntity))
		{
			continue;
		}

		auto* aTarget = getComponentFromEntity<AttackingComponent>(targetEntity);
		if (!aTarget->canBeAttacked)
		{
			continue;
		}

		Vec2 targetPosition = getComponentFromEntity<TransformComponent>(targetEntity)->position;
		RectCollider targetCollider = getComponentFromEntity<RectColliderComponent>(targetEntity)->collider;

		if (!aabb(transform->position, targetPosition, collider->collider, targetCollider))
		{
			continue;
		}

		// If we get here it's because the bullet hit a target
		addColliderToDebugList(transform->position, collider->collider);

		//TODO: PROPERLY DELETE THE BULLET
		clearEntityComponentsBitmask(*projectileEntity);

		targetEntity.entityState = SHOT_DYING_STATE;

		D_LOG(LOG, "Entity hit");
	}
}

void CombatSystem::update()
{
	clearDebugCollisions();

	for (Entity& entity : getAllEntities())
	{
		if (entity.id == k_invalidId)
		{
			continue;
		}

		if (entity.id == k_playerEntityId)
		{
			Entity& player = getEntityById(k_playerEntityId);
			auto* a = getComponentFromEntity<AttackingComponent>(player);
			auto* m = getComponentFromEntity<MovementComponent>(player);
			auto* t = getComponentFromEntity<TransformComponent>(player);
			auto* s = getComponentFromEntity<SpriteComponent>(player);
			auto* c = getComponentFromEntity<RectColliderComponent>(player);

			tryStartMainCharacterAttack(&player, a, m, t, s, c);
			handleMainCharacterAttackAnimations(&player, a, m, s);
			continue;
		}

		if (entityHasComponent<ProjectileComponent>(entity))
		{
			handleProjectileHitDetection(&entity);
			continue;
		}

		if (!entityHasComponent<AttackingComponent>(entity))
		{
			continue;
		}

		// Handle Combat related NPC animations
		AttackingComponent* a = getComponentFromEntity<AttackingComponent>(entity);
		SpriteComponent* s = getComponentFromEntity<SpriteComponent>(entity);

		switch (entity.entityState)
		{
		case SHOT_DYING_STATE:
			s->setAnimationToPlayIfNotPlaying(OSKAR_SHOT_DYING_SPRITE, false, 70, 70);
			break;
		}
	}
}

void CombatSystem::tryStartMainCharacterAttack(Entity* player, AttackingComponent* a, MovementComponent* m, TransformComponent* t, SpriteComponent* s, RectColliderComponent* c)
{
	// Prevent attacking while using the mouse for imgui related things
	if (s_isImGuiOpen)
	{
		return;
	}

	if (a->weaponInHand == NO_WEAPON_TYPE)
	{
		return;
	}

	// Attack based on state
	bool canAttackFromCurrentState = player->entityState != ATTACKING_STATE && player->entityState != ON_CUTSCENE_STATE;
	bool canAttack = canAttackFromCurrentState && m->isGrounded && wasAttackKeyPressedThisFrame();
	if (!canAttack)
	{
		return;
	}

	switch (a->weaponInHand)
	{
	case GOLF_WEAPON_TYPE:
		// Attack to the side the mouse is facing
		{
			Vec2 mouseWorldPosition = convertScreenPositionToCameraSpace(s_mousePositionThisFrameInScreenSpace);
			Vec2 characterColliderPosition = getColliderPosition(t->position, c->collider);

			bool shouldAttackInAnotherDirection = (mouseWorldPosition.x > characterColliderPosition.x && s->flipX) ||
				(mouseWorldPosition.x < characterColliderPosition.x && !s->flipX);

			if (shouldAttackInAnotherDirection)
			{
				s->flipX = !s->flipX;
			}

			float attackForwardBoost = s->flipX ? -2.f : 2.f;
			m->currentSpeed.x = attackForwardBoost;
		}

		// Scale effects
		t->scale = Vec2(1.2f, 0.9f);
		t->resetScaleLerp = 0.05f;

		break;
	case ROSTOV_WEAPON_PISTOL_TYPE:

		// Create bullet
		{
			Vec2 bulletPosition = s->flipX ? Vec2{ t->position.x + 11.f, t->position.y + 15.f } : Vec2{ t->position.x + 46.f, t->position.y + 15.f };
			Entity& bullet = addEntity("bullet", bulletPosition);

			auto* bulletTransform = getComponentFromEntity<TransformComponent>(bullet);
			bulletTransform->xScalePivot = LEFT_X_SCALE_PIVOT;
			bulletTransform->scale.x = 0.1f;
			bulletTransform->useDynamicScale = true;
			bulletTransform->resetScaleLerp = 0.05f;

			auto* bulletSprite = addComponentToEntity<SpriteComponent>(bullet);
			bulletSprite->setupSpriteForLayer(PISTOL_BULLET_SPRITE, UI_LAYER);
			bulletSprite->flipX = !s->flipX;

			auto* bulletCollider = addComponentToEntity<RectColliderComponent>(bullet);
			bulletCollider->collider = { {0, 0}, bulletSprite->size };

			SpriteType movementAnimations[] = { PISTOL_BULLET_SPRITE };
			auto* bulletMovement = addComponentToEntity<MovementComponent>(bullet);
			bulletMovement->setupMovementAnimations(movementAnimations);
			bulletMovement->gravity = 0.f;
			bulletMovement->airFriction = 0.f;
			int8_t movementDirection = bulletSprite->flipX ? 1.f : -1.f;
			bulletMovement->currentSpeed.x = 7.f * movementDirection;

			auto* projectile = addComponentToEntity<ProjectileComponent>(bullet);
			projectile->ownerEntityId = player->id;
		}

		break;
	}

	player->entityState = ATTACKING_STATE;
	return;
}

void CombatSystem::handleMainCharacterAttackAnimations(Entity* player, AttackingComponent* a, MovementComponent* m, SpriteComponent* s)
{
	if (a->weaponInHand == NO_WEAPON_TYPE)
	{
		return;
	}

	switch (player->entityState)
	{
	case ATTACKING_STATE:

		// Transition when attacking animation ends.
		if (s->animationData.finishedPlayingAnimation)
		{
			player->entityState = m->isMovingOnFloor ? TAKE_OFF_STATE : IDLE_STATE;
		}

		switch (a->weaponInHand)
		{
		case GOLF_WEAPON_TYPE:
		{
			// Set animation based on state and weapon
			float speedForAttackAnimation = (s->animationData.currentFrame == 2) ? 140 : 70;
			s->setAnimationToPlayIfNotPlaying(CHARACTER_WEAPON_GOLF_ATTACK_MIDDLE_SPRITE, false, speedForAttackAnimation, 70);
			break;
		}
		case ROSTOV_WEAPON_PISTOL_TYPE:
			if (s->animationData.currentFrame == 0)
			{
				s_renderingSystem._inFrontOfEverythingOpacity = 30;
			}
			else
			{
				s_renderingSystem._inFrontOfEverythingOpacity = 0;
			}

			// FX when shooting pistol
			{
				Entity& playerEffects = getEntityById(k_playerEffectsEntityId);
				auto* effectsSprite = getComponentFromEntity<SpriteComponent>(playerEffects);
				effectsSprite->setupAnimationForLayer(CHARACTER_WEAPON_PISTOL_FX_SPRITE, UI_LAYER, false, 70, 70);
				if (effectsSprite->animationData.finishedPlayingAnimation)
				{
					effectsSprite->setupSpriteForLayer(WHITE_ONE_BY_ONE_SPRITE, UI_LAYER);
				}
			}

			s->setAnimationToPlayIfNotPlaying(CHARACTER_WEAPON_PISTOL_FIRE_SPRITE, false, 70, 70);
			break;
		}

		break;
	}
}

#pragma endregion

#pragma region UI System

struct DialogueBoxSprite
{
	// Sprite properties
	AtlasType atlasType = GAME_ATLAS;
	IVec2 atlasOffset{ 0, 0 };
	IVec2 spriteSize{ 0, 0 };
};

enum DialgueSpriteType
{
	DIALOGUE_BASE_SPRITE,
	DIALOGUE_INDICATOR_SPRITE,
	DIALOGUE_INDICATOR_OUTLINE_SPRITE,
	DIALOGUE_FINISHED_INDICATOR_SPRITE,
	DIALOGUE_OPTION_BORDER_SPRITE,
	DIALOGUE_OPTION_HOVERED_BORDER_SPRITE,
	DIALOGUE_TENSION_FADE_SPRITE,
	DIALOGUE_SPRITE_COUNT
};

DialogueBoxSprite k_dialogueSpeechSprites[DIALOGUE_SPRITE_COUNT] = {
	{FONT_ATLAS, {1, 32}, {1,1}},   // Base sprite
	{FONT_ATLAS, {79, 1}, {10,8}},  // Dialogue indicator
	{FONT_ATLAS, {93, 1}, {10,8}},  // Dialogue indicator outline
	{FONT_ATLAS, {1, 1},  {5, 3}},	// Dialogue finished indicator
	{FONT_ATLAS, {1, 7},  {4, 22}},	// Dialogue option border
	{FONT_ATLAS, {1, 39}, {5, 22}},	// Dialogue option hovered border,
	{FONT_ATLAS, {146, 1}, {15, 5}}	// Dialogue tension border,
};

// Things I noticed about the font:
// T and Y are shifted to the left.
// , needs to be drawn a bit below, otherwise it's floating
// For now, " will not be supported. It's ' instead
// For now \ is not supported. It's _ instead

// Everything we draw as part of the UI should have its size as: sprite size on atlas / this value (3)
// UI is made for a target of 540p, but we still draw it on the 180p canvas and just scale it down
static constexpr float k_UIToGameCanvasScale = 3.f;

// Font atlas details
uint8_t k_maxCharactersPerRowOnAtlas = 36;
IVec2 k_characterSizeOnAtlas = { 7, 7 };
IVec2 k_firstCharacterOnAtlasOffset = { 17, 19 };
IVec2 k_spaceBetweenCharactersOnAtas = { 8, 8 };

// If (0,0) speech bubble is drawn exactly at the beginning and end of text
Vec2 k_dialogueOuterPadding = { 3.f, 3.f };
float k_secondsToStartShowingFirstCharacter = 0.3f;
float k_dialogueOutlineHeight = 0.75f;

Vec2 dialogueOptionsOuterPadding{ 5.f, 2.75f };

// Characters details (may change at runtime)
uint8_t k_pixelsBetweenCharacters = 1;
uint8_t k_pixelsBetweenNewLine = 5;

// UI related sizes
Vec2 k_characterSize = { k_characterSizeOnAtlas.x / k_UIToGameCanvasScale,
						 k_characterSizeOnAtlas.y / k_UIToGameCanvasScale };
Vec2 k_speechIndicatorSize = { k_dialogueSpeechSprites[DIALOGUE_INDICATOR_SPRITE].spriteSize.x / k_UIToGameCanvasScale,
							   k_dialogueSpeechSprites[DIALOGUE_INDICATOR_SPRITE].spriteSize.y / k_UIToGameCanvasScale };
Vec2 k_dialogueEndedIndicatorSize = { k_dialogueSpeechSprites[DIALOGUE_FINISHED_INDICATOR_SPRITE].spriteSize.x / k_UIToGameCanvasScale,
							          k_dialogueSpeechSprites[DIALOGUE_FINISHED_INDICATOR_SPRITE].spriteSize.y / k_UIToGameCanvasScale };
Vec2 k_dialogueOptionBorderSize = { k_dialogueSpeechSprites[DIALOGUE_OPTION_BORDER_SPRITE].spriteSize.x / k_UIToGameCanvasScale,
									  k_dialogueSpeechSprites[DIALOGUE_OPTION_BORDER_SPRITE].spriteSize.y / k_UIToGameCanvasScale };

Vec2 k_dialogueHoveredBorderSize = { k_dialogueSpeechSprites[DIALOGUE_OPTION_HOVERED_BORDER_SPRITE].spriteSize.x / k_UIToGameCanvasScale,
									  k_dialogueSpeechSprites[DIALOGUE_OPTION_HOVERED_BORDER_SPRITE].spriteSize.y / k_UIToGameCanvasScale };

Vec2 k_tensionFadeSize = { k_dialogueSpeechSprites[DIALOGUE_TENSION_FADE_SPRITE].spriteSize.x / k_UIToGameCanvasScale,
									  k_dialogueSpeechSprites[DIALOGUE_TENSION_FADE_SPRITE].spriteSize.y / k_UIToGameCanvasScale };

void UISystem::start()
{
	for (int i = 0; k_fontAtlasLayout[i] != '\0'; ++i)
	{
		char c = k_fontAtlasLayout[i];
		_asciiToAtlasIndex[c] = i;
	}
}

void UISystem::update()
{
	if (_cellphone.entity == nullptr)
	{
		_cellphone.entity = &getEntityById(k_cellphoneEntityId);
	}

	// Handle cellphone state
	{
		auto* t = getComponentFromEntity<TransformComponent>(*_cellphone.entity);
		switch (_cellphone.state)
		{
		case CELLPHONE_NOT_VISIBLE_STATE:
			t->position = lerp(t->position, { 10, 200 }, 10 * k_deltaTime);
			break;

		case CELLPHONE_PENDING_CALL_STATE:
			t->position = lerp(t->position, { 10, 163 }, 10 * k_deltaTime);

			if (wasPickupPhoneKeyPressedThisFrame())
			{
				_cellphone.state = CELLPHONE_TALKING;
				pushCellphoneDialogue(_cellphone.textToShowOnAnswer);
				_cellphone.textToShowOnAnswer = INVALID_TEXT;
			}
			break;

		case CELLPHONE_TALKING:
			t->position = lerp(t->position, { 10, 152 }, 10 * k_deltaTime);
			break;

		default:
			D_ASSERT(false, "Unsupported cellphone state");
			break;
		}
	}

	// Prevent executing any code if there's no dialogue
	if (!_currentDialogue.characters[0].isValid())
	{
		return;
	}
	
	// Destroy dialogue if has ended and it's not visible anymore
	if (_currentDialogue.dialogueBoxDynamicXSize <= 1.f && _currentDialogue.state == DIALOGUE_ENDED_STATE)
	{
		destroyCurrentDialogue();
		D_LOG(WARNING, "Dialogue destroyed")
	}

	// Skip dialogue
	if (wasSkipDialogueKeyPressedThisFrame())
	{
		if (_currentDialogue.timeSinceFinalCharacterWasDrawn > 0.f)
		{
			// If the dialogue has options we can't skip it, we need to choose a dialogue option
			bool doesDialogueHaveOptions = _dialogueOptions[0].isValid();
			if (!doesDialogueHaveOptions)
			{
				_currentDialogue.state = DIALOGUE_ENDED_STATE;
			}
		}
		else
		{
			skipDialogue();
		}
	}

	// Handle dialogue options
	bool doesDialogueHaveOptions = _dialogueOptions[0].isValid();
	if (doesDialogueHaveOptions)
	{
		static RectCollider mouseCollider{ {0,0}, {1, 1} };
		bool isHoverOption = false;

		for (uint8_t i = 0; i < k_maxDialogueOptions; ++i)
		{
			DialogueOption& dialogueOption = _dialogueOptions[i];

			// Dynamic colors + opacity for dialogue options
			switch (dialogueOption.state)
			{
			case DIALOGUE_OPTION_IDLE_STATE:
				switch (dialogueOption.optionTensionType)
				{
				case LOW_TENSION:
				case NORMAL_TENSION:
				case HIGH_TENSION:
					dialogueOption.backgroundSpriteColor = { 23, 9, 31 };
					break;
				case FATAL_TENSION:
					dialogueOption.backgroundSpriteColor = { 0, 0, 0 };
					break;
				default:
					D_ASSERT(false, "Unsupported tension type");
					break;
				}
				dialogueOption.opacity = 255.f;
				// No need to set the hovered border color since it's not visible on idle
				break;
			case DIALOGUE_OPTION_HOVERED_STATE:
				dialogueOption.backgroundSpriteColor.r = lerp(dialogueOption.backgroundSpriteColor.r, 81, 0.1f);
				dialogueOption.backgroundSpriteColor.g = lerp(dialogueOption.backgroundSpriteColor.g, 34, 0.1f);
				dialogueOption.backgroundSpriteColor.b = lerp(dialogueOption.backgroundSpriteColor.b, 43, 0.1f);

				dialogueOption.hoveredBorderSpriteColor.r = lerp(dialogueOption.hoveredBorderSpriteColor.r, 209, 0.05f);
				dialogueOption.hoveredBorderSpriteColor.g = lerp(dialogueOption.hoveredBorderSpriteColor.g, 209, 0.05f);
				dialogueOption.hoveredBorderSpriteColor.b = lerp(dialogueOption.hoveredBorderSpriteColor.b, 209, 0.05f);
				break;
			case DIALOGUE_OPTION_CHOSEN_STATE:
				dialogueOption.backgroundSpriteColor.r = lerp(dialogueOption.backgroundSpriteColor.r, 81, 0.02f);
				dialogueOption.backgroundSpriteColor.g = lerp(dialogueOption.backgroundSpriteColor.g, 34, 0.02f);
				dialogueOption.backgroundSpriteColor.b = lerp(dialogueOption.backgroundSpriteColor.b, 43, 0.02f);

				dialogueOption.hoveredBorderSpriteColor.r = lerp(dialogueOption.hoveredBorderSpriteColor.r, 209, 0.05f);
				dialogueOption.hoveredBorderSpriteColor.g = lerp(dialogueOption.hoveredBorderSpriteColor.g, 209, 0.05f);
				dialogueOption.hoveredBorderSpriteColor.b = lerp(dialogueOption.hoveredBorderSpriteColor.b, 209, 0.05f);

				dialogueOption.fadeOutTimer += k_deltaTime;
				if (dialogueOption.fadeOutTimer >= 1.f)
				{
					dialogueOption.opacity = max(dialogueOption.opacity - (1500.f * k_deltaTime), 0.f);
				}
				else
				{
					dialogueOption.opacity = 255.f;
				}
				break;
			case DIALOGUE_OPTION_NOT_CHOSEN_STATE:
				dialogueOption.backgroundSpriteColor = { 23, 9, 31 };
				dialogueOption.opacity = max(dialogueOption.opacity - (1500.f * k_deltaTime), 0.f);
				// No need to set the hovered border color since it's not visible on idle
				break;
			default:
				D_ASSERT(false, "Unsupported tension type");
				break;
			}

			// Dynamic dialogue box y size
			{
				float targetDialogueBoxYSize = dialogueOption.dialogueBoxSize.y + (dialogueOptionsOuterPadding.y * 2.f);
				if (_currentDialogue.timeSinceDialogueStarted >= dialogueOption.secondsToStartShowingOption)
				{
					dialogueOption.dialogueBoxDynamicYSize = lerp(dialogueOption.dialogueBoxDynamicYSize, targetDialogueBoxYSize, 0.1f);
				}
				else
				{
					dialogueOption.dialogueBoxDynamicYSize = 0.f;
				}
			}

			// Dialogue options characters
			for (uint16_t i = 0; i < k_maxCharactersPerDialogue; ++i)
			{
				DialogueCharacter& c = dialogueOption.characters[i];

				// Check if it causes issues. The thought process is that if we find a not valid character, we stop printing
				// Since it means we reached the end of the dialogue
				if (!c.isValid())
				{
					break;
				}

				if (_currentDialogue.timeSinceDialogueStarted >= dialogueOption.secondsToStartShowingOption)
				{
					c.dynamicYSize = lerp(c.dynamicYSize, c.size.y, 0.25f);
				}
				else
				{
					c.dynamicYSize = 0.f;
				}
			}

			Vec2 dialogueOptionPosition{ dialogueOption.colliderDest.x , dialogueOption.colliderDest.y };
			RectCollider dialogueOptionCollider{ {0,0}, {(int32_t)dialogueOption.colliderDest.w, (int32_t)dialogueOption.colliderDest.h} };

			bool canReactToHoverFeedback = (dialogueOption.state == DIALOGUE_OPTION_IDLE_STATE) || (dialogueOption.state == DIALOGUE_OPTION_HOVERED_STATE);
			bool isMouseHoverOption = aabb(s_mousePositionThisFrameInScreenSpace, dialogueOptionPosition, mouseCollider, dialogueOptionCollider);

			// End current dialogue and destroy dialogue options after choosing an option and the fade out of the chosen option is complete
			bool canRequestDialogueToEnd = (dialogueOption.state == DIALOGUE_OPTION_CHOSEN_STATE) && dialogueOption.opacity <= 50 && (_currentDialogue.state == DIALOGUE_BASE_STATE);
			if (canRequestDialogueToEnd)
			{
				_currentDialogue.state = DIALOGUE_ENDED_STATE;
				_currentDialogue.dialogueOptionChosen = dialogueOption.dialogueType;
				CrosshairSystem::s_corsshairOpacity = 255;
				for (DialogueOption& option : _dialogueOptions) { option.destroyDialogueOption(); }
				return;
			}

			if (isMouseHoverOption)
			{
				if (canReactToHoverFeedback)
				{
					dialogueOption.state = DIALOGUE_OPTION_HOVERED_STATE;
					isHoverOption = true;
				}

				if (dialogueOption.state == DIALOGUE_OPTION_CHOSEN_STATE)
				{
					isHoverOption = true;
				}

				// Choose dialogue option
				if (wasChooseDialogueOptionKeyPressedThisFrame() && dialogueOption.state != DIALOGUE_OPTION_CHOSEN_STATE)
				{
					dialogueOption.state = DIALOGUE_OPTION_CHOSEN_STATE;
					dialogueOption.backgroundSpriteColor = { 108, 26, 86, 255 };
					dialogueOption.hoveredBorderSpriteColor = { 108, 26, 86, 255 };
					startTimer(dialogueOption.fadeOutTimer);

					// Apply camera shake based on the type of option
					switch (dialogueOption.optionTensionType)
					{
					case HIGH_TENSION:
						LevelManager::getCurrentLevel()->_levelCamera.doShake(LIGHT_SHAKE, 0.f);
						break;
					case FATAL_TENSION:
						LevelManager::getCurrentLevel()->_levelCamera.doShake(MEDIUM_SHAKE, 0.f);
						break;
					default:
						break;
					}

					s_playerTension += dialogueOption.tensionDelta;

					// Change all the other options to be NOT_CHOSEN
					for (DialogueOption& option : _dialogueOptions) { if (option.state == dialogueOption.state) continue; option.state = DIALOGUE_OPTION_NOT_CHOSEN_STATE; }
				}
			}
			else if(canReactToHoverFeedback)
			{
				dialogueOption.state = DIALOGUE_OPTION_IDLE_STATE;
			}
		}

		CrosshairSystem::s_corsshairOpacity = isHoverOption ? 50 : 255;
	}

	// Dynamic dialogue box X size
	{
		float dialogueBoxTargetXSize = _currentDialogue.dialogueBoxSize.x + (k_dialogueOuterPadding.x * 2.f);
		float currentTargetXSize = (_currentDialogue.state == DIALOGUE_ENDED_STATE) ? 0.f : dialogueBoxTargetXSize;
		_currentDialogue.dialogueBoxDynamicXSize = lerp(_currentDialogue.dialogueBoxDynamicXSize, currentTargetXSize, 6.25 * k_deltaTime);
	}

	// Dynamic speech indicator y size
	{
		if (_currentDialogue.state == DIALOGUE_ENDED_STATE)
		{
			_currentDialogue.speechIndicatorDynamicYSize = lerp(_currentDialogue.speechIndicatorDynamicYSize, 0.f, 0.2f);
		}
		else
		{
			_currentDialogue.speechIndicatorDynamicYSize = lerp(_currentDialogue.speechIndicatorDynamicYSize, k_speechIndicatorSize.y, 0.1f);
		}
	}

	// Move speech indicator x
	bool shouldLerpSpeechIndicatorXPosition = (_currentDialogue.speechIndicatorTargetXPosition > 0.1f && _currentDialogue.speechIndicatorCurrentXPosition > 0.1f);
	if (shouldLerpSpeechIndicatorXPosition)
	{
		_currentDialogue.speechIndicatorCurrentXPosition = lerp(_currentDialogue.speechIndicatorCurrentXPosition, _currentDialogue.speechIndicatorTargetXPosition, 0.1f);
	}

	// Main dialogue characters logic + animate
	bool hasDialogueFinished = false;
	bool hasFinishedInterrupting = (_currentDialogue.state == DIALOGUE_INTERRUPTED_STATE) ? true : false;
	uint16_t numberOfCharactersOnCurrentDialogue = 0;
	for (uint16_t i = 0; i < k_maxCharactersPerDialogue; ++i)
	{
		DialogueCharacter& c = _currentDialogue.characters[i];

		// Check if it causes issues. The thought process is that if we find a not valid character, we stop printing
		// Since it means we reached the end of the dialogue
		if (!c.isValid())
		{
			int lastValidCharacterIndex = max(i - 1, 0);
			DialogueCharacter& lastValidCharacter = _currentDialogue.characters[lastValidCharacterIndex];
			hasDialogueFinished = lastValidCharacter.opacity > 200;
			numberOfCharactersOnCurrentDialogue = lastValidCharacterIndex + 1;
			break;
		}

		// Fade in each character
		bool canCharacterFadeIn = (_currentDialogue.timeSinceDialogueStarted >= c.secondsToStartShowingCharacter);
		if (canCharacterFadeIn)
		{
			float k_secondsToFadeInEachCharacter = 0.3f;
			float speed = 255.f / k_secondsToFadeInEachCharacter;
			c.opacity = min(c.opacity + (speed * k_deltaTime), 255.f);
			c.timeSinceCharacterAppeared += k_deltaTime;
		}
		else
		{
			c.opacity = 0.f;
		}

		// Run code only once per character
		if (canCharacterFadeIn && !c.wasInitialized)
		{
			// Reset camera shake flag. If it's a space character it means we're going for a new word, and therefore we can apply a new camera shake if wanted
			bool isSpaceCharacter = (c.atlasOffset.x == k_firstCharacterOnAtlasOffset.x) && (c.atlasOffset.y == k_firstCharacterOnAtlasOffset.y);
			if (isSpaceCharacter)
			{
				_currentDialogue.hasAppliedShakeForCurrentWord = false;
			}

			// Camera shake text effect
			bool isShakeEffect = (c.textEffectToApply == RED_SHAKE_EFFECT || c.textEffectToApply == INTERJECTION_EFFECT);
			if (!_currentDialogue.hasAppliedShakeForCurrentWord && isShakeEffect)
			{
				CameraShakeType shakeToPerform = (c.textEffectToApply == RED_SHAKE_EFFECT ? MEDIUM_SHAKE : LIGHT_SHAKE);
				LevelManager::getCurrentLevel()->_levelCamera.doShake(shakeToPerform, 0.05f);
				_currentDialogue.hasAppliedShakeForCurrentWord = true;
			}

			c.wasInitialized = true;
		}

		// Wave movement effect
		bool isWaveEffect = (c.textEffectToApply == WAVE_EFFECT) || (c.textEffectToApply == PINK_WAVE_EFFECT);
		if (isWaveEffect && canCharacterFadeIn)
		{
			// offset * sin(time * speed)
			float sinMovementOffset = 1.f * sin(c.timeSinceCharacterAppeared * 5.f);
			c.position.y = c.startingPosition.y + sinMovementOffset;
		}

		// Fade in animations
		bool canAnimateCharacterDuringFadeIn = !isWaveEffect;
		if (_currentDialogue.state == DIALOGUE_BASE_STATE)
		{
			if (canAnimateCharacterDuringFadeIn)
			{
				if (canCharacterFadeIn)
				{
					c.position.x = lerp(c.position.x, c.startingPosition.x, 0.2f);
					c.size.x = lerp(c.size.x, k_characterSize.x, 0.4f);
					c.size.y = lerp(c.size.y, k_characterSize.y, 0.4f);
				}
			}
			else
			{
				// If the current text effect will change the position, snap directly without fade in animation
				c.position.x = c.startingPosition.x;
				c.size.x = k_characterSize.x;
				c.size.y = k_characterSize.y;
			}
		}

		// Physics for dialogue interrupted smash
		if (_currentDialogue.state == DIALOGUE_INTERRUPTED_STATE)
		{
			c.velocity.x *= 0.96f;
			c.velocity.y += 12.f * k_deltaTime;

			c.position.x += c.velocity.x;
			c.position.y += c.velocity.y;

			c.rotationAngle += c.angularVelocity * k_deltaTime;

			bool isOffscreen = c.position.y > 180.f;
			if (!isOffscreen)
			{
				hasFinishedInterrupting = false;
			}
		}
	}

	_currentDialogue.timeSinceDialogueStarted += k_deltaTime;
	if (hasDialogueFinished)
	{
		// Incrase player tension once when dialogue ends
		if (_currentDialogue.timeSinceFinalCharacterWasDrawn < 0.001f)
		{
			s_playerTension += _currentDialogue.tensionDelta;
		}

		_currentDialogue.timeSinceFinalCharacterWasDrawn += k_deltaTime;
	}


	// Destroy dialogue if has finished interrupting
	if (hasFinishedInterrupting)
	{
		_currentDialogue.state = DIALOGUE_FINISHED_INTERRUPTED;
	}

	// Animate dialogue outline (auto skip)
	if (!doesDialogueHaveOptions &&_currentDialogue.state == DIALOGUE_BASE_STATE)
	{
		float secondsToSkipDialogue = 3.f;
		if (numberOfCharactersOnCurrentDialogue < 10)
		{
			secondsToSkipDialogue = 2.f;
		}

		if (numberOfCharactersOnCurrentDialogue > 70)
		{
			secondsToSkipDialogue = 4.f;
		}

		// Apply the no wait effect - make time to skip to next dialogue faster
		bool isApplyingNoWaitEffect = _currentDialogue.characters[0].textEffectToApply == NO_WAIT_EFFECT;
		bool canSkipFromNoWaitDialogue = isApplyingNoWaitEffect && (_currentDialogue.timeSinceFinalCharacterWasDrawn > 1.5f);

		if (_currentDialogue.timeSinceFinalCharacterWasDrawn > 0.5f && !isApplyingNoWaitEffect)
		{
			float amountToShrinkPerSecond = _currentDialogue.dialogueBoxDynamicXSize / secondsToSkipDialogue;
			float amountToShrinkPerFrame = amountToShrinkPerSecond / k_targetFrameRateForGameLogic;
			_currentDialogue.dialogueOutlineDynamicXSize = max(_currentDialogue.dialogueOutlineDynamicXSize - amountToShrinkPerFrame, 0.f);
		}
		else
		{
			_currentDialogue.dialogueOutlineDynamicXSize = _currentDialogue.dialogueBoxDynamicXSize;
		}

		if (_currentDialogue.dialogueOutlineDynamicXSize <= 0.f || canSkipFromNoWaitDialogue)
		{
			_currentDialogue.state = DIALOGUE_ENDED_STATE;
		}
	}

	if (doesDialogueHaveOptions)
	{
		_currentDialogue.dialogueOutlineDynamicXSize = _currentDialogue.dialogueBoxDynamicXSize;
	}

	// Tension bar logic
	{
		s_playerTension = max(s_playerTension, 0);
		_currentTensionSpriteXSize = lerp(_currentTensionSpriteXSize, s_playerTension, 0.2f);
	}

#ifndef RELEASE_BUILD

	// Reconstruct current dialogue
	if (_wasKeyPressedThisFrame(SDL_SCANCODE_J))
	{
		DialogueOptionsDTO currentDialogueOptions;
		for (uint8_t i = 0; i < k_maxDialogueOptions; ++i)
		{
			if (_dialogueOptions[i].isValid())
			{
				currentDialogueOptions.options[i] = _dialogueOptions[i].dialogueType;
			}
		}

		pushCellphoneDialogue(_currentDialogue.dialogueType, currentDialogueOptions);
	}

#endif // !RELEASE_BUILD
}

void UISystem::skipDialogue()
{
	// Pretend the dialogue already started a long time ago to allow opacity override
	_currentDialogue.timeSinceDialogueStarted = 50.f;

	for (DialogueCharacter& c : _currentDialogue.characters)
	{
		if (!c.isValid())
		{
			break;
		}

		c.opacity = 255.f;
	}
}

void UISystem::interruptCurrentDialogue()
{
	_currentDialogue.dialogueBoxDynamicXSize = 0.f;
	_currentDialogue.state = DIALOGUE_INTERRUPTED_STATE;

	Vec2 dialogueCenterPosition = { _currentDialogue.topLeftPosition.x + (_currentDialogue.dialogueBoxSize.x * 0.5f),
									_currentDialogue.topLeftPosition.y + (_currentDialogue.dialogueBoxSize.y * 0.5f) };


	LevelManager::getCurrentLevel()->_levelCamera.doShake(MEDIUM_SHAKE, 0.f);

	// Launch every character away from center
	for (uint16_t i = 0; i < k_maxCharactersPerDialogue; ++i)
	{
		DialogueCharacter& c = _currentDialogue.characters[i];

		if (!c.isValid())
		{
			break;
		}
		
		Vec2 direction = { c.position.x - dialogueCenterPosition.x, c.position.y - dialogueCenterPosition.y };

		// Normalize
		float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
		direction.x /= length;
		direction.y /= length;

		// Small random variation so it doesn't look perfectly radial
		direction.x += (SDL_randf() * 0.4f) - 0.2f;
		direction.y += (SDL_randf() * 0.4f) - 0.2f;

		c.velocity.x = direction.x * 4.f;
		c.velocity.y = direction.y * 4.f;

		// Give everything a slight upward kick
		c.velocity.y -= 1.f;

		// Random spin
		c.angularVelocity = (SDL_randf() * 360.f);
	}
}

void UISystem::destroyCurrentDialogue()
{
	_lastDialogueType = _currentDialogue.dialogueType;
	_lastOptionChosen = _currentDialogue.dialogueOptionChosen;
	_currentDialogue.destroyDialoge();
}

void UISystem::render(RenderingSystem* renderingSystem)
{
	static SDL_FRect src;
	static SDL_FRect dest;

	// Tension bar
	float tensionBarXSize = 90.f;
	if(_isTensionBarVisible)
	{
		// Reuse the same texture since it's a white 1x1 pixel
		DialogueBoxSprite& baseSprite = k_dialogueSpeechSprites[DIALOGUE_BASE_SPRITE];

		src.x = baseSprite.atlasOffset.x;
		src.y = baseSprite.atlasOffset.y;
		src.w = baseSprite.spriteSize.x;
		src.h = baseSprite.spriteSize.y;

		dest.x = (k_baseGameWidth * 0.5f) - (tensionBarXSize * 0.5f);
		dest.y = 144.f;
		dest.w = tensionBarXSize;
		dest.h = 1.85f;

		SDL_Texture* atlas = renderingSystem->loadAtlas(baseSprite.atlasType);
		SDL_SetTextureColorMod(atlas, 98, 85, 101);
		SDL_SetTextureAlphaMod(atlas, 255);

		SDL_RenderTexture(s_renderer, atlas, &src, &dest);

		// Fade out left and right
		{
			DialogueBoxSprite& fadeSprite = k_dialogueSpeechSprites[DIALOGUE_TENSION_FADE_SPRITE];

			src.x = fadeSprite.atlasOffset.x;
			src.y = fadeSprite.atlasOffset.y;
			src.w = fadeSprite.spriteSize.x;
			src.h = fadeSprite.spriteSize.y;

			dest.x = dest.x - (k_tensionFadeSize.x);
			dest.w = k_tensionFadeSize.x;

			SDL_Texture* atlas = renderingSystem->loadAtlas(fadeSprite.atlasType);
			SDL_SetTextureColorMod(atlas, 98, 85, 101);
			SDL_SetTextureAlphaMod(atlas, 255);

			SDL_RenderTexture(s_renderer, atlas, &src, &dest);

			dest.x = dest.x + (k_tensionFadeSize.x) + tensionBarXSize;
			SDL_RenderTextureRotated(s_renderer, atlas, &src, &dest, 0, nullptr, SDL_FLIP_HORIZONTAL);
		}
	}

	// Draw tension inside bar
	if (_isTensionBarVisible)
	{
		// Reuse the same texture since it's a white 1x1 pixel
		DialogueBoxSprite& baseSprite = k_dialogueSpeechSprites[DIALOGUE_BASE_SPRITE];
		src.x = baseSprite.atlasOffset.x;
		src.y = baseSprite.atlasOffset.y;
		src.w = baseSprite.spriteSize.x;
		src.h = baseSprite.spriteSize.y;

		dest.x = (k_baseGameWidth * 0.5f) - (_currentTensionSpriteXSize * 0.5f);
		dest.w = _currentTensionSpriteXSize;

		SDL_Texture* atlas = renderingSystem->loadAtlas(baseSprite.atlasType);
		SDL_SetTextureColorMod(atlas, 255, 0, 0);

		// current * maxOpacity / maxTensionSize. Make sure doesn't go below min allowed opacity
		float minOpacity = 150.f;
		float opacity = min(minOpacity + _currentTensionSpriteXSize, 255.f);
		SDL_SetTextureAlphaMod(atlas, opacity);

		SDL_RenderTexture(s_renderer, atlas, &src, &dest);
	}

	// Prevent executing any code if there's nothing to print
	if (!_currentDialogue.characters[0].isValid())
	{
		return;
	}

	// Dialogue base
	{
		DialogueBoxSprite& speechBubbleSprite = k_dialogueSpeechSprites[DIALOGUE_BASE_SPRITE];

		src.x = speechBubbleSprite.atlasOffset.x;
		src.y = speechBubbleSprite.atlasOffset.y;
		src.w = speechBubbleSprite.spriteSize.x;
		src.h = speechBubbleSprite.spriteSize.y;

		dest.x = _currentDialogue.topLeftPosition.x - k_dialogueOuterPadding.x;
		dest.y = _currentDialogue.topLeftPosition.y - k_dialogueOuterPadding.y;
		dest.w = _currentDialogue.dialogueBoxDynamicXSize;
		dest.h = _currentDialogue.dialogueBoxSize.y + (k_dialogueOuterPadding.y * 2.f);

		if (!_currentDialogue.isScreenSpace)
		{
			dest = convertWorldRectToCameraSpace(dest);
		}

		SDL_Texture* atlas = renderingSystem->loadAtlas(speechBubbleSprite.atlasType);
		SDL_SetTextureColorMod(atlas, s_currentDialogueEntityDTO.dialogueBoxColor.r, s_currentDialogueEntityDTO.dialogueBoxColor.g, s_currentDialogueEntityDTO.dialogueBoxColor.b);

		uint8_t opacity = 255;
		if (_currentDialogue.state == DIALOGUE_INTERRUPTED_STATE || _currentDialogue.state == DIALOGUE_FINISHED_INTERRUPTED)
		{
			opacity = 0;
		}
		else if (_currentDialogue.state == DIALOGUE_ENDED_STATE)
		{
			const float dialogueBoxSizeXToOpacityRatio = 255.f / _currentDialogue.dialogueBoxSize.x;
			float test = _currentDialogue.dialogueBoxDynamicXSize * dialogueBoxSizeXToOpacityRatio;
			opacity = min(test, 255.f);
		}

		SDL_SetTextureAlphaMod(atlas, opacity);
		SDL_RenderTexture(s_renderer, atlas, &src, &dest);
	}

	// Dialogue outline
	{
		// Reuse the same texture since it's a white 1x1 pixel
		DialogueBoxSprite& speechBubbleSprite = k_dialogueSpeechSprites[DIALOGUE_BASE_SPRITE];

		src.x = speechBubbleSprite.atlasOffset.x;
		src.y = speechBubbleSprite.atlasOffset.y;
		src.w = speechBubbleSprite.spriteSize.x;
		src.h = speechBubbleSprite.spriteSize.y;

		dest.x = _currentDialogue.topLeftPosition.x - k_dialogueOuterPadding.x;
		dest.y = _currentDialogue.topLeftPosition.y + _currentDialogue.dialogueBoxSize.y + k_dialogueOuterPadding.y;
		dest.w = _currentDialogue.dialogueOutlineDynamicXSize;
		dest.h = k_dialogueOutlineHeight;

		if (!_currentDialogue.isScreenSpace)
		{
			dest = convertWorldRectToCameraSpace(dest);
		}

		SDL_Texture* atlas = renderingSystem->loadAtlas(speechBubbleSprite.atlasType);
		SDL_SetTextureColorMod(atlas, s_currentDialogueEntityDTO.outlineColor.r, s_currentDialogueEntityDTO.outlineColor.g, s_currentDialogueEntityDTO.outlineColor.b);
		SDL_RenderTexture(s_renderer, atlas, &src, &dest);
	}

	// Speech indicator
	{
		DialogueBoxSprite& speechIndicatorSprite = k_dialogueSpeechSprites[DIALOGUE_INDICATOR_SPRITE];

		src.x = speechIndicatorSprite.atlasOffset.x;
		src.y = speechIndicatorSprite.atlasOffset.y;
		src.w = speechIndicatorSprite.spriteSize.x;
		src.h = speechIndicatorSprite.spriteSize.y;

		// Since the last thing drawn was the dialogue outline, use dest directly
		float xPositionToDrawSpeechIndicator = 0.f;
		switch (_currentDialogue.alignmentType)
		{
			case DIALOGUE_CENTER_ALIGNED:
			{
				float dialogueBoxTargetXSize = _currentDialogue.dialogueBoxSize.x + (k_dialogueOuterPadding.x * 2.f);
				float textCenterXPos = dest.x + (dialogueBoxTargetXSize / 2);
				xPositionToDrawSpeechIndicator = textCenterXPos;
				break;
			}
			case DIALOGUE_LEFT_ALIGNED:
			{
				float textLeftAlignedXPos = dest.x + 3;
				xPositionToDrawSpeechIndicator = textLeftAlignedXPos;
				break;
			}
			default:
				D_ASSERT(false, "Alignment type not supported");
				break;
		}

		// Initialize for the first time
		if (_currentDialogue.speechIndicatorCurrentXPosition < 0.1f)
		{
			_currentDialogue.speechIndicatorCurrentXPosition = xPositionToDrawSpeechIndicator;
		}

		// Calculate X Position for speech indicator.
		// If needed, move move it (only neeed when the entity moved during the dialogue)
		// On update, we take the current and target and just lerp.
		{
			Vec2 entityPosition = getComponentFromEntity<TransformComponent>(*_currentDialogue.entityTalking)->position;
			SDL_FRect positionWhereDialogueStartsDrawing;
			positionWhereDialogueStartsDrawing.x = entityPosition.x + s_currentDialogueEntityDTO.dialoguePositionOffset.x;
			positionWhereDialogueStartsDrawing.y = entityPosition.y + s_currentDialogueEntityDTO.dialoguePositionOffset.y;
			positionWhereDialogueStartsDrawing.w = positionWhereDialogueStartsDrawing.h = 1.f;
			SDL_FRect screenPositionWhereDialogueStartsDrawing = convertWorldRectToCameraSpace(positionWhereDialogueStartsDrawing);

			if (screenPositionWhereDialogueStartsDrawing.x > xPositionToDrawSpeechIndicator)
			{
				// Only allow movement until 90% of the dialogue box
				float dialogueBoxTargetXSize = _currentDialogue.dialogueBoxSize.x + (k_dialogueOuterPadding.x * 2.f);
				float dialogueBoxEndPosition = dest.x + dialogueBoxTargetXSize;
				float maxAllowedPositionForSpeechIndicator = dialogueBoxEndPosition * 0.9f;

				_currentDialogue.speechIndicatorTargetXPosition = min(screenPositionWhereDialogueStartsDrawing.x, maxAllowedPositionForSpeechIndicator);
				dest.x = _currentDialogue.speechIndicatorCurrentXPosition;

			}
			else
			{
				dest.x = xPositionToDrawSpeechIndicator;
			}
		}

		float dialogueOutlineEndYPosition = dest.y + dest.h;
		dest.y = dialogueOutlineEndYPosition - k_dialogueOutlineHeight;
		dest.w = k_speechIndicatorSize.x;
		dest.h = _currentDialogue.speechIndicatorDynamicYSize;

		// Since the speech indicator has its X and Y pos defined by the previous value, we shouldn't convert to camera space
		// as it's already in camera space
		//dest = convertWorldRectToCameraSpace(dest);

		SDL_Texture* atlas = renderingSystem->loadAtlas(speechIndicatorSprite.atlasType);
		SDL_SetTextureColorMod(atlas, s_currentDialogueEntityDTO.dialogueBoxColor.r, s_currentDialogueEntityDTO.dialogueBoxColor.g, s_currentDialogueEntityDTO.dialogueBoxColor.b);

		float opacity = 255.f;
		if (_currentDialogue.state == DIALOGUE_ENDED_STATE)
		{
			const float k_speechIndicatorSizeToOpacityRatio = 255.f / k_speechIndicatorSize.y;
			opacity = _currentDialogue.speechIndicatorDynamicYSize * k_speechIndicatorSizeToOpacityRatio;
		}
		else if (_currentDialogue.state == DIALOGUE_INTERRUPTED_STATE || _currentDialogue.state == DIALOGUE_FINISHED_INTERRUPTED)
		{
			opacity = 0.f;
		}

		SDL_SetTextureAlphaMod(atlas, (int8_t)opacity);
		SDL_RenderTextureRotated(s_renderer, atlas, &src, &dest, 0, nullptr, SDL_FLIP_HORIZONTAL);
	}

	// Speech indicator outline
	{
		DialogueBoxSprite& speechIndicatorOutlineSprite = k_dialogueSpeechSprites[DIALOGUE_INDICATOR_OUTLINE_SPRITE];

		src.x = speechIndicatorOutlineSprite.atlasOffset.x;
		src.y = speechIndicatorOutlineSprite.atlasOffset.y;
		src.w = speechIndicatorOutlineSprite.spriteSize.x;
		src.h = speechIndicatorOutlineSprite.spriteSize.y;

		// Since the last thing drawn was the speech indicator, use dest directly
		// Share everything since this sprite has the same size

		SDL_Texture* atlas = renderingSystem->loadAtlas(speechIndicatorOutlineSprite.atlasType);
		SDL_SetTextureColorMod(atlas, s_currentDialogueEntityDTO.outlineColor.r, s_currentDialogueEntityDTO.outlineColor.g, s_currentDialogueEntityDTO.outlineColor.b);
		SDL_RenderTextureRotated(s_renderer, atlas, &src, &dest, 0, nullptr, SDL_FLIP_HORIZONTAL);
	}

	// Main dialogue draw each character
	for (uint16_t i = 0; i < k_maxCharactersPerDialogue; ++i)
	{
		DialogueCharacter& c = _currentDialogue.characters[i];

		// Check if it causes issues. The thought process is that if we find a not valid character, we stop printing
		// Since it means we reached the end of the dialogue
		if (!c.isValid())
		{
			break;
		}

		src.x = c.atlasOffset.x;
		src.y = c.atlasOffset.y;
		src.w = k_characterSizeOnAtlas.x;
		src.h = k_characterSizeOnAtlas.y;

		dest.x = c.position.x;
		dest.y = c.position.y;
		dest.w = c.size.x;
		dest.h = c.size.y;

		if (!_currentDialogue.isScreenSpace)
		{
			dest = convertWorldRectToCameraSpace(dest);
		}

		SDL_Texture* fontAtlas = renderingSystem->loadAtlas(FONT_ATLAS);

		if (_currentDialogue.timeSinceDialogueStarted < c.secondsToStartShowingCharacter)
		{
			c.opacity = 0.f;
		}
		else
		{
			// Handled on update since it needs to lerp
		}

		if ((_currentDialogue.state == DIALOGUE_ENDED_STATE) && _currentDialogue.dialogueBoxDynamicXSize <= c.position.x + c.size.x + c.size.x - _currentDialogue.topLeftPosition.x)
		{
			c.opacity = 0.f;
		}

		if (isColorValid(c.overrideColor))
		{
			SDL_SetTextureColorMod(fontAtlas, c.overrideColor.r, c.overrideColor.g, c.overrideColor.b);
		}
		else
		{
			SDL_SetTextureColorMod(fontAtlas, s_currentDialogueEntityDTO.textColor.r, s_currentDialogueEntityDTO.textColor.g, s_currentDialogueEntityDTO.textColor.b);
		}

		SDL_SetTextureAlphaMod(fontAtlas, c.opacity);
		SDL_RenderTextureRotated(s_renderer, fontAtlas, &src, &dest, c.rotationAngle, nullptr, SDL_FLIP_NONE);
	}

	// Draw dialogue ended indicator
	{
		DialogueBoxSprite& dialogueFinishedSprite = k_dialogueSpeechSprites[DIALOGUE_FINISHED_INDICATOR_SPRITE];
		src.x = dialogueFinishedSprite.atlasOffset.x;
		src.y = dialogueFinishedSprite.atlasOffset.y;
		src.w = dialogueFinishedSprite.spriteSize.x;
		src.h = dialogueFinishedSprite.spriteSize.y;

		dest.x = _currentDialogue.topLeftPosition.x + _currentDialogue.dialogueBoxSize.x - 1;
		dest.y = _currentDialogue.topLeftPosition.y + _currentDialogue.dialogueBoxSize.y;
		dest.w = k_dialogueEndedIndicatorSize.x;
		dest.h = k_dialogueEndedIndicatorSize.y;

		if (!_currentDialogue.isScreenSpace)
		{
			dest = convertWorldRectToCameraSpace(dest);
		}

		SDL_Texture* fontAtlas = renderingSystem->loadAtlas(FONT_ATLAS);
		SDL_SetTextureColorMod(fontAtlas, s_currentDialogueEntityDTO.textColor.r, s_currentDialogueEntityDTO.textColor.g, s_currentDialogueEntityDTO.textColor.b);

		if (_currentDialogue.state == DIALOGUE_ENDED_STATE || _currentDialogue.state == DIALOGUE_INTERRUPTED_STATE || _currentDialogue.state == DIALOGUE_FINISHED_INTERRUPTED)
		{
			SDL_SetTextureAlphaMod(fontAtlas, 0);
		}
		// Don't do anything for else, since this is on the same texture as the font characters, we don't need to override the opacity
		// This will make it only visible when the last character is also visible

		SDL_RenderTexture(s_renderer, fontAtlas, &src, &dest);
	}

	// Dialogue Options
	bool hasAtleastOneDialogueOption = _dialogueOptions[0].isValid();
	if (!hasAtleastOneDialogueOption)
	{
		return;
	}

	for (DialogueOption& dialogueOption : _dialogueOptions)
	{
		if (!dialogueOption.isValid())
		{
			continue;
		}

		// Take the position of the first character since it's where we start
		Vec2 topLeftDialogueOptionPosition = dialogueOption.characters[0].position;

		// Dialogue option base sprite
		{
			// Reuse the same texture since it's a white 1x1 pixel
			DialogueBoxSprite& speechBubbleSprite = k_dialogueSpeechSprites[DIALOGUE_BASE_SPRITE];

			src.x = speechBubbleSprite.atlasOffset.x;
			src.y = speechBubbleSprite.atlasOffset.y;
			src.w = speechBubbleSprite.spriteSize.x;
			src.h = speechBubbleSprite.spriteSize.y;

			dest.x = topLeftDialogueOptionPosition.x - dialogueOptionsOuterPadding.x;
			dest.y = topLeftDialogueOptionPosition.y - dialogueOptionsOuterPadding.y;
			dest.w = dialogueOption.dialogueBoxSize.x + (dialogueOptionsOuterPadding.x * 2.f);
			dest.h = dialogueOption.dialogueBoxDynamicYSize;
			dialogueOption.colliderDest = dest;

			SDL_Texture* atlas = renderingSystem->loadAtlas(speechBubbleSprite.atlasType);
			SDL_SetTextureColorMod(atlas, dialogueOption.backgroundSpriteColor.r, dialogueOption.backgroundSpriteColor.g, dialogueOption.backgroundSpriteColor.b);

			SDL_SetTextureAlphaMod(atlas, dialogueOption.opacity);
			SDL_RenderTexture(s_renderer, atlas, &src, &dest);
		}

		// Dialogue option border (letf + right)
		{
			DialogueBoxSprite& borderSprite = k_dialogueSpeechSprites[DIALOGUE_OPTION_BORDER_SPRITE];

			src.x = borderSprite.atlasOffset.x;
			src.y = borderSprite.atlasOffset.y;
			src.w = borderSprite.spriteSize.x;
			src.h = borderSprite.spriteSize.y;

			// First draw the right side, to reuse dest (since last thing drawn was the dialogue box)
			dest.x = dest.x + dest.w;
			dest.y = dest.y;
			dest.w = k_dialogueOptionBorderSize.x;
			// Don't override dest.y since we can share the one used for option's dialogue box

			// Adjust collider to match the dialogue box considering the borders
			dialogueOption.colliderDest.x -= k_dialogueOptionBorderSize.x;
			dialogueOption.colliderDest.w += k_dialogueOptionBorderSize.x * 2.f;

			SDL_Texture* atlas = renderingSystem->loadAtlas(borderSprite.atlasType);
			SDL_SetTextureColorMod(atlas, dialogueOption.backgroundSpriteColor.r, dialogueOption.backgroundSpriteColor.g, dialogueOption.backgroundSpriteColor.b);
			SDL_SetTextureAlphaMod(atlas, dialogueOption.opacity);

			SDL_RenderTextureRotated(s_renderer, atlas, &src, &dest, 0, nullptr, SDL_FLIP_HORIZONTAL);

			// Hovered border for right side
			if (dialogueOption.state == DIALOGUE_OPTION_HOVERED_STATE ||
				dialogueOption.state == DIALOGUE_OPTION_CHOSEN_STATE)
			{
				DialogueBoxSprite& hoveredBorderSprite = k_dialogueSpeechSprites[DIALOGUE_OPTION_HOVERED_BORDER_SPRITE];

				src.x = hoveredBorderSprite.atlasOffset.x;
				src.y = hoveredBorderSprite.atlasOffset.y;
				src.w = hoveredBorderSprite.spriteSize.x;
				src.h = hoveredBorderSprite.spriteSize.y;

				dest.w = k_dialogueHoveredBorderSize.x;
				// Don't override dest.y since we can share the one used for option's dialogue box

				SDL_Texture* atlas = renderingSystem->loadAtlas(hoveredBorderSprite.atlasType);

				SDL_SetTextureColorMod(atlas, dialogueOption.hoveredBorderSpriteColor.r, dialogueOption.hoveredBorderSpriteColor.g, dialogueOption.hoveredBorderSpriteColor.b);
				SDL_RenderTextureRotated(s_renderer, atlas, &src, &dest, 0, nullptr, SDL_FLIP_HORIZONTAL);
			}

			// Now draw the left border
			src.x = borderSprite.atlasOffset.x;
			src.y = borderSprite.atlasOffset.y;
			src.w = borderSprite.spriteSize.x;
			src.h = borderSprite.spriteSize.y;

			dest.x = topLeftDialogueOptionPosition.x - dialogueOptionsOuterPadding.x - k_dialogueOptionBorderSize.x;
			dest.y = dest.y;
			dest.w = k_dialogueOptionBorderSize.x;
			// Don't override dest.y since we can share the one used for option's dialogue box

			SDL_SetTextureColorMod(atlas, dialogueOption.backgroundSpriteColor.r, dialogueOption.backgroundSpriteColor.g, dialogueOption.backgroundSpriteColor.b);
			SDL_RenderTexture(s_renderer, atlas, &src, &dest);

			// Hovered border for left side
			if (dialogueOption.state == DIALOGUE_OPTION_HOVERED_STATE ||
				dialogueOption.state == DIALOGUE_OPTION_CHOSEN_STATE)
			{
				DialogueBoxSprite& hoveredBorderSprite = k_dialogueSpeechSprites[DIALOGUE_OPTION_HOVERED_BORDER_SPRITE];

				src.x = hoveredBorderSprite.atlasOffset.x;
				src.y = hoveredBorderSprite.atlasOffset.y;
				src.w = hoveredBorderSprite.spriteSize.x;
				src.h = hoveredBorderSprite.spriteSize.y;

				dest.w = k_dialogueHoveredBorderSize.x;
				// Don't override dest.y since we can share the one used for option's dialogue box

				SDL_Texture* atlas = renderingSystem->loadAtlas(hoveredBorderSprite.atlasType);

				SDL_SetTextureColorMod(atlas, dialogueOption.hoveredBorderSpriteColor.r, dialogueOption.hoveredBorderSpriteColor.g, dialogueOption.hoveredBorderSpriteColor.b);
				SDL_RenderTexture(s_renderer, atlas, &src, &dest);
			}
		}

		// Custom colors for options depending on tension
		SDL_Color optionTextColor;
		{
			switch (dialogueOption.optionTensionType)
			{
			case LOW_TENSION:
				optionTextColor = { 240, 79, 120 };
				break;
			case NORMAL_TENSION:
				optionTextColor = { 209, 209, 209 };
				break;
			case HIGH_TENSION:
				optionTextColor = { 255, 0, 0 };
				break;
			case FATAL_TENSION:
				optionTextColor = { 255, 0, 0 };
				break;
			default:
				D_ASSERT(false, "Unsupported tension type");
				break;
			}
		}

		// Dialogue options characters
		for (uint16_t i = 0; i < k_maxCharactersPerDialogue; ++i)
		{
			DialogueCharacter& c = dialogueOption.characters[i];

			// Check if it causes issues. The thought process is that if we find a not valid character, we stop printing
			// Since it means we reached the end of the dialogue
			if (!c.isValid())
			{
				break;
			}

			src.x = c.atlasOffset.x;
			src.y = c.atlasOffset.y;
			src.w = k_characterSizeOnAtlas.x;
			src.h = k_characterSizeOnAtlas.y;

			dest.x = c.position.x;
			dest.y = c.position.y;
			dest.w = c.size.x;

			dest.h = c.dynamicYSize;
			c.opacity = dialogueOption.opacity;

			SDL_Texture* fontAtlas = renderingSystem->loadAtlas(FONT_ATLAS);

			if (isColorValid(c.overrideColor))
			{
				SDL_SetTextureColorMod(fontAtlas, c.overrideColor.r, c.overrideColor.g, c.overrideColor.b);
			}
			else
			{
				SDL_SetTextureColorMod(fontAtlas, optionTextColor.r, optionTextColor.g, optionTextColor.b);
			}

			SDL_SetTextureAlphaMod(fontAtlas, c.opacity);
			SDL_RenderTexture(s_renderer, fontAtlas, &src, &dest);
		}
	}
}

void UISystem::debugColliders()
{
#ifndef RELEASE_BUILD

	if (!s_debugUICollidersEnabled)
	{
		return;
	}

	bool doesDialogueHaveOptions = _dialogueOptions[0].isValid();
	if (!doesDialogueHaveOptions)
	{
		return;
	}

	SDL_Color dialogueOptionsDebugColor = { 251, 107, 29 };
	for (DialogueOption& dialogueOption : _dialogueOptions)
	{
		DebugSystem::debugRect({ dialogueOption.colliderDest.x, dialogueOption.colliderDest.y }, 
							   { {0,0}, {(int32_t)dialogueOption.colliderDest.w, (int32_t)dialogueOption.colliderDest.h} }, dialogueOptionsDebugColor, true);
	}
#endif // !RELEASE_BUILD
}

// Dpending on alignmentType the position should be different things.
// IF it's CENTERED, position should be bottom center pos. If it's left aligned, should be bottom left pos. If it's right aligned, should be bottom right pos
Vec2 getPositionToStartDrawingText(const char* textToShow, Vec2 position, DialogueAlignmentType alignmentType, uint8_t maxCharactersPerLine = 35)
{
	uint32_t currentHorizontalSpaceBetweenCharacters = 0;
	uint32_t currentVerticalSpaceBetweenCharacters = 0;
	uint16_t charactersOnCurrentLineCounter = 0;

	float maxXDialogueSize = 0;

	bool isTextEffectSyntax = false;

	for (int i = 0; textToShow[i] != '\0'; ++i)
	{
		char c = textToShow[i];
		bool isSpaceCharacter = (c == 32);

		if (c == '[')
		{
			isTextEffectSyntax = true;
			continue;
		}

		if (c == ']')
		{
			isTextEffectSyntax = false;
			continue;
		}

		if (isTextEffectSyntax)
		{
			continue;
		}

		// We only break to a new line if it's a space character. This avoids breaking words in half
		bool shouldBreakToNewLine = (++charactersOnCurrentLineCounter >= maxCharactersPerLine && isSpaceCharacter);
		if (shouldBreakToNewLine)
		{
			if (currentHorizontalSpaceBetweenCharacters > maxXDialogueSize)
			{
				maxXDialogueSize = currentHorizontalSpaceBetweenCharacters;
			}

			currentVerticalSpaceBetweenCharacters += k_pixelsBetweenNewLine;
			currentHorizontalSpaceBetweenCharacters = 0;
			charactersOnCurrentLineCounter = 0;
			continue;
		}

		// If we won't break to a new line, add spacing between the characters
		currentHorizontalSpaceBetweenCharacters += k_characterSize.x + k_pixelsBetweenCharacters;

		// Also check outside because the longest line might be the last oen
		if (currentHorizontalSpaceBetweenCharacters > maxXDialogueSize)
		{
			maxXDialogueSize = currentHorizontalSpaceBetweenCharacters;
		}
	}

	// Calculate dialogue box size
	Vec2 dialogueBoxSize;

	bool doesDialogueHaveMoreThanOneLine = currentVerticalSpaceBetweenCharacters > 0;
	dialogueBoxSize.x = doesDialogueHaveMoreThanOneLine ? maxXDialogueSize : currentHorizontalSpaceBetweenCharacters;

	float yPosWhereLastLineEnds = currentVerticalSpaceBetweenCharacters + k_characterSize.y;
	dialogueBoxSize.y = yPosWhereLastLineEnds;

	Vec2 topLeftPositionToStartDrawingText;

	switch (alignmentType)
	{
	case DIALOGUE_CENTER_ALIGNED:
		// Go to left from center. Position passed as argument must be bottomCenterPosition
		topLeftPositionToStartDrawingText.x = position.x - (dialogueBoxSize.x / 2);
		break;
	case DIALOGUE_LEFT_ALIGNED:
		// No need to change it. Position passed as arg must be bottomLeftPosition
		topLeftPositionToStartDrawingText.x = position.x;
		break;
	case DIALOGUE_RIGHT_ALIGNED:
		topLeftPositionToStartDrawingText.x = position.x - dialogueBoxSize.x;
		break;
	}

	// Go to top from bottom, also considering the dialogue outer padding and speech indicator size.
	// This way, we can pass a pos bottomCenterPosition that corresponds to where the tip of the speech indicator will be
	topLeftPositionToStartDrawingText.y = position.y - dialogueBoxSize.y - k_dialogueOuterPadding.y - k_speechIndicatorSize.y;

	return topLeftPositionToStartDrawingText;
}

bool UISystem::isCurrentDialogue(TextType dialogueType)
{
	return (_currentDialogue.dialogueType == dialogueType);
}

bool UISystem::hasDialogueFinihsed(TextType dialogueType)
{
	return (_currentDialogue.dialogueType == dialogueType) && (_currentDialogue.state == DIALOGUE_ENDED_STATE);
}

bool UISystem::hasChosenOption(TextType dialogueType)
{
	return (_currentDialogue.state == DIALOGUE_ENDED_STATE) && (_currentDialogue.dialogueOptionChosen == dialogueType);
}

bool UISystem::hasDialogueFinishedInterrupting(TextType dialogueType)
{
	return (_currentDialogue.state == DIALOGUE_FINISHED_INTERRUPTED) && (_currentDialogue.dialogueType == dialogueType);
}

bool UISystem::hasAnyDialogueOngoing()
{
	return (_currentDialogue.dialogueType != INVALID_TEXT);
}

void UISystem::receivePhoneCallAndPushDialogueOnAnswer(TextType dialogueTextType)
{
	_cellphone.state = CELLPHONE_PENDING_CALL_STATE;
	_cellphone.textToShowOnAnswer = dialogueTextType;
}

void UISystem::hangupPhone()
{
	_cellphone.state = CELLPHONE_NOT_VISIBLE_STATE;
}

void UISystem::pushCellphoneDialogue(TextType dialogueTextType, const DialogueOptionsDTO dialogueOptions)
{
	pushTensionBar();
	pushEntityDialogue(dialogueTextType, dialogueOptions, false, DIALOGUE_CENTER_ALIGNED);
}

void applyStaticTextEffect(UISystem::DialogueCharacter& dialogueCharacter)
{
	// Apply text effects that don't require update-based changes
	switch (dialogueCharacter.textEffectToApply)
	{
	case LIGHT_PINK_EFFECT:
		dialogueCharacter.overrideColor = { 240, 79, 120 };
		break;
	case PINK_EFFECT:
	case PINK_WAVE_EFFECT:
		dialogueCharacter.overrideColor = { 240, 79, 210 };
		break;
	case FATHER_EDWARD_EFFECT:
		dialogueCharacter.overrideColor = { 255, 255, 255};
		break;
	case ROSTOV_EFFECT:
		dialogueCharacter.overrideColor = { 139, 191, 174 };
		break;
	case DARWIN_EFFECT:
		dialogueCharacter.overrideColor = { 174, 77, 77 };
		break;
	case PA_EFFECT:
		dialogueCharacter.overrideColor = { 75, 114, 110 };
		break;
	case BLUE_EFFECT:
		dialogueCharacter.overrideColor = { 77, 101, 180 };
		break;
	case YELLOW_EFFECT:
		dialogueCharacter.overrideColor = { 249, 194, 43 };
		break;
	case RED_EFFECT:
	case RED_SHAKE_EFFECT:
		dialogueCharacter.overrideColor = { 198, 35, 35 };
		break;
	case INTERJECTION_EFFECT:
		dialogueCharacter.rotationAngle = 1.f;
		break;
	default:
		break;
	}
}

void UISystem::pushEntityDialogue(TextType dialogueTextType, const DialogueOptionsDTO dialogueOptions, bool isScreenSpace, DialogueAlignmentType alignmentType)
{
	TextDTO textInfo = getTextInfo(dialogueTextType);
	const char* textToShow = textInfo.text;
	if (strlen(textToShow) > k_maxCharactersPerDialogue)
	{
		D_ASSERT(false, "Trying to print more characters per dialogue than allowed");
		return;
	}

	destroyCurrentDialogue();

	updateDialogueColorsAndOffsetForEntity(textInfo.entityTalking);

	static SDL_FRect src;
	static SDL_FRect dest;

	uint32_t currentHorizontalSpaceBetweenCharacters = 0;
	uint32_t currentVerticalSpaceBetweenCharacters = 0;
	uint16_t charactersOnCurrentLineCounter = 0;
	float maxXDialogueSize = 0;

	uint8_t maxCharactersPerLine = 35;

	Entity* entityToAttachDialogue = &getEntityById(s_currentDialogueEntityDTO.entityId);
	if (entityToAttachDialogue->id == k_invalidId)
	{
		D_ASSERT(false, "Trying to create dialogue on invalid entity, fallback to player");
		entityToAttachDialogue = &getEntityById(k_playerEntityId);
	}

	Vec2 entityPosition = getComponentFromEntity<TransformComponent>(*entityToAttachDialogue)->position;
	Vec2 positionToDrawText = { entityPosition.x + s_currentDialogueEntityDTO.dialoguePositionOffset.x,
							   entityPosition.y + s_currentDialogueEntityDTO.dialoguePositionOffset.y };

	_currentDialogue.topLeftPosition = getPositionToStartDrawingText(textToShow, positionToDrawText, alignmentType, maxCharactersPerLine);
	_currentDialogue.isScreenSpace = isScreenSpace;
	_currentDialogue.alignmentType = alignmentType;
	_currentDialogue.dialogueType = dialogueTextType;
	_currentDialogue.timeSinceDialogueStarted = 0.f;
	_currentDialogue.tensionDelta = textInfo.playerTensionDelta;
	_currentDialogue.entityTalking = entityToAttachDialogue;

	bool isCheckingEffectName = false;
	TextEffectType textEffectApplying = INVALID_EFFECT;
	uint8_t effectNameLength = 0;
	static char effectToApplyName[64] = "";

	// Same as i inside the for loop but ignores everything that's text effects syntax
	uint16_t mainDialogueCurrentCharacterIndex = 0;

	// Some text effects (example WAIT) will make the next character wait a bit more before showing
	float extraSecondsToStartShowingCharacter = 0.f;

	float secondsToShowPreviousCharacter = 0.f;

	// Main dialogue characters
	for (int i = 0; textToShow[i] != '\0'; ++i)
	{
		char c = textToShow[i];
		uint16_t atlasIndex = _asciiToAtlasIndex[c];

		bool isSpaceCharacter = (c == 32);
		if (atlasIndex == 0 && !isSpaceCharacter)
		{
			D_LOG(ERROR, "Trying to print unsupported character: %c", c);
			// Continue to prevent printing unsupported characters as a space character
			continue;
		}

		// The character [ is reserved for text effects (start and end)
		// We also continue since we don't want to draw to the text the effect code
		// Ex: [yellow] Hello [yellow] should only be printed to the user as Hello.
		if (c == '[')
		{
			isCheckingEffectName = true;
			continue;
		}

		if (isCheckingEffectName)
		{
			// Character ] is reserved for text effetcs [yellow] <- it means we should stop reading the effect name
			// and check what to apply until we see [yellow] again, or stop the effect that's being applied
			if (c == ']')
			{
				// Stop applying effet
				if (textEffectApplying != INVALID_EFFECT)
				{
					textEffectApplying = INVALID_EFFECT;
					D_LOG(MINI, "Stopped applying text effect: %s", effectToApplyName);
				}
				else
				{
					textEffectApplying = getTextEffectTypeFromName(effectToApplyName);
				}

				isCheckingEffectName = false;
				memset(effectToApplyName, 0, effectNameLength);
				effectNameLength = 0;
				continue;
			}

			effectToApplyName[effectNameLength++] = c;
			continue;
		}

		uint8_t column = atlasIndex % k_maxCharactersPerRowOnAtlas;
		uint8_t row = floor(atlasIndex / k_maxCharactersPerRowOnAtlas);

		src.x = k_firstCharacterOnAtlasOffset.x + (k_spaceBetweenCharactersOnAtas.x * column);
		src.y = k_firstCharacterOnAtlasOffset.y + (k_spaceBetweenCharactersOnAtas.y * row);
		src.w = k_characterSizeOnAtlas.x;
		src.h = k_characterSizeOnAtlas.y;

		dest.x = _currentDialogue.topLeftPosition.x + currentHorizontalSpaceBetweenCharacters;
		dest.y = _currentDialogue.topLeftPosition.y + currentVerticalSpaceBetweenCharacters;
		// Draw comma below where it should. Otherwise it looks weird since every character is 8x8
		if (c == ',')
		{
			dest.y += 1.f;
		}

		dest.w = k_characterSize.x;
		dest.h = k_characterSize.y;

		DialogueCharacter& dialogueCharacter = _currentDialogue.characters[mainDialogueCurrentCharacterIndex];
		dialogueCharacter.atlasOffset = { (int)src.x, (int)src.y };
		dialogueCharacter.startingPosition = { dest.x, dest.y };
		dialogueCharacter.textEffectToApply = textEffectApplying;

		float k_secondsBetweenEachCharacter = 0.035f;
		if (mainDialogueCurrentCharacterIndex == 0)
		{
			dialogueCharacter.secondsToStartShowingCharacter = (k_secondsToStartShowingFirstCharacter * 0.5f) + (k_secondsBetweenEachCharacter * mainDialogueCurrentCharacterIndex);
		}
		else
		{
			dialogueCharacter.secondsToStartShowingCharacter = secondsToShowPreviousCharacter + k_secondsBetweenEachCharacter + extraSecondsToStartShowingCharacter;

			// Some text effects show every character in a word at the same time
			bool canShowAllCharacterAtSameTime = (dialogueCharacter.textEffectToApply == RED_SHAKE_EFFECT || dialogueCharacter.textEffectToApply == INTERJECTION_EFFECT);
			if (canShowAllCharacterAtSameTime)
			{
				float extraTimeFromLastCharacter = dialogueCharacter.textEffectToApply == RED_SHAKE_EFFECT ? 0.01f : 0.f;
				dialogueCharacter.secondsToStartShowingCharacter = secondsToShowPreviousCharacter + extraTimeFromLastCharacter;
			}
		}
		secondsToShowPreviousCharacter = dialogueCharacter.secondsToStartShowingCharacter;

		// Offset character and make it a bit smaller to animate during fade in
		dialogueCharacter.position = { dest.x + 2.f, dest.y };
		dialogueCharacter.size.x = { k_characterSize.x / 2.f };
		dialogueCharacter.size.y = { k_characterSize.y / 2.f };

		applyStaticTextEffect(dialogueCharacter);

		mainDialogueCurrentCharacterIndex++;

		// These characters will always delay the next character to pretend it's an actual speech with punctuation
		bool canApplyExtraSecondsToShowNextCharacter = (c == '?') || (c == '!') || (c == ',') || (c == '.') || (dialogueCharacter.textEffectToApply == INTERJECTION_EFFECT);
		extraSecondsToStartShowingCharacter = canApplyExtraSecondsToShowNextCharacter ? 0.5f : 0.f;

		// We only break to a new line if it's a space character. This avoids breaking words in half
		bool shouldBreakToNewLine = (++charactersOnCurrentLineCounter >= maxCharactersPerLine && isSpaceCharacter);
		if (shouldBreakToNewLine)
		{
			if (currentHorizontalSpaceBetweenCharacters > maxXDialogueSize)
			{
				maxXDialogueSize = currentHorizontalSpaceBetweenCharacters;
			}

			currentVerticalSpaceBetweenCharacters += k_pixelsBetweenNewLine;
			currentHorizontalSpaceBetweenCharacters = 0;
			charactersOnCurrentLineCounter = 0;
			continue;
		}

		// If we won't break to a new line, add spacing between the characters
		currentHorizontalSpaceBetweenCharacters += k_characterSize.x + k_pixelsBetweenCharacters;

		// This will only be executed if the very last word makes the dialogue box go beyond maxXDialogueSize
		// As a safeguard, also update maxXDialogueSize for the text to not be drawn outside
		if (currentHorizontalSpaceBetweenCharacters > maxXDialogueSize)
		{
			maxXDialogueSize = currentHorizontalSpaceBetweenCharacters;
		}
	}

	// Main Dialogue speech bubble sprite
	{
		bool doesDialogueHaveMoreThanOneLine = currentVerticalSpaceBetweenCharacters > 0;
		_currentDialogue.dialogueBoxSize.x = doesDialogueHaveMoreThanOneLine ? maxXDialogueSize : currentHorizontalSpaceBetweenCharacters;

		// Add offset to the end to fit dialogue ended indicator
		_currentDialogue.dialogueBoxSize.x += 3;

		_currentDialogue.dialogueOutlineDynamicXSize = _currentDialogue.dialogueBoxSize.x;

		float yPosWhereLastLineEnds = currentVerticalSpaceBetweenCharacters + k_characterSize.y;
		_currentDialogue.dialogueBoxSize.y = yPosWhereLastLineEnds;
	}

	for (DialogueOption& option : _dialogueOptions) { option.destroyDialogueOption(); }
	// Dialogue options
	bool hasDialogueOptions = (dialogueOptions.options[0] != INVALID_TEXT);
	if (!hasDialogueOptions)
	{
		return;
	}

	maxCharactersPerLine = 50;

	float screenCenterX = k_baseGameWidth * 0.5f;
	Vec2 dialogueOptionsStartPosition[k_maxDialogueOptions] = { { screenCenterX - 15.f, 162.f } , { screenCenterX + 15.f, 162.f } , { screenCenterX, 174.f } };
	DialogueAlignmentType dialogueOptionsAlignmentType[k_maxDialogueOptions] = { DIALOGUE_RIGHT_ALIGNED, DIALOGUE_LEFT_ALIGNED, DIALOGUE_CENTER_ALIGNED };

	// Dialogue options characters + speech bubble
	for (uint8_t optionIndex = 0; optionIndex < k_maxDialogueOptions; ++optionIndex)
	{
		DialogueOption& dialogueOption = _dialogueOptions[optionIndex];

		dialogueOption.destroyDialogueOption();
		if (dialogueOptions.options[optionIndex] == INVALID_TEXT)
		{
			continue;
		}

		TextDTO optionInfo = getTextInfo(dialogueOptions.options[optionIndex]);

		dialogueOption.dialogueType = dialogueOptions.options[optionIndex];
		dialogueOption.tensionDelta = optionInfo.playerTensionDelta;
		dialogueOption.optionTensionType = optionInfo.tensionType;

		currentHorizontalSpaceBetweenCharacters = 0;
		currentVerticalSpaceBetweenCharacters = 0;
		charactersOnCurrentLineCounter = 0;
		maxXDialogueSize = 0;

		const char* optionText = optionInfo.text;
		Vec2 positionToDrawOptionText = getPositionToStartDrawingText(optionText, dialogueOptionsStartPosition[optionIndex], dialogueOptionsAlignmentType[optionIndex], maxCharactersPerLine);

		// Same as i inside the for loop but ignores everything that's text effects syntax
		uint16_t optionsTextCurrentCharacterIndex = 0;

		for (int i = 0; optionText[i] != '\0'; ++i)
		{
			char c = optionText[i];
			uint16_t atlasIndex = _asciiToAtlasIndex[c];

			bool isSpaceCharacter = (c == 32);
			if (atlasIndex == 0 && !isSpaceCharacter)
			{
				D_LOG(ERROR, "Trying to print unsupported character: %c", c);
				// Continue to prevent printing unsupported characters as a space character
				continue;
			}

			// The character [ is reserved for text effects (start and end)
			// We also continue since we don't want to draw to the text the effect code
			// Ex: [yellow] Hello [yellow] should only be printed to the user as Hello.
			if (c == '[')
			{
				isCheckingEffectName = true;
				continue;
			}

			if (isCheckingEffectName)
			{
				// This character ] is reserved for text effetcs [yellow] <- it means we should stop reading the effect name
				// and check what to apply until we see [yellow] again
				if (c == ']')
				{
					// Stop applying effet
					if (textEffectApplying != INVALID_EFFECT)
					{
						textEffectApplying = INVALID_EFFECT;
						D_LOG(MINI, "Stopped applying options text effect: %s", effectToApplyName);
					}
					else
					{
						textEffectApplying = getTextEffectTypeFromName(effectToApplyName);
					}

					isCheckingEffectName = false;
					memset(effectToApplyName, 0, effectNameLength);
					effectNameLength = 0;
					continue;
				}

				effectToApplyName[effectNameLength++] = c;
				continue;
			}

			uint8_t column = atlasIndex % k_maxCharactersPerRowOnAtlas;
			uint8_t row = floor(atlasIndex / k_maxCharactersPerRowOnAtlas);

			src.x = k_firstCharacterOnAtlasOffset.x + (k_spaceBetweenCharactersOnAtas.x * column);
			src.y = k_firstCharacterOnAtlasOffset.y + (k_spaceBetweenCharactersOnAtas.y * row);
			src.w = k_characterSizeOnAtlas.x;
			src.h = k_characterSizeOnAtlas.y;

			dest.x = positionToDrawOptionText.x + currentHorizontalSpaceBetweenCharacters;
			dest.y = positionToDrawOptionText.y + currentVerticalSpaceBetweenCharacters;
			if (c == ',')
			{
				dest.y += 1.f;
			}

			dest.w = k_characterSize.x;
			dest.h = k_characterSize.y;

			DialogueCharacter& dialogueCharacter = _dialogueOptions[optionIndex].characters[optionsTextCurrentCharacterIndex];
			dialogueCharacter.atlasOffset = { (int)src.x, (int)src.y };
			dialogueCharacter.position = { dest.x, dest.y };
			dialogueCharacter.size = { k_characterSize };
			dialogueCharacter.dynamicYSize = 0.f;
			dialogueCharacter.textEffectToApply = textEffectApplying;
			applyStaticTextEffect(dialogueCharacter);

			optionsTextCurrentCharacterIndex++;

			// We only break to a new line if it's a space character. This avoids breaking words in half
			bool shouldBreakToNewLine = (++charactersOnCurrentLineCounter >= maxCharactersPerLine && isSpaceCharacter);
			if (shouldBreakToNewLine)
			{
				if (currentHorizontalSpaceBetweenCharacters > maxXDialogueSize)
				{
					maxXDialogueSize = currentHorizontalSpaceBetweenCharacters;
				}

				currentVerticalSpaceBetweenCharacters += k_pixelsBetweenNewLine;
				currentHorizontalSpaceBetweenCharacters = 0;
				charactersOnCurrentLineCounter = 0;
				continue;
			}

			// If we won't break to a new line, add spacing between the characters
			currentHorizontalSpaceBetweenCharacters += k_characterSize.x + k_pixelsBetweenCharacters;
		}

		// Dialogue option speech bubble
		{
			bool doesDialogueHaveMoreThanOneLine = currentVerticalSpaceBetweenCharacters > 0;
			dialogueOption.dialogueBoxSize.x = doesDialogueHaveMoreThanOneLine ? maxXDialogueSize : currentHorizontalSpaceBetweenCharacters;

			float yPosWhereLastLineEnds = currentVerticalSpaceBetweenCharacters + k_characterSize.y;
			dialogueOption.dialogueBoxSize.y = yPosWhereLastLineEnds;

			dialogueOption.dialogueBoxDynamicYSize = 0.f;

			// Dialogue options appear close to the finishing of the main dialogue
			uint16_t mainDialogueLength = mainDialogueCurrentCharacterIndex;
			int32_t characterIndexToStartShowingOptions = max(mainDialogueLength - 10, 3);
			float baseSecondsToWaitBeforeShowingOptions = _currentDialogue.characters[characterIndexToStartShowingOptions].secondsToStartShowingCharacter;

			float baseSecondsToStartShowingOption = baseSecondsToWaitBeforeShowingOptions + (optionIndex * 0.2f);

			if (dialogueOption.optionTensionType == FATAL_TENSION)
			{
				dialogueOption.secondsToStartShowingOption = baseSecondsToStartShowingOption + 1.f;
			}
			else
			{
				dialogueOption.secondsToStartShowingOption = baseSecondsToStartShowingOption;
			}
		}
	}

}

#pragma endregion


