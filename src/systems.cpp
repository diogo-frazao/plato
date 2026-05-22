#include "core/systems.h"
#include "level.h"
#include "components.h"
#include "core/constants.h"
#include "core/input.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>
#include <string>

static bool isAmbientColorValid(SDL_Color color)
{
	static constexpr SDL_Color k_whiteColor = { 255, 255, 255, 255 };
	return color.r != k_whiteColor.r || color.g != k_whiteColor.g || color.b != k_whiteColor.b;
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
	//TODO: If needed improve performance, since every function interates over every entity
	computeLightsAtLayer(BACK_LIGHTS_LAYER, true);
	computeLightsAtLayer(FRONT_LIGHTS_LAYER);

	renderSpritesAtLayer(BEHIND_CHAR_LAYER, renderAlpha);
	renderSpritesAtLayer(CHARACTER_LAYER, renderAlpha);
	renderSpritesAtLayer(IN_FRONT_CHAR_LAYER, renderAlpha);
	renderLightsAtLayer(BACK_LIGHTS_LAYER, true);
	renderLightsAtLayer(FRONT_LIGHTS_LAYER);
	renderSpritesAtLayer(LEVEL_GEOMETRY_LAYER, renderAlpha);
	renderSpritesAtLayer(CELLPHONE_LAYER, renderAlpha);
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
			frameSizeX = (spriteComponent->size.x / spriteComponent->numberOfFrames);
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
			scaleOffsetX = (frameSizeX - scaledWidth) * 0.5f;
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
		SDL_SetTextureColorMod(atlas, spriteComponent->color.r, spriteComponent->color.g, spriteComponent->color.b);
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
	if (isAffectedByAmbientLight && isAmbientColorValid(_ambientColor))
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

	if (isAffectedByAmbientLight && isAmbientColorValid(_ambientColor))
	{
		SDL_SetRenderDrawColor(s_renderer, _ambientColor.r, _ambientColor.g, _ambientColor.b, 255);
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

		for (RectCollider& attackCollider : AttackingSystem::s_attackCollisionsToDebugThisFrame)
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

	s->color.a = s_corsshairOpacity;

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
	//	sprite->setupSpriteForLayer(SMEAR_MELEE_ATTACK_SPRITE, BEHIND_CHAR_LAYER);
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

	//TODO: remove debug to reset player pos
	if (_wasKeyPressedThisFrame(SDL_SCANCODE_Q))
	{
		transformComponent->previousPosition = Vec2(490, 117);
		transformComponent->position = Vec2(490, 117);
		movementComponent->currentSpeed.x = 0;
		movementComponent->currentSpeed.y = 0;
		spriteComponent->flipX = true;

		Entity& enemy = getEntityById(10);
		if (enemy.id != k_invalidId && entityHasComponent<AttackingComponent>(enemy))
		{
			getComponentFromEntity<TransformComponent>(enemy)->previousPosition = Vec2(160, 0);
			getComponentFromEntity<TransformComponent>(enemy)->position = Vec2(160, 0);
			getComponentFromEntity<MovementComponent>(enemy)->currentSpeed = Vec2(0, 0);
			getComponentFromEntity<AttackingComponent>(enemy)->damageCounter = 0;
			enemy.entityState = IDLE_STATE;
		}
	}

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

		Entity& takeoffParticle = addEntity({ transformComponent->position.x + 10, transformComponent->position.y + 15 });
		auto* particleTransform = getComponentFromEntity<TransformComponent>(takeoffParticle);
		particleTransform->previousPosition = particleTransform->position;
		particleTransform->scale = { 0.6f, 0.6f };
		addComponentToEntity<SpriteComponent>(takeoffParticle)->setupAnimationForLayer(TAKEOFF_PARTICLE_SPRITE, BEHIND_CHAR_LAYER, false, 70, 70);
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
		Entity& turnParticle = addEntity({ transformComponent->position.x + 12, transformComponent->position.y + 15 });
		auto* particleTransform = getComponentFromEntity<TransformComponent>(turnParticle);
		particleTransform->previousPosition = particleTransform->position;
		particleTransform->scale = { 0.7f, 0.7f };
		addComponentToEntity<SpriteComponent>(turnParticle)->setupAnimationForLayer(TURN_PARTICLE_SPRITE, BEHIND_CHAR_LAYER, false, 70, 70);
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
	static bool hasGolf = false;
	if (_wasKeyPressedThisFrame(SDL_SCANCODE_K))
	{
		hasGolf = !hasGolf;
		getComponentFromEntity<AttackingComponent>(player)->weaponInHand = hasGolf ? GOLF_WEAPON_TYPE : NO_WEAPON_TYPE;
	}

	// Handle movement animations
	switch (player.entityState)
	{
	case IDLE_STATE:
	case ON_CUTSCENE_STATE:
		spriteComponent->setAnimationToPlayIfNotPlaying(hasGolf ? CHARACTER_WEAPON_GOLF_IDLE_SPRITE : CHARACTER_IDLE_SPRITE, true, 70, 600);
		break;
	case TAKE_OFF_STATE:
		spriteComponent->setAnimationToPlayIfNotPlaying(hasGolf ? CHARACTER_WEAPON_GOLF_TAKEOFF_SPRITE : CHARACTER_TAKEOFF_SPRITE, false, 60, 60);
		break;
	case RUNNING_STATE:
		spriteComponent->setAnimationToPlayIfNotPlaying(hasGolf ? CHARACTER_WEAPON_GOLF_RUN_SPRITE : CHARACTER_RUN_SPRITE, true, 70, 70);
		break;
	case SLOWDOWN_STATE:
		spriteComponent->setAnimationToPlayIfNotPlaying(hasGolf ? CHARACTER_WEAPON_GOLF_SLOWDOWN_SPRITE : CHARACTER_RUN_SPRITE, false, 70, 70);
		break;
	case JUMPING_STATE:
		spriteComponent->setAnimationToPlayIfNotPlaying(hasGolf ? CHARACTER_WEAPON_GOLF_JUMP_SPRITE : CHARACTER_JUMP_SPRITE, true, 70, 70);
		break;
	case FALLING_STATE:
		spriteComponent->setAnimationToPlayIfNotPlaying(hasGolf ? CHARACTER_WEAPON_GOLF_FALL_SPRITE : CHARACTER_FALL_SPRITE, false, 70, 70);
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

#pragma region Attacking System

void AttackingSystem::update()
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
			handleMainCharacter();
			continue;
		}

		if (!entityHasComponent<AttackingComponent>(entity))
		{
			continue;
		}

		AttackingComponent* a = getComponentFromEntity<AttackingComponent>(entity);
		SpriteComponent* s = getComponentFromEntity<SpriteComponent>(entity);

		// Later, we also need to switch on entity type
		switch (entity.entityState)
		{
		case HURT_ONE_STATE:
		{
			SpriteType animationToPlay = INVALID_SPRITE;
			switch (a->lastDamageType)
			{
			case BOTTOM_ATTACK:
				animationToPlay = GANGSTER_SMALL_HURT_BOTTOM_SPRITE;
				break;
			case TOP_ATTACK:
				animationToPlay = GANGSTER_SMALL_HURT_TOP_SPRITE;
				break;
			}

			s->setAnimationToPlayIfNotPlaying(animationToPlay, false, 70, 70);

			a->recoverTimer += k_deltaTime;
			if (a->recoverTimer >= a->timeToRecoverFromHurtOneState)
			{
				entity.entityState = HURT_ONE_RECOVER_STATE;
				invalidateTimer(a->recoverTimer);
			}
			break;
		}
		case HURT_ONE_RECOVER_STATE:
		{
			SpriteType animationToPlay = INVALID_SPRITE;
			switch (a->lastDamageType)
			{
			case BOTTOM_ATTACK:
				animationToPlay = GANGSTER_SMALL_HURT_BOTTOM_RECOVER_SPRITE;
				break;
			case TOP_ATTACK:
				animationToPlay = GANGSTER_SMALL_HURT_TOP_RECOVER_SPRITE;
				break;
			}

			s->setAnimationToPlayIfNotPlaying(animationToPlay, false, 70, 70);

			if (s->animationData.finishedPlayingAnimation)
			{
				entity.entityState = IDLE_STATE;
			}

			break;
		}
		case HURT_TWO_STATE:
		{
			SpriteType animationToPlay = INVALID_SPRITE;
			switch (a->lastDamageType)
			{
			case BOTTOM_BOTTOM_ATTACK:
				animationToPlay = GANGSTER_SMALL_HURT_BOTTOM_BOTTOM_SPRITE;
				break;
			case BOTTOM_TOP_ATTACK:
				animationToPlay = GANGSTER_SMALL_HURT_BOTTOM_TOP_SPRITE;
				break;
			case TOP_BOTTOM_ATTACK:
				animationToPlay = GANGSTER_SMALL_HURT_TOP_BOTTOM_SPRITE;
				break;
			case TOP_TOP_ATTACK:
				animationToPlay = GANGSTER_SMALL_HURT_TOP_TOP_SPRITE;
			}

			float animationSpeed = 70.f;
			if (s->animationData.currentFrame == 0)
			{
				animationSpeed = 300.f;
			}
			s->setAnimationToPlayIfNotPlaying(animationToPlay, false, animationSpeed, 70);

			a->recoverTimer += k_deltaTime;
			if (a->recoverTimer >= a->timeToRecoverFromHurtTwoState)
			{
				entity.entityState = HURT_TWO_RECOVER_STATE;
				invalidateTimer(a->recoverTimer);
			}

			break;
		}
		case HURT_TWO_RECOVER_STATE:
		{
			SpriteType animationToPlay = INVALID_SPRITE;
			switch (a->lastDamageType)
			{
			case BOTTOM_BOTTOM_ATTACK:
				animationToPlay = GANGSTER_SMALL_HURT_BOTTOM_BOTTOM_RECOVER_SPRITE;
				break;
			case BOTTOM_TOP_ATTACK:
				animationToPlay = GANGSTER_SMALL_HURT_BOTTOM_TOP_RECOVER_SPRITE;
				break;
			case TOP_BOTTOM_ATTACK:
				animationToPlay = GANGSTER_SMALL_HURT_TOP_BOTTOM_RECOVER_SPRITE;
				break;
			case TOP_TOP_ATTACK:
				animationToPlay = GANGSTER_SMALL_HURT_TOP_TOP_RECOVER_SPRITE;
			}

			s->setAnimationToPlayIfNotPlaying(animationToPlay, false, 70, 70);

			a->recoverTimer += k_deltaTime;
			if (a->recoverTimer >= a->timeToStartCrawling)
			{
				entity.entityState = CRAWL_STATE;
				invalidateTimer(a->recoverTimer);
			}

			break;
		}
		case CRAWL_STATE:
		{
			bool shouldInvertCrawlDirection = false;
			int8_t crawlMovementDirection = -1;

			switch (a->lastDamageType)
			{
			case BOTTOM_TOP_ATTACK:
			case TOP_BOTTOM_ATTACK:
			case TOP_TOP_ATTACK:
				shouldInvertCrawlDirection = true;
				break;
			}

			if (shouldInvertCrawlDirection)
			{
				s->flipX = false;
				crawlMovementDirection = 1;
			}

			//TODO: For now, if we can't crawl, reuse the same state. This should be improved and have a new state associated
			if (a->shouldWaitToDie)
			{
				s->setSpriteData(GANGSTER_SMALL_CRAWL_SPRITE);
				break;
			}

			s->setAnimationToPlayIfNotPlaying(GANGSTER_SMALL_CRAWL_SPRITE, true, 400, 400);

			// TODO: Improve to move alongside animation
			MovementComponent* m = getComponentFromEntity<MovementComponent>(entity);
			m->currentSpeed.x = crawlMovementDirection * 20.f * k_deltaTime;

			break;
		}
		case DEAD_STATE:
			s->setAnimationToPlayIfNotPlaying(GANGSTER_SMALL_DEAD_SPRITE, false, 70, 70);
			break;
		}
	}
}

void AttackingSystem::tryMainCharacterAttack(Entity* player, AttackingComponent* a, MovementComponent* m, TransformComponent* t, SpriteComponent* s, RectColliderComponent* c)
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
	}

	clearEntitiesPlayerAttacked();
	player->entityState = ATTACKING_STATE;
	return;
}

void AttackingSystem::handleMainCharacterAttackAnimations(Entity* player, AttackingComponent* a, MovementComponent* m, SpriteComponent* s)
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

		// Set animation based on state and weapon
		float speedForAttackAnimation = (s->animationData.currentFrame == 2) ? 140 : 70;
		s->setAnimationToPlayIfNotPlaying(CHARACTER_WEAPON_GOLF_ATTACK_MIDDLE_SPRITE, false, speedForAttackAnimation, 70);
		break;
	}
}

void AttackingSystem::handleMainCharacter()
{
	Entity& player = getEntityById(k_playerEntityId);
	auto* a = getComponentFromEntity<AttackingComponent>(player);
	auto* m = getComponentFromEntity<MovementComponent>(player);
	auto* t = getComponentFromEntity<TransformComponent>(player);
	auto* s = getComponentFromEntity<SpriteComponent>(player);
	auto* c = getComponentFromEntity<RectColliderComponent>(player);

	tryMainCharacterAttack(&player, a, m, t, s, c);
	handleMainCharacterAttackAnimations(&player, a, m, s);

	// Actual hit detection, depending on the weapon
	if (player.entityState != ATTACKING_STATE || s->animationData.currentFrame > 3)
	{
		return;
	}

	// Look for targets to hit from player attack
	for (Entity& target : getAllEntities())
	{
		if (target.id == k_invalidId || target.id == k_playerEntityId)
		{
			continue;
		}

		if (!entityHasComponent<AttackingComponent>(target) || !entityHasComponent<RectColliderComponent>(target) ||
			hasPlayerAlreadyAttackedEntity(target.id))
		{
			continue;
		}

		auto* aTarget = getComponentFromEntity<AttackingComponent>(target);
		auto* tTarget = getComponentFromEntity<TransformComponent>(target);

		Vec2 attackStartingLocation{ t->position.x, t->position.y + 18 };
		if (s->flipX)
		{
			attackStartingLocation.x += 8.f;
		}
		else
		{
			attackStartingLocation.x += 35.f;
		}

		RectCollider attackCollider = { {0,0}, {17, 10} };

		Vec2 targetPos = tTarget->position;
		RectCollider targetCollider = getComponentFromEntity<RectColliderComponent>(target)->collider;

		addColliderToDebugList(attackStartingLocation, attackCollider);
		if (!aabb(attackStartingLocation, targetPos, attackCollider, targetCollider))
		{
			continue;
		}

		if (!aTarget->canBeAttacked)
		{
			continue;
		}

		// If we get here, we hit the target
		registerPlayerAttackToEntity(&target);
		aTarget->damageCounter++;

		bool isEnemyAlreadyDead = (target.entityState == DEAD_STATE);
		if (isEnemyAlreadyDead)
		{
			continue;
		}

		// Check if we did a up hit or bottom hit
		TransformComponent* targetTransform = getComponentFromEntity<TransformComponent>(target);
		Vec2 mouseWorldPosition = convertScreenPositionToCameraSpace(s_mousePositionThisFrameInScreenSpace);
		bool wasUpHit = mouseWorldPosition.y < targetTransform->position.y;

		auto* mTarget = getComponentFromEntity<MovementComponent>(target);
		int8_t hitDirection = s->flipX ? -1 : 1;

		bool targetWillRemainStanding = aTarget->damageCounter < aTarget->numberOfHitsToFall;
		if (targetWillRemainStanding)
		{
			target.entityState = HURT_ONE_STATE;
			mTarget->currentSpeed = { 3.f * hitDirection, 0.f };
		}
		else if (aTarget->damageCounter == aTarget->numberOfHitsToFall)
		{
			if (target.entityState == IDLE_STATE)
			{
				target.entityState = HURT_ONE_STATE;
				mTarget->currentSpeed = { 3.f * hitDirection, 0.f };
			}
			else
			{
				target.entityState = HURT_TWO_STATE;
				mTarget->currentSpeed = { 3.f * hitDirection, -1.f };
			}
		}
		else
		{
			if (canKillyEntityFromCurrentState(target.entityState))
			{
				mTarget->currentSpeed = { 4.5f * hitDirection, 0.f };
				target.entityState = DEAD_STATE;
			}
			else
			{
				target.entityState = HURT_TWO_STATE;
				mTarget->currentSpeed = { 3.f * hitDirection, -1.f };
			}
		}

		// TODO: Expand when we add the new combos
		switch (target.entityState)
		{
		case HURT_ONE_STATE:
			aTarget->lastDamageType = wasUpHit ? TOP_ATTACK : BOTTOM_ATTACK;
			break;
		case HURT_TWO_STATE:
			AttackType lastDamageType = aTarget->lastDamageType;
			if (lastDamageType == TOP_ATTACK)
			{
				aTarget->lastDamageType = wasUpHit ? TOP_TOP_ATTACK : TOP_BOTTOM_ATTACK;

			}

			if (lastDamageType == BOTTOM_ATTACK)
			{
				aTarget->lastDamageType = wasUpHit ? BOTTOM_TOP_ATTACK : BOTTOM_BOTTOM_ATTACK;
			}
			break;
		}

		invalidateTimer(aTarget->recoverTimer);
	}
}

void AttackingSystem::registerPlayerAttackToEntity(Entity* entity)
{
	for (int32_t& entityId : _entitiesPlayerAttackedForCurrentAttack)
	{
		if (entityId == k_invalidId)
		{
			entityId = entity->id;
			return;
		}
	}

	D_LOG(ERROR, "Player couldn't attack entity %i because array is too small", entity->id);
}

bool AttackingSystem::hasPlayerAlreadyAttackedEntity(int32_t idToCheck)
{
	for (int32_t entityId : _entitiesPlayerAttackedForCurrentAttack)
	{
		if (entityId == idToCheck)
		{
			return true;
		}
	}

	return false;
}

void AttackingSystem::clearEntitiesPlayerAttacked()
{
	memset(_entitiesPlayerAttackedForCurrentAttack, k_invalidId, sizeof(_entitiesPlayerAttackedForCurrentAttack));
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

float k_secondsToFadeInEachCharacter = 0.3f;
float k_secondsBetweenEachCharacter = 0.035f;

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

		if (_currentDialogue.timeSinceDialogueStarted >= c.secondsToStartShowingCharacter)
		{
			float speed = 255.f / k_secondsToFadeInEachCharacter;
			c.opacity = min(c.opacity + (speed * k_deltaTime), 255.f);
		}
		else
		{
			c.opacity = 0.f;
		}

		if (_currentDialogue.state == DIALOGUE_INTERRUPTED_STATE)
		{
			c.velocity.x = approach(c.velocity.x, 0.4f * sign(c.velocity.x), 1.f * k_deltaTime);
			c.velocity.y = approach(c.velocity.y, 2.5f, 10.f * k_deltaTime);
			c.position.x += c.velocity.x;
			c.position.y += c.velocity.y;

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

	// Auto skip dialogue if it doesn't have choices
	if (_currentDialogue.state == DIALOGUE_BASE_STATE)
	{
		bool doesDialogueHaveOptions = _dialogueOptions[0].isValid();
		float secondsToSkipDialogue = 3.f;
		if (numberOfCharactersOnCurrentDialogue < 10)
		{
			secondsToSkipDialogue = 2.f;
		}

		if (numberOfCharactersOnCurrentDialogue > 70)
		{
			secondsToSkipDialogue = 4.f;
		}

		if (_currentDialogue.timeSinceFinalCharacterWasDrawn >= secondsToSkipDialogue && !doesDialogueHaveOptions)
		{
			_currentDialogue.state = DIALOGUE_ENDED_STATE;
		}
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

	// Launch every character in a random direction
	for (uint16_t i = 0; i < k_maxCharactersPerDialogue; ++i)
	{
		DialogueCharacter& c = _currentDialogue.characters[i];

		if (!c.isValid())
		{
			break;
		}
		
		// Rand 0 or 1
		bool shouldInvertXSpeed = SDL_rand(2);
		c.velocity.x = SDL_randf() * 2.f;
		c.velocity.x *= shouldInvertXSpeed ? -1.f : 1.f;

		c.velocity.y = SDL_randf() * 4.f * -1.f;
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
		// Since the last thing drawn was the dialogue box, use dest directly
		float dialogueBoxEndYPosition = dest.y + dest.h;
		dest.y = dialogueBoxEndYPosition - k_dialogueOutlineHeight;
		dest.w = _currentDialogue.dialogueBoxDynamicXSize;
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

		// Calculate X Position for speech indicator.
		// If needed, move move it (only neeed when the entity moved during the dialogue)
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

				dest.x = min(screenPositionWhereDialogueStartsDrawing.x, maxAllowedPositionForSpeechIndicator);
			}
			else
			{
				dest.x = xPositionToDrawSpeechIndicator;
			}
		}
		

		float dialogueOutlineEndYPosition = dest.y + dest.h;
		dest.y = dialogueOutlineEndYPosition - k_dialogueOutlineHeight;
		dest.w = k_speechIndicatorSize.x;
		dest.h = k_speechIndicatorSize.y;

		// Since the speech indicator has its X and Y pos defined by the previous value, we shouldn't convert to camera space
		// as it's already in camera space
		//dest = convertWorldRectToCameraSpace(dest);

		SDL_Texture* atlas = renderingSystem->loadAtlas(speechIndicatorSprite.atlasType);
		SDL_SetTextureColorMod(atlas, s_currentDialogueEntityDTO.dialogueBoxColor.r, s_currentDialogueEntityDTO.dialogueBoxColor.g, s_currentDialogueEntityDTO.dialogueBoxColor.b);

		float opacity = 255.f;
		if (_currentDialogue.state == DIALOGUE_ENDED_STATE)
		{
			if (_currentDialogue.dialogueBoxDynamicXSize <= 3.f + k_speechIndicatorSize.x)
			{
				opacity = 0.f;
			}
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

		SDL_SetTextureColorMod(fontAtlas, s_currentDialogueEntityDTO.textColor.r, s_currentDialogueEntityDTO.textColor.g, s_currentDialogueEntityDTO.textColor.b);
		SDL_SetTextureAlphaMod(fontAtlas, c.opacity);
		SDL_RenderTexture(s_renderer, fontAtlas, &src, &dest);
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

		// Custom characters colors
		SDL_Color optionTextColor;
		{
			switch (dialogueOption.optionTensionType)
			{
			case LOW_TENSION:
				optionTextColor.r = 240;
				optionTextColor.g = 79;
				optionTextColor.b = 120;
				break;
			case NORMAL_TENSION:
				optionTextColor.r = 209;
				optionTextColor.g = 209;
				optionTextColor.b = 209;
				break;
			case HIGH_TENSION:
				optionTextColor.r = 255;
				optionTextColor.g = 0;
				optionTextColor.b = 0;
				break;
			case FATAL_TENSION:
				optionTextColor.r = 255;
				optionTextColor.g = 0;
				optionTextColor.b = 0;
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

			SDL_SetTextureColorMod(fontAtlas, optionTextColor.r, optionTextColor.g, optionTextColor.b);
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

	for (int i = 0; textToShow[i] != '\0'; ++i)
	{
		char c = textToShow[i];
		bool isSpaceCharacter = (c == 32);

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

		uint8_t column = atlasIndex % k_maxCharactersPerRowOnAtlas;
		uint8_t row = floor(atlasIndex / k_maxCharactersPerRowOnAtlas);

		src.x = k_firstCharacterOnAtlasOffset.x + (k_spaceBetweenCharactersOnAtas.x * column);
		src.y = k_firstCharacterOnAtlasOffset.y + (k_spaceBetweenCharactersOnAtas.y * row);
		src.w = k_characterSizeOnAtlas.x;
		src.h = k_characterSizeOnAtlas.y;

		dest.x = _currentDialogue.topLeftPosition.x + currentHorizontalSpaceBetweenCharacters;
		dest.y = _currentDialogue.topLeftPosition.y + currentVerticalSpaceBetweenCharacters;
		dest.w = k_characterSize.x;
		dest.h = k_characterSize.y;

		DialogueCharacter& dialogueCharacter = _currentDialogue.characters[i];
		dialogueCharacter.atlasOffset = { (int)src.x, (int)src.y };
		dialogueCharacter.position = { dest.x, dest.y };
		dialogueCharacter.size = { k_characterSize };
		dialogueCharacter.secondsToStartShowingCharacter = (k_secondsToStartShowingFirstCharacter * 0.5f) + (k_secondsBetweenEachCharacter * i);

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

			uint8_t column = atlasIndex % k_maxCharactersPerRowOnAtlas;
			uint8_t row = floor(atlasIndex / k_maxCharactersPerRowOnAtlas);

			src.x = k_firstCharacterOnAtlasOffset.x + (k_spaceBetweenCharactersOnAtas.x * column);
			src.y = k_firstCharacterOnAtlasOffset.y + (k_spaceBetweenCharactersOnAtas.y * row);
			src.w = k_characterSizeOnAtlas.x;
			src.h = k_characterSizeOnAtlas.y;

			dest.x = positionToDrawOptionText.x + currentHorizontalSpaceBetweenCharacters;
			dest.y = positionToDrawOptionText.y + currentVerticalSpaceBetweenCharacters;
			dest.w = k_characterSize.x;
			dest.h = k_characterSize.y;

			DialogueCharacter& dialogueCharacter = _dialogueOptions[optionIndex].characters[i];
			dialogueCharacter.atlasOffset = { (int)src.x, (int)src.y };
			dialogueCharacter.position = { dest.x, dest.y };
			dialogueCharacter.size = { k_characterSize };
			dialogueCharacter.dynamicYSize = 0.f;

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
			int mainDialogueLength = int(strlen(textToShow));
			int32_t characterIndexToStartShowingOptions = max(mainDialogueLength - 10, 3);
			float baseSecondsToWaitBeforeShowingOptions = _currentDialogue.characters[characterIndexToStartShowingOptions].secondsToStartShowingCharacter;

			dialogueOption.secondsToStartShowingOption = baseSecondsToWaitBeforeShowingOptions + (optionIndex * 0.2f);
		}
	}

}

#pragma endregion


