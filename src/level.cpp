#include "level.h"

#include "core/input.h"
#include "components.h"
#include "imgui.h"
#include <SDL3/SDL_render.h>

int lightThatFollowsPlayerEntityId = k_invalidId;

static bool s_isInsideRestaurant = false;

float k_restaurantBaseY = 60.f;

void createBlockAtPositionWithSize(Vec2 pos, IVec2 size)
{
    Entity& block = addEntity();
    addComponentToEntity<SpriteComponent>(block)->setupSpriteForLayer(TODO_TEMOVE_INVISIBLE_SPRITE, LEVEL_GEOMETRY_LAYER);
    getComponentFromEntity<TransformComponent>(block)->previousPosition = { (float)pos.x, (float)pos.y };
    getComponentFromEntity<TransformComponent>(block)->position = { (float)pos.x, (float)pos.y };
    addComponentToEntity<RectColliderComponent>(block)->collider = RectCollider({ 0, 0 }, { size.x, size.y });
    getComponentFromEntity<RectColliderComponent>(block)->isLevelGeometry = true;
}

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

void setupOutsideRestaurantScene()
{
    s_isInsideRestaurant = false;

    Entity& bg = addEntity();
    addComponentToEntity<SpriteComponent>(bg)->setupSpriteForLayer(TODO_REMOVE_BG_SPRITE, BEHIND_CHAR_LAYER);

    Entity& geometry = addEntity({ 0, 135 });
    addComponentToEntity<SpriteComponent>(geometry)->setupSpriteForLayer(TODO_REMOVE_LEVEL_GEOMETRY_SPRITE, LEVEL_GEOMETRY_LAYER);

    Entity& fg = addEntity({ 34, 129 });
    addComponentToEntity<SpriteComponent>(fg)->setupSpriteForLayer(TODO_REMOVE_FG_SPRITE, IN_FRONT_CHAR_LAYER);

    Entity& darwinLettersLight = addEntity();
    addComponentToEntity<SpriteComponent>(darwinLettersLight)->setupSpriteForLayer(ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    getComponentFromEntity<TransformComponent>(darwinLettersLight)->position = { 220, 55 };
    getComponentFromEntity<TransformComponent>(darwinLettersLight)->scale = { 1.5f, 0.4f };
    getComponentFromEntity<SpriteComponent>(darwinLettersLight)->color = { 200, 0, 0 , 255 };

    Entity& darwinLettersSmallGlow = addEntity();
    addComponentToEntity<SpriteComponent>(darwinLettersSmallGlow)->setupSpriteForLayer(ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    getComponentFromEntity<TransformComponent>(darwinLettersSmallGlow)->position = { 225, 20 };
    getComponentFromEntity<TransformComponent>(darwinLettersSmallGlow)->scale = { 1.35f, 2.f };
    getComponentFromEntity<SpriteComponent>(darwinLettersSmallGlow)->color = { 80, 0, 0 , 255 };

    Entity& streetLampLight = addEntity();
    addComponentToEntity<SpriteComponent>(streetLampLight)->setupSpriteForLayer(STREET_LAMP_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    getComponentFromEntity<TransformComponent>(streetLampLight)->position = { 24, 86 };
    getComponentFromEntity<TransformComponent>(streetLampLight)->scale = { 1.f, 1.f };
    getComponentFromEntity<SpriteComponent>(streetLampLight)->color = { 43, 15, 0 , 255 };

    Entity& streetLampGlow = addEntity();
    addComponentToEntity<SpriteComponent>(streetLampGlow)->setupSpriteForLayer(ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    getComponentFromEntity<TransformComponent>(streetLampGlow)->position = { 36, 88 };
    getComponentFromEntity<TransformComponent>(streetLampGlow)->scale = { 0.2f, 0.2f };
    getComponentFromEntity<SpriteComponent>(streetLampGlow)->color = { 125, 50, 0 , 200 };

    Entity& lightThatFollowsPlayer = addEntity();
    addComponentToEntity<SpriteComponent>(lightThatFollowsPlayer)->setupSpriteForLayer(BIG_ROUND_LIGHT_SPRITE, BACK_LIGHTS_LAYER);
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

void setupInsideRestaurantScene()
{
    s_isInsideRestaurant = true;

    {
        Entity& bgColor = addEntity();
        TransformComponent* t = getComponentFromEntity<TransformComponent>(bgColor);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(bgColor);
        s->setupSpriteForLayer(WHITE_ONE_BY_ONE_SPRITE, BEHIND_CHAR_LAYER);
        s->color = { 0, 0, 0, 255 };
        s->drawnAtScreenSpace = true;
        t->scale = { 322.f, 182.f };
        t->position = { 159, 179 };
    }

    {
        Entity& restaurant = addEntity({ 14, k_restaurantBaseY });
        addComponentToEntity<SpriteComponent>(restaurant)->setupSpriteForLayer(TODO_REMOVE_RESTAURANT_INTERIOR, BEHIND_CHAR_LAYER);
        createBlockAtPositionWithSize({ 24, k_restaurantBaseY + 83.f }, { 81, 11 });
        createBlockAtPositionWithSize({ 104, k_restaurantBaseY + 75.f }, { 488, 18 });
    }

    {
        Entity& ceilingLightLeft = addEntity();
        TransformComponent* t = getComponentFromEntity<TransformComponent>(ceilingLightLeft);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(ceilingLightLeft);
        s->setupSpriteForLayer(BIG_ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
        s->color = { 255, 167, 0, 18 };
        t->scale = { 1.f, 1.f };
        t->position = { 30, 55 };
    }

    {
        Entity& ceilingLightRight = addEntity();
        TransformComponent* t = getComponentFromEntity<TransformComponent>(ceilingLightRight);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(ceilingLightRight);
        s->setupSpriteForLayer(BIG_ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
        s->color = { 255, 167, 0, 17 };
        t->scale = { 1.f, 1.f };
        t->position = { 270, k_restaurantBaseY - 14.f };
    }

    {
        Entity& highlightCeilingLeft = addEntity();
        TransformComponent* t = getComponentFromEntity<TransformComponent>(highlightCeilingLeft);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(highlightCeilingLeft);
        s->setupSpriteForLayer(ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
        s->color = { 228, 228, 66, 40 };
        t->scale = { 0.65f, 0.15f };
        t->position = { 62, k_restaurantBaseY - 1.f };
    }

    {
        Entity& highlightCeilingRight = addEntity();
        TransformComponent* t = getComponentFromEntity<TransformComponent>(highlightCeilingRight);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(highlightCeilingRight);
        s->setupSpriteForLayer(ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
        s->color = { 228, 228, 66, 40 };
        t->scale = { 0.65f, 0.15f };
        t->position = { 305, k_restaurantBaseY + 1.f };
    }

    {
        Entity& exitLight = addEntity();
        TransformComponent* t = getComponentFromEntity<TransformComponent>(exitLight);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(exitLight);
        s->setupSpriteForLayer(ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
        s->color = { 0, 255, 50, 20 };
        t->scale = { 1.3f, 0.8f };
        t->position = { 10, k_restaurantBaseY + 26.f };
    }

    {
        Entity& tvLight = addEntity();
        TransformComponent* t = getComponentFromEntity<TransformComponent>(tvLight);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(tvLight);
        s->setupSpriteForLayer(ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
        s->color = { 255, 255, 255, 15 };
        t->scale = { 0.8f, 0.45f };
        t->position = { 106, k_restaurantBaseY + 29.f };
    }

    {
        Entity& brokenLight = addEntity();
        TransformComponent* t = getComponentFromEntity<TransformComponent>(brokenLight);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(brokenLight);
        s->setupSpriteForLayer(ROUND_LOW_QUALITY_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
        s->color = { 0, 240, 255, 37 };
        t->scale = { 1.f, 1.f };
        t->position = { 170, k_restaurantBaseY - 15.f };
    }

    {
        Entity& lampLight = addEntity();
        TransformComponent* t = getComponentFromEntity<TransformComponent>(lampLight);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(lampLight);
        s->setupSpriteForLayer(BIG_ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
        s->color = { 255, 77, 0, 25 };
        t->scale = { 0.47f, 0.47f };
        t->position = { 325, k_restaurantBaseY + 31.f };
    }

    {
        Entity& rightRoomLight = addEntity();
        TransformComponent* t = getComponentFromEntity<TransformComponent>(rightRoomLight);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(rightRoomLight);
        s->setupSpriteForLayer(ROUND_LOW_QUALITY_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
        s->color = { 215, 218, 143, 46 };
        t->scale = { 1.25f, 1.25f };
        t->position = { 463, k_restaurantBaseY + 16.f };
    }

    {
        Entity& rightRoomHighlight = addEntity();
        TransformComponent* t = getComponentFromEntity<TransformComponent>(rightRoomHighlight);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(rightRoomHighlight);
        s->setupSpriteForLayer(ROUND_LOW_QUALITY_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
        s->color = { 253, 204, 106, 40 };
        t->scale = { 0.25f, 0.25f };
        t->position = { 495 + 2, k_restaurantBaseY + 34.f };
    }

    {
        Entity& floor = addEntity();
        TransformComponent* t = getComponentFromEntity<TransformComponent>(floor);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(floor);
        s->setupSpriteForLayer(TODO_REMOVE_RESTAURANT_FLOOR_SPRITE, LEVEL_GEOMETRY_LAYER);
        t->position = { 24, k_restaurantBaseY + 75.f };
    }
}

void createDummyEntities(int amount)
{
    for (int i = 0; i < amount; ++i)
    {
        Entity& entity = addEntity();
        if (i == 0)
        {
            D_LOG(MINI, "Dummy Entities start at %i", entity.id);
        }
        if (i == amount - 1)
        {
            D_LOG(MINI, "Dummy Entities end at %i", entity.id);
        }
    }
}

void ECSLevel::start()
{
	_renderingSystem.createLightsBuffers();
    _uiSystem.start();

    Entity& player = addEntity({ 490, k_restaurantBaseY + 48.f });
    SpriteComponent* playerSprite = addComponentToEntity<SpriteComponent>(player);
    auto* movementComponent = addComponentToEntity<MovementComponent>(player);
    addComponentToEntity<AttackingComponent>(player)->weaponInHand = GOLF_WEAPON_TYPE;
    getComponentFromEntity<TransformComponent>(player)->useDynamicScale = true;
    player.entityState = ON_PHONE_STATE;

    Entity& crosshair = addEntity();
    auto* crosshairSprite = addComponentToEntity<SpriteComponent>(crosshair);
    crosshairSprite->setupSpriteForLayer(CROSSHAIR_MELEE_WEAPON_SPRITE, CROSSHAIR_LAYER);
    crosshairSprite->drawnAtScreenSpace = true;

    Entity& cellphone = addEntity({10, 200});
    auto* cellphoneSprite = addComponentToEntity<SpriteComponent>(cellphone);
    cellphoneSprite->setupSpriteForLayer(CELLPHONE_IDLE_SPRITE, CELLPHONE_LAYER);
    cellphoneSprite->drawnAtScreenSpace = true;

    playerSprite->setupSpriteForLayer(CHARACTER_IDLE_SPRITE, CHARACTER_LAYER);
    playerSprite->flipX = true;
    addComponentToEntity<RectColliderComponent>(player)->collider = RectCollider({ 4, 4 }, { 9, 17 });

    // Outside restaurant
    //setupOutsideRestaurantScene();

    // Inside restaurant
    setupInsideRestaurantScene();
    if (s_isInsideRestaurant)
    {
        _renderingSystem.setAmbientColor(138, 138, 138);
        _levelCamera.position = { 490.f, 90.f };
    }
}

void ECSLevel::update()
{
    // Debug
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
            s_isImGuiOpen = !s_isImGuiOpen;
        }
    }

    Entity& player = getEntityById(k_playerEntityId);
    TransformComponent* playerTransform = getComponentFromEntity<TransformComponent>(player);

    // Light that follows player
    if(lightThatFollowsPlayerEntityId != k_invalidId)
    {
        Entity& lightThatFollowsPlayer = getEntityById(lightThatFollowsPlayerEntityId);

        auto* playerSprite = getComponentFromEntity<SpriteComponent>(player);
        SpriteComponent* lightSprite = getComponentFromEntity<SpriteComponent>(lightThatFollowsPlayer);
        auto* lightTransform = getComponentFromEntity<TransformComponent>(lightThatFollowsPlayer);
  
        Vec2 targetPos = { playerTransform->position.x - 5,
                           playerTransform->position.y - 15 };
        getComponentFromEntity<TransformComponent>(lightThatFollowsPlayer)->position = targetPos;
    }

    // Update systems
    {
        _savePositionSystem.update();
        overrideColliderOffsetsBasedOnCurrentSprite();
        _characterMovementSystem.update();
        _attackingSystem.update();
        _animationSystem.update();
        _crosshairSystem.update();
        _uiSystem.update();
    }


    // Custom level logic
    {
        // Receive phone call
        static float cellphoneCallTutorialTimer = 0.f;
        if (isTimerOngoing(cellphoneCallTutorialTimer))
        {
            cellphoneCallTutorialTimer += k_deltaTime;

            float k_timeToShowCellphoneFirstTime = 1.f;
            if (cellphoneCallTutorialTimer >= k_timeToShowCellphoneFirstTime)
            {
                _uiSystem.receivePhoneCallAndPushDialogueOnAnswer(ROSTOV_MARKETING_PHONE_1);
                invalidateTimer(cellphoneCallTutorialTimer);
            }
        }

        if (_uiSystem.hasDialogueFinihsed(ROSTOV_MARKETING_PHONE_1))
        {
            _uiSystem.pushCellphoneDialogue(ETC_TEXT);
        }

        if (_uiSystem.hasDialogueFinihsed(ETC_TEXT))
        {
            _uiSystem.pushCellphoneDialogue(ROSTOV_MARKETING_PHONE_2, {ROSTOV_MARKETING_PHONE_2_1, ROSTOV_MARKETING_PHONE_2_2});
        }

        if (_uiSystem.hasChosenOption(ROSTOV_MARKETING_PHONE_2_2))
        {
            _uiSystem.pushCellphoneDialogue(ROSTOV_MARKETING_PHONE_3);
        }
        else if (_uiSystem.hasChosenOption(ROSTOV_MARKETING_PHONE_2_1))
        {
            _uiSystem.pushCellphoneDialogue(ROSTOV_MARKETING_PHONE_4, { ROSTOV_MARKETING_PHONE_4_1, ROSTOV_MARKETING_PHONE_4_2, ROSTOV_MARKETING_PHONE_4_3 });
        }

        if (_uiSystem.hasDialogueFinihsed(ROSTOV_MARKETING_PHONE_3))
        {
            _uiSystem.pushCellphoneDialogue(ROSTOV_MARKETING_PHONE_4, {ROSTOV_MARKETING_PHONE_4_1, ROSTOV_MARKETING_PHONE_4_2, ROSTOV_MARKETING_PHONE_4_3});
        }

        if (_uiSystem.hasChosenOption(ROSTOV_MARKETING_PHONE_4_1) || _uiSystem.hasChosenOption(ROSTOV_MARKETING_PHONE_4_2))
        {
            _uiSystem.pushCellphoneDialogue(ROSTOV_MARKETING_PHONE_5);
        }
        else if (_uiSystem.hasChosenOption(ROSTOV_MARKETING_PHONE_4_3))
        {
            _uiSystem.pushCellphoneDialogue(ROSTOV_MARKETING_PHONE_6);
        }

        if (_uiSystem.hasDialogueFinihsed(ROSTOV_MARKETING_PHONE_5))
        {
            _uiSystem.pushCellphoneDialogue(ROSTOV_MARKETING_PHONE_6);
        }

        if (_uiSystem.hasDialogueFinihsed(ROSTOV_MARKETING_PHONE_6))
        {
            _uiSystem.pushCellphoneDialogue(ROSTOV_MARKETING_PHONE_7);
        }

        if (_uiSystem.hasDialogueFinihsed(ROSTOV_MARKETING_PHONE_7))
        {
            _uiSystem.pushCellphoneDialogue(ROSTOV_MARKETING_PHONE_8);
        }

        if (_uiSystem.hasDialogueFinihsed(ROSTOV_MARKETING_PHONE_8))
        {
            _uiSystem.pushCellphoneDialogue(ROSTOV_MARKETING_PHONE_9, {ROSTOV_MARKETING_PHONE_9_1, ROSTOV_MARKETING_PHONE_9_2, ROSTOV_MARKETING_PHONE_9_3});
        }

        if (_uiSystem.hasChosenOption(ROSTOV_MARKETING_PHONE_9_2))
        {
            _uiSystem.pushCellphoneDialogue(ROSTOV_MARKETING_PHONE_11);
        }
        else if (_uiSystem.hasChosenOption(ROSTOV_MARKETING_PHONE_9_3))
        {
            _uiSystem.pushCellphoneDialogue(ROSTOV_MARKETING_PHONE_10);
        }

        if (_uiSystem.hasDialogueFinihsed(ROSTOV_MARKETING_PHONE_10) || _uiSystem.hasDialogueFinihsed(ROSTOV_MARKETING_PHONE_11))
        {
            _uiSystem.pushCellphoneDialogue(ROSTOV_MARKETING_PHONE_12);
        }

        if (_uiSystem.hasDialogueFinihsed(ROSTOV_MARKETING_PHONE_12))
        {
            _uiSystem.pushCellphoneDialogue(ROSTOV_MARKETING_PHONE_13, { ROSTOV_MARKETING_PHONE_13_1, ROSTOV_MARKETING_PHONE_13_2, ROSTOV_MARKETING_PHONE_13_3 });
        }
    }

    static float cameraOffsetXFromPlayer = 0.f;
    // After all systems, update camera
    {
        _levelCamera.minX = s_isInsideRestaurant ? 160 : -320;
        _levelCamera.maxX = s_isInsideRestaurant ? 540 : 0;
        _levelCamera.followTargetRatio = 0.06f;
        _levelCamera.targetPosition = { playerTransform->position.x + cameraOffsetXFromPlayer, 90.f };
        _levelCamera.targetPosition.x = clamp(_levelCamera.targetPosition.x, _levelCamera.minX, _levelCamera.maxX);

        if (abs(_levelCamera.targetPosition.x - _levelCamera.position.x) > 0.5f)
        {
            _levelCamera.position = lerp(_levelCamera.position, _levelCamera.targetPosition, _levelCamera.followTargetRatio);
        }
    }
}

void ECSLevel::render(float renderAlpha)
{
    _renderingSystem.render(renderAlpha);
    _uiSystem.render(&_renderingSystem);
    _debugCollidersSystem.render();
    _uiSystem.debugColliders();
    _renderingSystem.renderCrosshair(renderAlpha);
}

void ECSLevel::imguiRender()
{
    ImGui::SetMouseCursor(s_isImGuiOpen ? ImGuiMouseCursor_Arrow : ImGuiMouseCursor_None);

    // TODO: see if it causes issues. this will overlap crosshair alpha blending
    Entity& crosshair = getEntityById(k_crosshairEntityId);
    getComponentFromEntity<SpriteComponent>(crosshair)->color.a = s_isImGuiOpen ? 0 : CrosshairSystem::s_corsshairOpacity;

    if (!s_isImGuiOpen)
    {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

    ImGui::Begin("Player");

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

    Vec2 mouseWorldPosition = convertScreenPositionToCameraSpace(s_mousePositionThisFrameInScreenSpace);
    ImGui::Text("Mouse World X: %f", mouseWorldPosition.x);
    ImGui::Text("Mouse World Y: %f", mouseWorldPosition.y);
    ImGui::Text("Mouse Screen X: %f", s_mousePositionThisFrameInScreenSpace.x);
    ImGui::Text("Mouse Screen Y: %f", s_mousePositionThisFrameInScreenSpace.y);

    ImGui::Checkbox("Debug colliders", &s_debugCollidersEnabled);
    ImGui::Checkbox("Debug UI colliders", &s_debugUICollidersEnabled);
    ImGui::Checkbox("Debug grid", &s_debugGridEnabled);

    ImGui::Text("Average %.1f FPS", io.Framerate);
    ImGui::Text("V-sync is %s", s_vsyncEnabled ? "enabled" : "disabled");

    ImGui::SeparatorText("Levels");

    if (ImGui::ColorEdit3("Ambient Color", _renderingSystem._debugAmbientColorPicker, ImGuiColorEditFlags_NoInputs))
    {
        _renderingSystem.setAmbientColor(_renderingSystem._debugAmbientColorPicker[0] * 255,
                                         _renderingSystem._debugAmbientColorPicker[1] * 255,
                                         _renderingSystem._debugAmbientColorPicker[2] * 255);
    }
    
    ImGui::DragFloat("Camera Pos X", &_levelCamera.position.x);
    ImGui::DragFloat("Camera Pos Y", &_levelCamera.position.y);
    ImGui::DragFloat("Camera zoom", &_levelCamera.zoom, 0.01f, 1.f, 2.f);
    if (ImGui::Button("Reset camera"))
    {
        _levelCamera.position = { k_baseGameWidth / 2.f, k_baseGameHeight / 2.f };
        _levelCamera.zoom = 1.f;
    }

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

    ImGui::Text("Enemy state: %s", getEntityStateAsString(testEnemy->entityState));

    ImGui::End();
}
