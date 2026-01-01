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

		_dest.x = interpolatedPosition.x + scaleOffsetX - cameraPosition.x;
		_dest.y = interpolatedPosition.y + scaleOffsetY;
		_dest.w = scaledWidth;
		_dest.h = scaledHeight;

		SDL_Texture* atlas = loadAtlas(spriteComponent->atlas);
		SDL_SetTextureColorMod(atlas, spriteComponent->color.r, spriteComponent->color.g, spriteComponent->color.b);
		SDL_SetTextureAlphaMod(atlas, spriteComponent->color.a);

		SDL_RenderTextureRotated(s_renderer, atlas, &_src, &_dest, 0, nullptr, spriteComponent->flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
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

	for (Entity& player : getAllEntities())
	{
		if (player.id == k_invalidId)
		{
			continue;
		}

		if (!entityHasComponent<MainCharacterMovementComponent>(player))
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

float calculateHorizontalSpeedMultiplier(MainCharacterMovementComponent* movementComponent)
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
		if (sprite->numberOfFrames == 0)
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

void CharacterMovementSystem::update()
{
	Entity& character = getEntityById(k_playerEntityId);

	auto* transformComponent = getComponentFromEntity<TransformComponent>(character);
	auto* movementComponent = getComponentFromEntity<MainCharacterMovementComponent>(character);
	auto* spriteComponent = getComponentFromEntity<SpriteComponent>(character);

	//TODO: remove debug to reset player pos
	if (_isKeyDown(SDL_SCANCODE_Q))
	{
		transformComponent->previousPosition = Vec2(0, 0);
		transformComponent->position = Vec2(0, 0);
		movementComponent->currentSpeed.x = 0;
		movementComponent->currentSpeed.y = 0;
	}

	bool wasGrounded = movementComponent->isGrounded;
	float horizontalSpeedMultiplier = calculateHorizontalSpeedMultiplier(movementComponent);

	bool isAttacking = movementComponent->movementState == REMOVE_ATTACKING_STATE;

	bool isMovingRight = isMoveRightKeyDown() && !isMoveLeftKeyDown() && !isAttacking;
	bool isMovingLeft = isMoveLeftKeyDown() && !isMoveRightKeyDown() && !isAttacking;

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
	Vec2 checkGroundBelowPosition { transformComponent->position.x, transformComponent->position.y + 2 };
	if (wasJumpKeyPressedThisFrame() && !movementComponent->isGrounded && (willCollideWithLevelGeometryAtPosition(&character, checkGroundBelowPosition)))
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

	//TODO: Improve
	bool attackedThisFrame = false;
	if (wasAttackKeyPressedThisFrame() && movementComponent->isGrounded)
	{
		attackedThisFrame = true;

		if (!isMovingHorizontally || isMovingHorizontally)
		{
			float attackForwardBoost = spriteComponent->flipX ? -2.f : 2.f;
			movementComponent->currentSpeed.x = attackForwardBoost;
		}
	}

	processHorizontalMovement(&character);
	processVerticalMovement(&character);

	// After vertical movement was processed and isGrounded was updated, check coyoteTime
	handleCoyoteTime(movementComponent, wasGrounded);

	static float resetScaleLerp = 1.f;
	transformComponent->scale.x = lerp(transformComponent->scale.x, 1.f, resetScaleLerp);
	transformComponent->scale.y = lerp(transformComponent->scale.y, 1.f, resetScaleLerp);
	
	//TODO REMOVE:
	bool isNotAttacking = movementComponent->movementState != REMOVE_ATTACKING_STATE;
	bool isGroundedAndNotMoving = !isMovingHorizontally && movementComponent->isGrounded;
	if (isGroundedAndNotMoving && isNotAttacking)
	{
		if (abs(movementComponent->currentSpeed.x) <= 0.05f)
		{
			movementComponent->movementState = IDLE_STATE;
		}
		else
		{
			if (movementComponent->movementState != SLOWDOWN_STATE)
			{
				transformComponent->scale.x = 1.15f;
				resetScaleLerp = 0.05f;
			}
			movementComponent->movementState = SLOWDOWN_STATE;
		}
	}

	bool isMovingOnFloor = isMovingHorizontally && movementComponent->isGrounded;
	bool canChangeToTakeOffState = (movementComponent->movementState == IDLE_STATE || movementComponent->movementState == SLOWDOWN_STATE);
	if (isMovingOnFloor && canChangeToTakeOffState)
	{
		movementComponent->movementState = TAKE_OFF_STATE;
		transformComponent->scale.x = 1.3f;
		resetScaleLerp = 1.f;

		Entity& takeoffParticle = addEntity({ transformComponent->position.x + 10, transformComponent->position.y + 15 });
		auto* particleTransform = getComponentFromEntity<TransformComponent>(takeoffParticle);
		particleTransform->previousPosition = particleTransform->position;
		particleTransform->scale = { 0.6f, 0.6f };
		addComponentToEntity<SpriteComponent>(takeoffParticle)->setupAnimationForLayer(TAKEOFF_PARTICLE_SPRITE, BEHIND_CHAR_LAYER, false, 70, 70);
		getComponentFromEntity<SpriteComponent>(takeoffParticle)->color = { 255, 255, 255, 200 };
	}

	bool canChangeFromTakeOffToRunningState = ((movementComponent->movementState == TAKE_OFF_STATE) && 
											  spriteComponent->animationData.finishedPlayingAnimation);
	if (canChangeFromTakeOffToRunningState)
	{
		movementComponent->movementState = RUNNING_STATE;
	}

	bool canChangeFromFallingToRunState = (movementComponent->movementState == FALLING_STATE) && movementComponent->isGrounded && isMovingHorizontally;
	if (canChangeFromFallingToRunState)
	{
		movementComponent->movementState = RUNNING_STATE;
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
		movementComponent->movementState = JUMPING_STATE;
		transformComponent->scale = Vec2(0.8f, 1.5f);
		resetScaleLerp = 0.1f;
	}

	bool canChangeToFallingState = movementComponent->currentSpeed.y > 0.f;
	if (canChangeToFallingState)
	{
		movementComponent->movementState = FALLING_STATE;
	}

	if (!wasGrounded && movementComponent->isGrounded)
	{
		transformComponent->scale = Vec2(1.4f, 0.6f);
		resetScaleLerp = 0.3f;
	}

	//TODO: remove later
	static bool hasGolf = true;
	if (_wasKeyPressedThisFrame(SDL_SCANCODE_K))
	{
		hasGolf = !hasGolf;
	}

	if (attackedThisFrame)
	{
		movementComponent->movementState = REMOVE_ATTACKING_STATE;
	}

	if (movementComponent->movementState == REMOVE_ATTACKING_STATE && spriteComponent->animationData.finishedPlayingAnimation)
	{
		movementComponent->movementState = isMovingOnFloor ? TAKE_OFF_STATE : IDLE_STATE;
	}

	switch (movementComponent->movementState)
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
	case REMOVE_ATTACKING_STATE:
		spriteComponent->setAnimationToPlayIfNotPlaying(CHARACTER_WEAPON_GOLF_ATTACK_MIDDLE_SPRITE, false, 70, 70);
		break;
	}
}

void CharacterMovementSystem::processVerticalMovement(Entity* self)
{
	auto* transformComponent = getComponentFromEntity<TransformComponent>(*self);
	auto* movementComponent = getComponentFromEntity<MainCharacterMovementComponent>(*self);

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

void CharacterMovementSystem::processHorizontalMovement(Entity* self)
{
	auto* transformComponent = getComponentFromEntity<TransformComponent>(*self);
	auto* movementComponent = getComponentFromEntity<MainCharacterMovementComponent>(*self);

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

void CharacterMovementSystem::handleCoyoteTime(MainCharacterMovementComponent* movementComponent, bool wasGrounded)
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

bool CharacterMovementSystem::willCollideWithLevelGeometryAtPosition(Entity* self, const Vec2 positionToCheck)
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