#include "core/systems.h"
#include "level.h"
#include "components.h"
#include "core/constants.h"
#include "core/input.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>
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

	SDL_Surface* surface = SDL_LoadPNG(atlasFilePath.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(s_renderer, surface);
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
	//TODO: If needed improve performance, since every function interates over every entity
	computeLightsAtLayer(BACK_LIGHTS_LAYER);
	computeLightsAtLayer(FRONT_LIGHTS_LAYER);

	renderSpritesAtLayer(BEHIND_CHAR_LAYER, renderAlpha);
	renderLightsAtLayer(BACK_LIGHTS_LAYER);
	renderSpritesAtLayer(CHARACTER_LAYER, renderAlpha);
	renderSpritesAtLayer(IN_FRONT_CHAR_LAYER, renderAlpha);
	renderLightsAtLayer(FRONT_LIGHTS_LAYER);
	renderSpritesAtLayer(LEVEL_GEOMETRY_LAYER, renderAlpha);
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

		Vec2 cameraPosition = LevelManager::getCurrentLevel()->_levelCamera.position;

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

		if (spriteComponent->drawnAtScreenSpace)
		{
			_dest.x = interpolatedPosition.x + scaleOffsetX;
		}
		else
		{
			_dest.x = interpolatedPosition.x + scaleOffsetX - cameraPosition.x;
		}

		_dest.y = interpolatedPosition.y + scaleOffsetY;
		_dest.w = scaledWidth;
		_dest.h = scaledHeight;

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

void RenderingSystem::renderLightsAtLayer(LayerType layer)
{
	SDL_Texture* targetBuffer = getTargetLightsBuffer(layer);
	if (isColorValid(s_ambientColor))
	{
		SDL_SetTextureBlendMode(targetBuffer, SDL_BLENDMODE_MOD);
	}
	SDL_RenderTexture(s_renderer, targetBuffer, nullptr, nullptr);
}

void RenderingSystem::computeLightsAtLayer(LayerType layer)
{
	SDL_Texture* targetBuffer = getTargetLightsBuffer(layer);

	// Start drawing to lightsBuffer, make it all black, and set its blend mode to additive
	SDL_SetRenderTarget(s_renderer, targetBuffer);
	SDL_SetTextureBlendMode(targetBuffer, SDL_BLENDMODE_ADD);

	if (isColorValid(s_ambientColor))
	{
		SDL_SetRenderDrawColor(s_renderer, s_ambientColor[0], s_ambientColor[1], s_ambientColor[2], 255);
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

		Vec2 cameraPosition = LevelManager::getCurrentLevel()->_levelCamera.position;

		_src.x = spriteComponent->atlasOffset.x;
		_src.y = spriteComponent->atlasOffset.y;
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

void DebugSystem::debugRect(Vec2 position, RectCollider collider, SDL_Color color)
{
	SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, color.a);
	Vec2 colliderPosition = getColliderPosition(position, collider);

	Vec2 cameraPosition = LevelManager::getCurrentLevel()->_levelCamera.position;
	SDL_FRect debugRect{ colliderPosition.x - cameraPosition.x, colliderPosition.y, (float)collider.size.x, (float)collider.size.y };

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

	// Follow mouse
	{
		SDL_HideCursor();
		auto* t = getComponentFromEntity<TransformComponent>(crosshair);
		Vec2 targetPosition = { s_mousePositionThisFrameInScreenSpace.x - 3.5f, s_mousePositionThisFrameInScreenSpace.y - 4 };
		t->position = lerp(t->position, targetPosition, 1.f);
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
	//	Vec2 smearPosition = { LevelManager::getCurrentLevel()->_levelCamera.position.x, hitLocation.y };
	//	_crosshairSmear.entity = &addEntity(smearPosition);
	//	auto* sprite = addComponentToEntity<SpriteComponent>(*_crosshairSmear.entity);
	//	sprite->setupSpriteForLayer(SMEAR_MELEE_ATTACK_SPRITE, BEHIND_CHAR_LAYER);
	//	sprite->color = { 138, 148, 255, 100 };
	//	sprite->rotationPivotType = TOP_LEFT_ROTATION;

	//	// rotate towards mouse
	//	Vec2 mouseWorldPosition = convertScreenToWorldPosition(s_mousePositionThisFrameInScreenSpace);
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
		transformComponent->previousPosition = Vec2(0, 0);
		transformComponent->position = Vec2(0, 0);
		movementComponent->currentSpeed.x = 0;
		movementComponent->currentSpeed.y = 0;

		Entity& enemy = getEntityById(10);
		getComponentFromEntity<TransformComponent>(enemy)->previousPosition = Vec2(160, 0);
		getComponentFromEntity<TransformComponent>(enemy)->position = Vec2(160, 0);
		getComponentFromEntity<MovementComponent>(enemy)->currentSpeed = Vec2(0, 0);
		getComponentFromEntity<AttackingComponent>(enemy)->damageCounter = 0;
		enemy.entityState = IDLE_STATE;
	}

	bool wasGrounded = movementComponent->isGrounded;
	float horizontalSpeedMultiplier = calculateHorizontalSpeedMultiplier(movementComponent);

	bool isInAllowedStateToMove = player.entityState != ATTACKING_STATE;

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

	// Fake jump if we're 2 pixels away from floor or less
	bool canJumpWithoutTouchingFloor = false;
	Vec2 checkGroundBelowPosition{ transformComponent->position.x, transformComponent->position.y + 2 };
	if (wasJumpKeyPressedThisFrame() && !movementComponent->isGrounded && (willCollideWithLevelGeometryAtPosition(&player, checkGroundBelowPosition)))
	{
		canJumpWithoutTouchingFloor = true;
	}

	// Jump
	bool canCoyoteJump = (movementComponent->timeSinceLeftPlatform > k_invalidId && movementComponent->timeSinceLeftPlatform <= movementComponent->coyoteTime);
	bool canJump = movementComponent->isGrounded || canCoyoteJump || canJumpWithoutTouchingFloor;
	if (wasJumpKeyPressedThisFrame() && canJump)
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
	if (wasJumpKeyReleasedThisFrame())
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

	bool canChangeFromCurrentStateToIdle = player.entityState != ATTACKING_STATE;
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

	bool canChangeToFallingState = movementComponent->currentSpeed.y > 0.f && player.entityState != ATTACKING_STATE;
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
	static bool hasGolf = true;
	if (_wasKeyPressedThisFrame(SDL_SCANCODE_K))
	{
		hasGolf = !hasGolf;
		getComponentFromEntity<AttackingComponent>(player)->weaponInHand = hasGolf ? GOLF_WEAPON_TYPE : NO_WEAPON_TYPE;
	}

	// Handle movement animations
	switch (player.entityState)
	{
	case IDLE_STATE:
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

		switch (entity.entityState)
		{
		case IDLE_STATE:
			getComponentFromEntity<SpriteComponent>(entity)->setAnimationToPlayIfNotPlaying(GANGSTER_SMALL_IDLE_SPRITE, true, 70, 70);
			break;
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
	bool canAttackFromCurrentState = player->entityState != ATTACKING_STATE;
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
			Vec2 mouseWorldPosition = convertScreenToWorldPosition(s_mousePositionThisFrameInScreenSpace);
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

		Vec2 attackStartingLocation = { t->position.x + 35, t->position.y + 18 };
		RectCollider attackCollider = { {0,0}, {17, 10} };

		Vec2 targetPos = tTarget->position;
		RectCollider targetCollider = getComponentFromEntity<RectColliderComponent>(target)->collider;

		addColliderToDebugList(attackStartingLocation, attackCollider);
		if (!aabb(attackStartingLocation, targetPos, attackCollider, targetCollider))
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
		Vec2 mouseWorldPosition = convertScreenToWorldPosition(s_mousePositionThisFrameInScreenSpace);
		bool wasUpHit = mouseWorldPosition.y < targetTransform->position.y;

		auto* mTarget = getComponentFromEntity<MovementComponent>(target);

		bool targetWillRemainStanding = aTarget->damageCounter < aTarget->numberOfHitsToFall;
		if (targetWillRemainStanding)
		{
			target.entityState = HURT_ONE_STATE;
			mTarget->currentSpeed = { 3.f, 0.f };
		}
		else if (aTarget->damageCounter == aTarget->numberOfHitsToFall)
		{
			if (target.entityState == IDLE_STATE)
			{
				target.entityState = HURT_ONE_STATE;
				mTarget->currentSpeed = { 3.f, 0.f };
			}
			else
			{
				target.entityState = HURT_TWO_STATE;
				mTarget->currentSpeed = { 3.f, -1.f };
			}
		}
		else
		{
			if (canKillyEntityFromCurrentState(target.entityState))
			{
				mTarget->currentSpeed = { 4.5f, 0.f };
				target.entityState = DEAD_STATE;
			}
			else
			{
				target.entityState = HURT_TWO_STATE;
				mTarget->currentSpeed = { 3.f, -1.f };
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