#include "level.h"

#include "core/input.h"
#include "renderingComponents.h"
#include "imgui.h"

int lightThatFollowsPlayerEntityId;

static bool s_canShowImGuiUI = true;

void createBlockAtPosition(IVec2 position)
{
    Entity& block = addEntity();
    addComponentToEntity<SpriteComponent>(block)->setupSpriteForLayer(TODO_TEMOVE_INVISIBLE_SPRITE, LEVEL_GEOMETRY_LAYER);
    getComponentFromEntity<TransformComponent>(block)->previousPosition = { (float)position.x, (float)position.y };
    getComponentFromEntity<TransformComponent>(block)->position = { (float)position.x, (float)position.y };
    addComponentToEntity<RectColliderComponent>(block)->collider = RectCollider({ 0, 0 }, { 8, 8 });
    getComponentFromEntity<RectColliderComponent>(block)->isLevelGeometry = true;
}

void overrideColliderOffsetsBasedOnCurrentSprite()
{
    static const IVec2 k_emptyIVec = { 0,0 };

    for (Entity& entity : getAllEntities())
    {
        if (entity.id == k_invalidId)
        {
            continue;
        }

        if (!entityHasComponent<SpriteComponent>(entity) ||
            !(getComponentFromEntity<SpriteComponent>(entity)->collidertopLeftPointOffset.x > 0) ||
            !(getComponentFromEntity<SpriteComponent>(entity)->collidertopLeftPointOffset.y > 0) ||
            !entityHasComponent<RectColliderComponent>(entity))
        {
            continue;
        }

        getComponentFromEntity<RectColliderComponent>(entity)->collider.topLeftPointOffset = getComponentFromEntity<SpriteComponent>(entity)->collidertopLeftPointOffset;
    }
}

void ECSLevel::start()
{
	_renderingSystem.createLightsBuffers();

    Entity& player = addEntity();
    SpriteComponent* playerSprite = addComponentToEntity<SpriteComponent>(player);
    auto* movementComponent = addComponentToEntity<MovementComponent>(player);
    addComponentToEntity<AttackingComponent>(player)->weaponInHand = GOLF_WEAPON_TYPE;
    getComponentFromEntity<TransformComponent>(player)->useDynamicScale = true;

    Entity& crosshair = addEntity();
    auto* crosshairSprite = addComponentToEntity<SpriteComponent>(crosshair);
    crosshairSprite->setupSpriteForLayer(CROSSHAIR_MELEE_WEAPON_SPRITE, CROSSHAIR_LAYER);
    crosshairSprite->drawnAtScreenSpace = true;

    playerSprite->setupSpriteForLayer(CHARACTER_IDLE_SPRITE, CHARACTER_LAYER);
    addComponentToEntity<RectColliderComponent>(player)->collider = RectCollider({ 4, 4 }, { 9, 17 });

    Entity& bg = addEntity();
    addComponentToEntity<SpriteComponent>(bg)->setupSpriteForLayer(TODO_REMOVE_BG_SPRITE, BEHIND_CHAR_LAYER);

    Entity& geometry = addEntity({0, 135});
    addComponentToEntity<SpriteComponent>(geometry)->setupSpriteForLayer(TODO_REMOVE_LEVEL_GEOMETRY_SPRITE, LEVEL_GEOMETRY_LAYER);

    Entity& fg = addEntity({34, 129});
    addComponentToEntity<SpriteComponent>(fg)->setupSpriteForLayer(TODO_REMOVE_FG_SPRITE, IN_FRONT_CHAR_LAYER);

    Entity& darwinLettersLight = addEntity();
    addComponentToEntity<SpriteComponent>(darwinLettersLight)->setupSpriteForLayer(ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    getComponentFromEntity<TransformComponent>(darwinLettersLight)->position = { 220, 55 };
    getComponentFromEntity<TransformComponent>(darwinLettersLight)->scale = { 1.5f, 0.4f };
    getComponentFromEntity<SpriteComponent>(darwinLettersLight)->color = { 200, 0, 0 , 255};

    Entity& darwinLettersSmallGlow = addEntity();
    addComponentToEntity<SpriteComponent>(darwinLettersSmallGlow)->setupSpriteForLayer(ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    getComponentFromEntity<TransformComponent>(darwinLettersSmallGlow)->position = { 225, 20 };
    getComponentFromEntity<TransformComponent>(darwinLettersSmallGlow)->scale = { 1.35f, 2.f };
    getComponentFromEntity<SpriteComponent>(darwinLettersSmallGlow)->color = { 80, 0, 0 , 255};

    Entity& streetLampLight = addEntity();
    addComponentToEntity<SpriteComponent>(streetLampLight)->setupSpriteForLayer(STREET_LAMP_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    getComponentFromEntity<TransformComponent>(streetLampLight)->position = { 24, 86 };
    getComponentFromEntity<TransformComponent>(streetLampLight)->scale = { 1.f, 1.f };
    getComponentFromEntity<SpriteComponent>(streetLampLight)->color = { 43, 15, 0 , 255};

    Entity& streetLampGlow = addEntity();
    addComponentToEntity<SpriteComponent>(streetLampGlow)->setupSpriteForLayer(ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    getComponentFromEntity<TransformComponent>(streetLampGlow)->position = { 36, 88 };
    getComponentFromEntity<TransformComponent>(streetLampGlow)->scale = { 0.2f, 0.2f };
    getComponentFromEntity<SpriteComponent>(streetLampGlow)->color = { 125, 50, 0 , 200 };

    Entity& lightThatFollowsPlayer = addEntity();
    addComponentToEntity<SpriteComponent>(lightThatFollowsPlayer)->setupSpriteForLayer(ROUND_SOFT_LIGHT_SPRITE, BACK_LIGHTS_LAYER);
    getComponentFromEntity<TransformComponent>(lightThatFollowsPlayer)->position = { 36, 88 };
    getComponentFromEntity<TransformComponent>(lightThatFollowsPlayer)->scale = { 0.5f, 0.5f };
    getComponentFromEntity<SpriteComponent>(lightThatFollowsPlayer)->color = { 87, 69, 50, 42 };
    lightThatFollowsPlayerEntityId = lightThatFollowsPlayer.id;

    Entity& dummyEnemy = addEntity({ 160, 0 });
    addComponentToEntity<SpriteComponent>(dummyEnemy)->setupAnimationForLayer(GANGSTER_SMALL_IDLE_SPRITE, IN_FRONT_CHAR_LAYER, true, 70, 900);
    addComponentToEntity<RectColliderComponent>(dummyEnemy)->collider = RectCollider({ 4, 4 }, { 9, 17 });
    getComponentFromEntity<SpriteComponent>(dummyEnemy)->flipX = true;
    auto* enemyMovementComponent = addComponentToEntity<MovementComponent>(dummyEnemy);
    addComponentToEntity<AttackingComponent>(dummyEnemy);
    getComponentFromEntity<TransformComponent>(dummyEnemy)->useDynamicScale = true;

    #pragma region Level Gemoetry
    createBlockAtPosition({ 0, 152 });
    createBlockAtPosition({ 8, 152 });
    createBlockAtPosition({ 16, 152 });
    createBlockAtPosition({ 24, 152 });
    createBlockAtPosition({ 32, 152 });
    createBlockAtPosition({ 40, 152 });
    createBlockAtPosition({ 48, 152 });
    createBlockAtPosition({ 56, 152 });
    createBlockAtPosition({ 64, 152 });
    createBlockAtPosition({ 72, 152 });
    createBlockAtPosition({ 80, 152 });
    createBlockAtPosition({ 80, 144 });
    createBlockAtPosition({ 80, 136 });
    createBlockAtPosition({ 88, 136 });
    createBlockAtPosition({ 96, 136 });
    createBlockAtPosition({ 104, 136 });
    createBlockAtPosition({ 104, 144 });
    createBlockAtPosition({ 104, 152 });
    createBlockAtPosition({ 112, 152 });
    createBlockAtPosition({ 120, 152 });
    createBlockAtPosition({ 128, 152 });
    createBlockAtPosition({ 136, 152 });
    createBlockAtPosition({ 144, 152 });
    createBlockAtPosition({ 152, 152 });
    createBlockAtPosition({ 160, 152 });
    createBlockAtPosition({ 160, 144 });
    createBlockAtPosition({ 168, 144 });
    createBlockAtPosition({ 184, 144 });
    createBlockAtPosition({ 176, 144 });
    createBlockAtPosition({ 192, 144 });
    createBlockAtPosition({ 200, 144 });
    createBlockAtPosition({ 216, 144 });
    createBlockAtPosition({ 208, 144 });
    createBlockAtPosition({ 224, 144 });
    createBlockAtPosition({ 232, 144 });
    createBlockAtPosition({ 240, 144 });
    createBlockAtPosition({ 248, 144 });
    createBlockAtPosition({ 256, 144 });
    createBlockAtPosition({ 264, 144 });
    createBlockAtPosition({ 272, 144 });
    createBlockAtPosition({ 280, 144 });
    createBlockAtPosition({ 288, 144 });
    createBlockAtPosition({ 296, 144 });
    createBlockAtPosition({ 304, 144 });
    createBlockAtPosition({ 312, 144 });
    createBlockAtPosition({ 320, 144 });
#pragma endregion
}

void ECSLevel::update()
{
    // TODO: remove, placeholder to place tiles
    /*
    if (wasMouseButtonPressedThisFrame(LEFT))
    {
        IVec2 closestGridPosition = { (int32_t)(s_mousePositionThisFrame.x / 8), (int32_t)(s_mousePositionThisFrame.y / 8) };
        IVec2 gridWorldPosition = { closestGridPosition.x * 8, closestGridPosition.y * 8 };
        createBlockAtPosition(gridWorldPosition);
    }
    */

    if (_wasKeyPressedThisFrame(SDL_SCANCODE_O))
    {
        D_LOG(LOG, "--------------");
        for (Entity& entity : getAllEntities())
        {
            if (entity.id == k_invalidId)
            {
                continue;
            }

            if (entityHasComponent<RectColliderComponent>(entity) && getComponentFromEntity<RectColliderComponent>(entity)->isLevelGeometry)
            {
                auto* t = getComponentFromEntity<TransformComponent>(entity);
                D_LOG(LOG, "createBlockAtPosition({ %i, %i});", (int)t->position.x, (int)t->position.y);
            }
        }
    }

    if (_wasKeyPressedThisFrame(SDL_SCANCODE_TAB))
    {
        s_canShowImGuiUI = !s_canShowImGuiUI;
    }

    Entity& player = getEntityById(k_playerEntityId);
    TransformComponent* playerTransform = getComponentFromEntity<TransformComponent>(player);

    // Light that follows player
    {
        Entity& lightThatFollowsPlayer = getEntityById(lightThatFollowsPlayerEntityId);

        auto* playerSprite = getComponentFromEntity<SpriteComponent>(player);
        SpriteComponent* lightSprite = getComponentFromEntity<SpriteComponent>(lightThatFollowsPlayer);
        auto* lightTransform = getComponentFromEntity<TransformComponent>(lightThatFollowsPlayer);

        Vec2 targetPos = { playerTransform->position.x - 5,
                           playerTransform->position.y - 15 };
        getComponentFromEntity<TransformComponent>(lightThatFollowsPlayer)->position = targetPos;
    }

	_savePositionSystem.update();
    overrideColliderOffsetsBasedOnCurrentSprite();
	_characterMovementSystem.update();
    _attackingSystem.update();
    _animationSystem.update();
    _crosshairSystem.update();

    // After all systems, update camera
    {
        _levelCamera.minX = -320;
        _levelCamera.maxX = 0;
        _levelCamera.followTargetRatio = 0.06f;
        _levelCamera.targetPosition = { playerTransform->position.x - (k_baseGameWidth / 2), 0 };
        _levelCamera.targetPosition.x = clamp(_levelCamera.targetPosition.x, _levelCamera.minX, _levelCamera.maxX);
        _levelCamera.position = lerp(_levelCamera.position, _levelCamera.targetPosition, _levelCamera.followTargetRatio);
    }
}

void ECSLevel::imguiRender()
{
    if (!s_canShowImGuiUI)
    {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

    ImGui::Begin("Player");

    ImGui::SetMouseCursor( ImGui::IsWindowHovered() ? ImGuiMouseCursor_Arrow : ImGuiMouseCursor_None);

    Entity& player = getEntityById(k_playerEntityId);

    auto* movement = getComponentFromEntity<MovementComponent>(player);
    auto* collider = getComponentFromEntity<RectColliderComponent>(player);

    ImGui::TextColored({ 255, 255, 0, 0.2 }, "Press [Tab] to close panel");

    ImGui::SeparatorText("Horizontal");

    ImGui::SliderFloat("Player Acceleration", &(movement->runAcceleration), 1, 30);
    ImGui::SliderFloat("Player Friction", &(movement->friction), 0.5f, 20.f);
    ImGui::SliderFloat("Player Max Horizontal Speed", &(movement->maxHorizontalSpeed), 0.1f, 10.f);
    ImGui::SliderFloat("Player Coyote time", &(movement->coyoteTime), 0.f, 1.f);

    ImGui::SeparatorText("Vertical");

    ImGui::SliderFloat("Player Max V Speed", &(movement->maxVerticalSpeed), 0.1f, 5.f);
    ImGui::SliderFloat("Player Jump Speed", &(movement->jumpSpeed), 0.1f, 5.f);
    ImGui::SliderFloat("Player Gravity", &(movement->gravity), 5.f, 20.f);
    ImGui::SliderFloat("Air friction", &(movement->airFriction), 1.f, 5.f);

    ImGui::SeparatorText("Misc");

    ImGui::Text("Character state: %s", getEntityStateAsString(player.entityState));

    ImGui::Text("Player is %s grounded", movement->isGrounded ? "" : "not");

    ImGui::Text("Player Collider X Offset: %i", collider->collider.topLeftPointOffset.x);
    ImGui::Text("Player Collider Y Offset: %i", collider->collider.topLeftPointOffset.y);

    ImGui::Text("Player timeSinceLeftPlatform: %f", movement->timeSinceLeftPlatform);
    ImGui::Text("Player Speed X: %f", getComponentFromEntity<MovementComponent>(player)->currentSpeed.x);
    ImGui::Text("Player Speed Y: %f", getComponentFromEntity<MovementComponent>(player)->currentSpeed.y);
    ImGui::Text("Player X: %f", getComponentFromEntity<TransformComponent>(player)->position.x);
    ImGui::Text("Player Y: %f", getComponentFromEntity<TransformComponent>(player)->position.y);

    ImGui::Checkbox("Debug colliders", &s_debugCollidersEnabled);
    ImGui::Checkbox("Debug grid", &s_debugGridEnabled);

    ImGui::Text("Average %.1f FPS", io.Framerate);
    ImGui::Text("V-sync is %s", s_vsyncEnabled ? "enabled" : "disabled");

    ImGui::End();

    Entity* testEnemy = nullptr;
    for (Entity& entity : getAllEntities())
    {
        if (entity.id == k_invalidId || entity.id == player.id || !entityHasComponent<AttackingComponent>(entity))
        {
            continue;
        }

        testEnemy = &entity;
        break;
    }

    if (!testEnemy)
    {
        return;
    }

    ImGui::Begin("Enemy");

    if (ImGui::GetMouseCursor() == ImGuiMouseCursor_None)
    {
        ImGui::SetMouseCursor(ImGui::IsWindowHovered() ? ImGuiMouseCursor_Arrow : ImGuiMouseCursor_None);
    }

    ImGui::Text("Enemy state: %s", getEntityStateAsString(testEnemy->entityState));

    ImGui::End();
}

void ECSLevel::render(float renderAlpha)
{
	_renderingSystem.render(renderAlpha);
	_debugCollidersSystem.render();
}