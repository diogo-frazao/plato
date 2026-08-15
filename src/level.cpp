#include "level.h"

#include "core/input.h"
#include "components.h"
#include "debugUtils.h"
#include "imgui.h"
#include <SDL3/SDL_render.h>

int lightThatFollowsPlayerEntityId = k_invalidId;

static bool s_isInsideRestaurant = false;
static float s_multiPurpuseTimer = 0.f;
float k_restaurantBaseY = 60.f;

void createBlockAtPositionWithSize(Vec2 pos, IVec2 size)
{
    Entity& block = addEntity("colliderInvisibleBlock");
    addComponentToEntity<SpriteComponent>(block)->setupSpriteForLayer(TODO_TEMOVE_INVISIBLE_SPRITE, LEVEL_GEOMETRY_LAYER);
    getComponentFromEntity<TransformComponent>(block)->previousPosition = { (float)pos.x, (float)pos.y };
    getComponentFromEntity<TransformComponent>(block)->position = { (float)pos.x, (float)pos.y };
    addComponentToEntity<RectColliderComponent>(block)->collider = RectCollider({ 0, 0 }, { size.x, size.y });
    getComponentFromEntity<RectColliderComponent>(block)->isLevelGeometry = true;
}

void createBlockAtPosition(IVec2 position)
{
    Entity& block = addEntity("colliderInvisibleBlock");
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

    Entity& bg = addEntity("bg");
    addComponentToEntity<SpriteComponent>(bg)->setupSpriteForLayer(TODO_REMOVE_BG_SPRITE, BEHIND_LIGHTS_LAYER);

    Entity& geometry = addEntity("geometry", { 0, 135 });
    addComponentToEntity<SpriteComponent>(geometry)->setupSpriteForLayer(TODO_REMOVE_LEVEL_GEOMETRY_SPRITE, LEVEL_GEOMETRY_LAYER);

    Entity& fg = addEntity("fg", { 34, 129 });
    addComponentToEntity<SpriteComponent>(fg)->setupSpriteForLayer(TODO_REMOVE_FG_SPRITE, BAKED_HIGHLIGHTS_LAYER);

    Entity& darwinLettersLight = addEntity("darwinLettersLight");
    addComponentToEntity<SpriteComponent>(darwinLettersLight)->setupSpriteForLayer(ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    getComponentFromEntity<TransformComponent>(darwinLettersLight)->position = { 220, 55 };
    getComponentFromEntity<TransformComponent>(darwinLettersLight)->scale = { 1.5f, 0.4f };
    getComponentFromEntity<SpriteComponent>(darwinLettersLight)->color = { 200, 0, 0 , 255 };

    Entity& darwinLettersSmallGlow = addEntity("darwinLettersSmallGlow");
    addComponentToEntity<SpriteComponent>(darwinLettersSmallGlow)->setupSpriteForLayer(ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    getComponentFromEntity<TransformComponent>(darwinLettersSmallGlow)->position = { 225, 20 };
    getComponentFromEntity<TransformComponent>(darwinLettersSmallGlow)->scale = { 1.35f, 2.f };
    getComponentFromEntity<SpriteComponent>(darwinLettersSmallGlow)->color = { 80, 0, 0 , 255 };

    Entity& streetLampLight = addEntity("streetLampLight");
    addComponentToEntity<SpriteComponent>(streetLampLight)->setupSpriteForLayer(STREET_LAMP_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    getComponentFromEntity<TransformComponent>(streetLampLight)->position = { 24, 86 };
    getComponentFromEntity<TransformComponent>(streetLampLight)->scale = { 1.f, 1.f };
    getComponentFromEntity<SpriteComponent>(streetLampLight)->color = { 43, 15, 0 , 255 };

    Entity& streetLampGlow = addEntity("streetLampGlow");
    addComponentToEntity<SpriteComponent>(streetLampGlow)->setupSpriteForLayer(ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    getComponentFromEntity<TransformComponent>(streetLampGlow)->position = { 36, 88 };
    getComponentFromEntity<TransformComponent>(streetLampGlow)->scale = { 0.2f, 0.2f };
    getComponentFromEntity<SpriteComponent>(streetLampGlow)->color = { 125, 50, 0 , 200 };

    Entity& lightThatFollowsPlayer = addEntity("lightThatFollowsPlayer");
    addComponentToEntity<SpriteComponent>(lightThatFollowsPlayer)->setupSpriteForLayer(BIG_ROUND_LIGHT_SPRITE, BACK_LIGHTS_LAYER);
    getComponentFromEntity<TransformComponent>(lightThatFollowsPlayer)->position = { 36, 88 };
    getComponentFromEntity<TransformComponent>(lightThatFollowsPlayer)->scale = { 0.5f, 0.5f };
    getComponentFromEntity<SpriteComponent>(lightThatFollowsPlayer)->color = { 87, 69, 50, 42 };
    lightThatFollowsPlayerEntityId = lightThatFollowsPlayer.id;

    Entity& dummyEnemy = addEntity("dummyEnemy", { 160, 0 });
    addComponentToEntity<SpriteComponent>(dummyEnemy)->setupAnimationForLayer(GANGSTER_SMALL_IDLE_SPRITE, BAKED_HIGHLIGHTS_LAYER, true, 70, 900);
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
        Entity& bgColor = addEntity("bgColor");
        TransformComponent* t = getComponentFromEntity<TransformComponent>(bgColor);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(bgColor);
        s->setupSpriteForLayer(WHITE_ONE_BY_ONE_SPRITE, BEHIND_LIGHTS_LAYER);
        s->color = { 0, 0, 0, 255 };
        s->drawnAtScreenSpace = true;
        t->scale = { 322.f, 182.f };
        t->position = { 159, 179 };
    }

    {
        Entity& restaurant = addEntity("restaurant", { 14, k_restaurantBaseY });
        addComponentToEntity<SpriteComponent>(restaurant)->setupSpriteForLayer(TODO_REMOVE_RESTAURANT_INTERIOR, BEHIND_LIGHTS_LAYER);
        createBlockAtPositionWithSize({ 24, k_restaurantBaseY + 83.f }, { 81, 11 });
        createBlockAtPositionWithSize({ 104, k_restaurantBaseY + 75.f }, { 488, 18 });
    }

    {
        Entity& lightTopLeftB = addEntity("lightTopLeftB");
        auto* t = getComponentFromEntity<TransformComponent>(lightTopLeftB);
        t->position = { -0.88f, 78.00f };
        t->scale = { 1.37f, 0.70f };
        addComponentToEntity<SpriteComponent>(lightTopLeftB);
        auto* s = getComponentFromEntity<SpriteComponent>(lightTopLeftB);
        s->color = { 255, 184, 107, 15 };
        s->setupSpriteForLayer(BIG_ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    }

    {
        Entity& lightTopLeftM = addEntity("lightTopLeftM");
        auto* t = getComponentFromEntity<TransformComponent>(lightTopLeftM);
        t->position = { 36.95f, 76.35f };
        t->scale = { 0.80f, 0.70f };
        addComponentToEntity<SpriteComponent>(lightTopLeftM);
        auto* s = getComponentFromEntity<SpriteComponent>(lightTopLeftM);
        s->color = { 255, 133, 0, 12 };
        s->setupSpriteForLayer(BIG_ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    }

    {
        Entity& lightTopLeftS = addEntity("lightTopLeftS");
        auto* t = getComponentFromEntity<TransformComponent>(lightTopLeftS);
        t->position = { 53.09f, 65.91f };
        t->scale = { 0.55f, 0.80f };
        addComponentToEntity<SpriteComponent>(lightTopLeftS);
        auto* s = getComponentFromEntity<SpriteComponent>(lightTopLeftS);
        s->color = { 255, 173, 84, 10 };
        s->setupSpriteForLayer(BIG_ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    }

    {
        Entity& tvLight = addEntity("tvLight");
        TransformComponent* t = getComponentFromEntity<TransformComponent>(tvLight);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(tvLight);
        s->setupSpriteForLayer(ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
        s->color = { 255, 255, 255, 5 };
        t->scale = { 0.8f, 0.45f };
        t->position = { 106, k_restaurantBaseY + 29.f };
    }

    {
        Entity& brokenLight = addEntity("brokenLight");
        TransformComponent* t = getComponentFromEntity<TransformComponent>(brokenLight);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(brokenLight);
        s->setupSpriteForLayer(ROUND_LOW_QUALITY_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
        s->color = { 0, 240, 255, 37 };
        t->scale = { 1.f, 1.f };
        t->position = { 170, k_restaurantBaseY - 15.f };
    }

    {
        Entity& lightTopRightB = addEntity("lightTopRightB");
        auto* t = getComponentFromEntity<TransformComponent>(lightTopRightB);
        t->position = { 257.f, 78.00f };
        t->scale = { 1.37f, 0.70f };
        addComponentToEntity<SpriteComponent>(lightTopRightB);
        auto* s = getComponentFromEntity<SpriteComponent>(lightTopRightB);
        s->color = { 255, 184, 107, 15 };
        s->setupSpriteForLayer(BIG_ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    }

    {
        Entity& lightTopRightM = addEntity("lightTopRightM");
        auto* t = getComponentFromEntity<TransformComponent>(lightTopRightM);
        t->position = { 290.f, 75.19f };
        t->scale = { 0.80f, 0.70f };
        addComponentToEntity<SpriteComponent>(lightTopRightM);
        auto* s = getComponentFromEntity<SpriteComponent>(lightTopRightM);
        s->color = { 255, 133, 0, 12 };
        s->setupSpriteForLayer(BIG_ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    }

    {
        Entity& lightTopRightS = addEntity("lightTopRightS");
        auto* t = getComponentFromEntity<TransformComponent>(lightTopRightS);
        t->position = { 304.f, 65.91f };
        t->scale = { 0.55f, 0.80f };
        addComponentToEntity<SpriteComponent>(lightTopRightS);
        auto* s = getComponentFromEntity<SpriteComponent>(lightTopRightS);
        s->color = { 255, 173, 84, 10 };
        s->setupSpriteForLayer(BIG_ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    }

    {
        Entity& lightHighlightLeft = addEntity("lightHighlightLeft");
        auto* t = getComponentFromEntity<TransformComponent>(lightHighlightLeft);
        t->position = { 56.f, 77.f };
        addComponentToEntity<SpriteComponent>(lightHighlightLeft);
        auto* s = getComponentFromEntity<SpriteComponent>(lightHighlightLeft);
        s->color = { 255, 211, 131, 148 };
        s->setupSpriteForLayer(LONG_THIN_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    }

    {
        Entity& lightHighlightRight = addEntity("lightHighlightRight");
        auto* t = getComponentFromEntity<TransformComponent>(lightHighlightRight);
        t->position = { 311.f, 78.f };
        auto* s = addComponentToEntity<SpriteComponent>(lightHighlightRight);
        s->color = { 255, 211, 131, 148 };
        s->setupSpriteForLayer(LONG_THIN_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
    }

    {
        Entity& bakedHighlights = addEntity("bakedHighlights");
        addComponentToEntity<SpriteComponent>(bakedHighlights);
        auto* t = getComponentFromEntity<TransformComponent>(bakedHighlights);
        t->position = { 58.00f, 65.00f };
        t->scale = { 1.00f, 1.00f };
        auto* s = getComponentFromEntity<SpriteComponent>(bakedHighlights);
        s->color = { 255, 255, 255, 180 };
        s->setupSpriteForLayer(RESTAURANT_BAKED_HIGHLIGHTS_SPRITE, BAKED_HIGHLIGHTS_LAYER);
    }

    {
        Entity& floor = addEntity("floor");
        TransformComponent* t = getComponentFromEntity<TransformComponent>(floor);
        SpriteComponent* s = addComponentToEntity<SpriteComponent>(floor);
        s->setupSpriteForLayer(TODO_REMOVE_RESTAURANT_FLOOR_SPRITE, LEVEL_GEOMETRY_LAYER);
        t->position = { 24, k_restaurantBaseY + 75.f };
    }

    {
        Entity& darwin = addEntity("darwin", { 415.f, 117.f });
        addComponentToEntity<SpriteComponent>(darwin)->setupSpriteForLayer(DARWIN_PLACEHOLDER_SPRITE, CHARACTERS_LAYER);
        addComponentToEntity<RectColliderComponent>(darwin)->collider = RectCollider({ 0,0 }, { 13, 18 });

        auto* darwinM = addComponentToEntity<MovementComponent>(darwin);
        darwinM->maxHorizontalSpeed = 0.3f;

        SpriteType darwinAnimations[] = { DARWIN_PLACEHOLDER_SPRITE };
        darwinM->setupMovementAnimations(darwinAnimations);

        s_darwinEntityId = darwin.id;
    }

    {
        Entity& hugo = addEntity("hugo", { 179.f, 106.f });
        addComponentToEntity<SpriteComponent>(hugo)->setupAnimationForLayer(GANGSTER_SMALL_IDLE_SPRITE, CHARACTERS_LAYER, true, 70, 900);
        addComponentToEntity<RectColliderComponent>(hugo)->collider = RectCollider({ 4, 4 }, { 9, 17 });
        // We can't attack hugo until oskar dies
        addComponentToEntity<AttackingComponent>(hugo)->canBeAttacked = false;
        auto* hugoM = addComponentToEntity<MovementComponent>(hugo);
        hugoM->maxHorizontalSpeed = 0.3f;
        s_hugoEntityId = hugo.id;

        SpriteType hugoAnimations[] = { GANGSTER_SMALL_IDLE_SPRITE };
        hugoM->setupMovementAnimations(hugoAnimations);
    }

    {
        Entity& oskar = addEntity("oskar", { 242.f, 95.f });
        addComponentToEntity<SpriteComponent>(oskar)->setupAnimationForLayer(GANGSTER_SMALL_IDLE_SPRITE, CHARACTERS_LAYER, true, 70, 900);
        addComponentToEntity<RectColliderComponent>(oskar)->collider = RectCollider({ 4, 4 }, { 9, 17 });
        addComponentToEntity<AttackingComponent>(oskar);
        getComponentFromEntity<TransformComponent>(oskar)->useDynamicScale = true;
        auto* oskarM = addComponentToEntity<MovementComponent>(oskar);
        oskarM->maxHorizontalSpeed = 0.3f;
        s_oskarEntityId = oskar.id;

        SpriteType oskarAnimations[] = { GANGSTER_SMALL_IDLE_SPRITE };
        oskarM->setupMovementAnimations(oskarAnimations);
    }

    Entity& golfWeapon = addEntity("golfWeapon");
    addComponentToEntity<SpriteComponent>(golfWeapon)->setupSpriteForLayer(GOLF_WEAPON_SPRITE, BEHIND_LIGHTS_LAYER);
    addComponentToEntity<TransformComponent>(golfWeapon)->position = { 383.f, 124.f };
}   

void createDummyEntities(int amount)
{
    for (int i = 0; i < amount; ++i)
    {
        Entity& entity = addEntity("dummyEntity");
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

void Level::start()
{
	s_renderingSystem.createLightsBuffers();
    s_renderingSystem.createInFrontOfEverythingBuffer();
    s_uiSystem.start();

    Entity& player = addEntity("player", { 490.f, k_restaurantBaseY + 48.f });
    SpriteComponent* playerSprite = addComponentToEntity<SpriteComponent>(player);
    auto* movementComponent = addComponentToEntity<MovementComponent>(player);
    addComponentToEntity<AttackingComponent>(player)->weaponInHand;
    getComponentFromEntity<TransformComponent>(player)->useDynamicScale = true;
    player.entityState = ON_CUTSCENE_STATE;

    Entity& crosshair = addEntity("crosshair");
    auto* crosshairSprite = addComponentToEntity<SpriteComponent>(crosshair);
    crosshairSprite->setupSpriteForLayer(CROSSHAIR_MELEE_WEAPON_SPRITE, CROSSHAIR_LAYER);
    crosshairSprite->drawnAtScreenSpace = true;

    Entity& cellphone = addEntity("cellphone", {10, 200});
    auto* cellphoneSprite = addComponentToEntity<SpriteComponent>(cellphone);
    cellphoneSprite->setupSpriteForLayer(CELLPHONE_IDLE_SPRITE, UI_LAYER);
    cellphoneSprite->drawnAtScreenSpace = true;

    Entity& playerEffects = addEntity("playerEffects");
    addComponentToEntity<SpriteComponent>(playerEffects)->setLayer(UI_LAYER);

    // We don't need to set the sprite since the player's sprite is handled on the movement system
    playerSprite->setLayer(CHARACTERS_LAYER);
    playerSprite->flipX = true;
    addComponentToEntity<RectColliderComponent>(player)->collider = RectCollider({ 4, 4 }, { 9, 17 });

    // Outside restaurant
    //setupOutsideRestaurantScene();

    // Inside restaurant
    setupInsideRestaurantScene();
    if (s_isInsideRestaurant)
    {
        s_renderingSystem.setTargetAmbientColor(64, 64, 64);
        _levelCamera.position = { 510.f, 90.f };
    }

    _currentLevelStage = FREE_STAGE;
    getComponentFromEntity<AttackingComponent>(player)->weaponInHand = ROSTOV_WEAPON_PISTOL_TYPE;
    player.entityState = IDLE_STATE;
}

bool moveEntityUntilXPosition(TransformComponent* t, MovementComponent* m, SpriteComponent* s, float targetXPosition)
{
    bool shouldMoveLeft = targetXPosition < t->position.x;

    if (abs(t->position.x - targetXPosition) < 1.f)
    {
        m->isMovingOnFloor = false;
        return true;
    }
    
    m->currentSpeed.x = shouldMoveLeft ? m->maxHorizontalSpeed * -1.f : m->maxHorizontalSpeed;
    s->flipX = shouldMoveLeft;
    m->isMovingOnFloor = true;
    return false;
}

void entityLookAtAnother(Entity* source, Entity* target)
{
    float sourceXPos = getComponentFromEntity<TransformComponent>(*source)->position.x;
    float targetXPos = getComponentFromEntity<TransformComponent>(*target)->position.x;

    SpriteComponent* sourceS = getComponentFromEntity<SpriteComponent>(*source);

    bool shouldLookRight = (targetXPos > sourceXPos) && sourceS->flipX;
    bool shouldLookLeft = (sourceXPos > targetXPos) && !sourceS->flipX;
    bool shouldLookAtSprite = shouldLookRight || shouldLookLeft;

    if (!shouldLookAtSprite)
    {
        return;
    }

    sourceS->flipX = !sourceS->flipX;
}

void pushGolfCueControntationDialogue(UISystem& u)
{
    Entity& player = getEntityById(k_playerEntityId);
    auto* a = getComponentFromEntity<AttackingComponent>(player);

    Entity& oskar = getEntityById(s_oskarEntityId);
    if (a->weaponInHand == NO_WEAPON_TYPE)
    {
        u.pushEntityDialogue(C_4_BC_3_C_1);
    }
    else
    {
        u.pushEntityDialogue(C_4_BC_3_C_CUE_1);
    }
}

void finishHugoConversation(UISystem& u, Entity& player, Entity& hugo, bool& canHugoProvokeDarwin)
{
    u.popTensionBar();
    player.entityState = IDLE_STATE;
    getComponentFromEntity<AttackingComponent>(hugo)->canBeAttacked = true;
    canHugoProvokeDarwin = true;
}

void handleCameraShake(Camera& camera)
{
    if (camera.cameraShakeToPerform == NO_SHAKE)
    {
        return;
    }

    if (isTimerOngoing(camera.delayToShakeTimer))
    {
        camera.delayToShakeTimer += k_deltaTime;
        if (camera.delayToShakeTimer >= camera.delayToShake)
        {
            camera.delayToShake = 0.f;
            invalidateTimer(camera.delayToShakeTimer);
        }

        return;
    }

    switch (camera.cameraShakeToPerform)
    {
    case NO_SHAKE:
        break;
    case LIGHT_SHAKE:
        camera.targetPosition.x += 5.f;
        camera.targetPosition.y += 10.f;
        break;
    case MEDIUM_SHAKE:
        camera.targetPosition.x += 20.f;
        camera.targetPosition.y += 40.f;
        break;
    }
    camera.cameraShakeToPerform = NO_SHAKE;
}

void Level::update()
{
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

    // Update save position system
    {
        s_savePositionSystem.update();
    }

    UISystem& u = s_uiSystem;

    // Level logic
    switch (_currentLevelStage)
    {
        case MARKETING_PHONE_STAGE:
        {
            // Receive phone call
            if (isTimerOngoing(s_multiPurpuseTimer))
            {
                s_multiPurpuseTimer += k_deltaTime;

                float k_timeToShowCellphoneFirstTime = 1.f;
                if (s_multiPurpuseTimer >= k_timeToShowCellphoneFirstTime)
                {
                    s_uiSystem.receivePhoneCallAndPushDialogueOnAnswer(MARKETING_PHONE_1);
                    invalidateTimer(s_multiPurpuseTimer);
                }
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_1))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_2, { MARKETING_PHONE_2_A, MARKETING_PHONE_2_B, MARKETING_PHONE_2_C });
            }

            if (s_uiSystem.hasChosenOption(MARKETING_PHONE_2_A))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_2_A_1);
            }
            else if (s_uiSystem.hasChosenOption(MARKETING_PHONE_2_B))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_2_B_1);
            }
            else if (s_uiSystem.hasChosenOption(MARKETING_PHONE_2_C))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_2_C_1);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_2_C_1))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_2_C_2);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_2_C_2))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_2_C_3);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_2_C_3))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_2_C_4, { MARKETING_PHONE_2_C_4_A, MARKETING_PHONE_2_C_4_B });
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_2_B_1))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_2_B_2);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_2_A_1) || s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_2_B_2))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_3, { MARKETING_PHONE_3_A, MARKETING_PHONE_3_B });
            }

            if (s_uiSystem.hasChosenOption(MARKETING_PHONE_3_A))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_3_A_1);
            }
            else if (s_uiSystem.hasChosenOption(MARKETING_PHONE_3_B))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_3_B_1);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_3_B_1))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_3_B_2);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_3_A_1) || s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_3_B_2))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_4);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_4))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_5);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_5))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_6);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_6))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_7, { MARKETING_PHONE_7_A,
                    MARKETING_PHONE_7_B, s_playerTension >= 30 ? MARKETING_PHONE_7_C_HIGH_TENSION : MARKETING_PHONE_7_C_LOW_TENSION });
            }

            if (s_uiSystem.hasChosenOption(MARKETING_PHONE_7_A))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_7_A_1);
            }
            else if (s_uiSystem.hasChosenOption(MARKETING_PHONE_7_B))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_11_B_1);
            }
            else if (s_uiSystem.hasChosenOption(MARKETING_PHONE_7_C_LOW_TENSION))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_8);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_7_A_1))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_8);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_8))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_9);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_9))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_10);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_10))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_11, { MARKETING_PHONE_11_A,
                    s_playerTension >= 50 ? MARKETING_PHONE_11_B_HIGH_TENSION : MARKETING_PHONE_11_B_LOW_TENSION });
            }

            if (s_uiSystem.hasChosenOption(MARKETING_PHONE_11_A))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_11_A_1);
            }
            else if (s_uiSystem.hasChosenOption(MARKETING_PHONE_11_B_LOW_TENSION))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_11_B_1);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_11_A_1) || s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_11_B_1))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_12);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_12))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_13);
            }

            if (s_uiSystem.hasDialogueFinihsed(MARKETING_PHONE_13))
            {
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_14, { MARKETING_PHONE_14_A, MARKETING_PHONE_14_B });
            }

            if (s_uiSystem.hasChosenOption(MARKETING_PHONE_2_C_4_A) || s_uiSystem.hasChosenOption(MARKETING_PHONE_2_C_4_B) ||
                s_uiSystem.hasChosenOption(MARKETING_PHONE_7_C_HIGH_TENSION) || s_uiSystem.hasChosenOption(MARKETING_PHONE_11_B_HIGH_TENSION) ||
                s_uiSystem.hasChosenOption(MARKETING_PHONE_14_A) || s_uiSystem.hasChosenOption(MARKETING_PHONE_14_B))
            {
                s_uiSystem.hangupPhone();
                _currentLevelStage = FIRST_DAD_PHONE_STAGE;
                startTimer(s_multiPurpuseTimer);
            }

            break;
        }

        case FIRST_DAD_PHONE_STAGE:
        {
            // After marketing dialogue, wait x seconds and receive dad call
            if (isTimerOngoing(s_multiPurpuseTimer))
            {
                s_multiPurpuseTimer += k_deltaTime;

                if (s_multiPurpuseTimer >= 5.f)
                {
                    s_uiSystem.receivePhoneCallAndPushDialogueOnAnswer(ONE_DAD_PHONE_1);
                    invalidateTimer(s_multiPurpuseTimer);
                }
            }

            if (u.hasDialogueFinihsed(ONE_DAD_PHONE_1))
            {
                u.pushCellphoneDialogue(ONE_DAD_PHONE_2, { ONE_DAD_PHONE_2_A, ONE_DAD_PHONE_2_B});
            }

            if (u.hasDialogueFinihsed(ONE_DAD_PHONE_2))
            {
                u.pushCellphoneDialogue(ONE_DAD_PHONE_3, { ONE_DAD_PHONE_3_A, ONE_DAD_PHONE_3_B });
            }

            if (u.hasChosenOption(ONE_DAD_PHONE_3_A))
            {
                u.pushCellphoneDialogue(ONE_DAD_PHONE_3_A_1);
            }

            if (u.hasDialogueFinihsed(ONE_DAD_PHONE_3_A_1))
            {
                u.pushCellphoneDialogue(ONE_DAD_PHONE_3_A_2, { ONE_DAD_PHONE_4_A , ONE_DAD_PHONE_4_B, ONE_DAD_PHONE_4_C });
            }

            if (u.hasChosenOption(ONE_DAD_PHONE_3_B))
            {
                u.pushCellphoneDialogue(ONE_DAD_PHONE_3_B_1);
            }

            if (u.hasDialogueFinihsed(ONE_DAD_PHONE_3_B_1))
            {
                u.pushCellphoneDialogue(ONE_DAD_PHONE_3_B_2);
            }

            if (u.hasDialogueFinihsed(ONE_DAD_PHONE_3_B_2))
            {
                u.pushCellphoneDialogue(ONE_DAD_PHONE_3_B_3, { ONE_DAD_PHONE_3_B_3_A , ONE_DAD_PHONE_3_B_3_B });
            }

            if (u.hasDialogueFinihsed(ONE_DAD_PHONE_3_B_3))
            {
                u.pushCellphoneDialogue(ONE_DAD_PHONE_3_B_4);
            }

            if (u.hasDialogueFinihsed(ONE_DAD_PHONE_3_B_4))
            {
                u.pushCellphoneDialogue(ONE_DAD_PHONE_4, { ONE_DAD_PHONE_4_A , ONE_DAD_PHONE_4_B, ONE_DAD_PHONE_4_C });
            }

            if (u.hasChosenOption(ONE_DAD_PHONE_4_A) || u.hasChosenOption(ONE_DAD_PHONE_4_B) || u.hasChosenOption(ONE_DAD_PHONE_4_C))
            {
                u.pushCellphoneDialogue(ONE_DAD_PHONE_5);
            }

            if (u.hasDialogueFinihsed(ONE_DAD_PHONE_5))
            {
                u.pushCellphoneDialogue(ONE_DAD_PHONE_6, { ONE_DAD_PHONE_6_A , ONE_DAD_PHONE_6_B });
            }

            if (u.hasDialogueFinihsed(ONE_DAD_PHONE_6))
            {
                u.pushCellphoneDialogue(ONE_DAD_PHONE_7);
            }

            if (u.hasDialogueFinihsed(ONE_DAD_PHONE_7))
            {
                u.pushCellphoneDialogue(ONE_DAD_PHONE_8);
            }

            if (u.hasDialogueFinihsed(ONE_DAD_PHONE_8))
            {
                u.pushCellphoneDialogue(ONE_DAD_PHONE_9);
                startTimer(s_multiPurpuseTimer);

            }

            // Darwin enters and interrupts father dialogue
            if (u._currentDialogue.dialogueType == ONE_DAD_PHONE_9 && isTimerOngoing(s_multiPurpuseTimer))
            {
                s_multiPurpuseTimer += k_deltaTime;
                if (s_multiPurpuseTimer >= 3.1f)
                {
                    u.interruptCurrentDialogue();
                    _currentLevelStage = DARWIN_CONVERSATION_STAGE;
                    invalidateTimer(s_multiPurpuseTimer);
                }
            }

            break;
        }

        case DARWIN_CONVERSATION_STAGE:
        {
            Entity& darwin = getEntityById(s_darwinEntityId);
            auto* darwinT = getComponentFromEntity<TransformComponent>(darwin);
            auto* darwinS = getComponentFromEntity<SpriteComponent>(darwin);
            auto* darwinM = getComponentFromEntity<MovementComponent>(darwin);

            if (u.hasDialogueFinishedInterrupting(ONE_DAD_PHONE_9))
            {
                u.pushEntityDialogue(ONE_DARWIN_1);
                _darwinConversationStageData.canMoveFromDoor = true;
            }

            if (_darwinConversationStageData.canMoveFromDoor)
            {
                if (moveEntityUntilXPosition(darwinT, darwinM, darwinS, 427.f))
                {
                    _darwinConversationStageData.canMoveFromDoor = false;
                }
            }

            if (u.hasDialogueFinihsed(ONE_DARWIN_1))
            {
                u.pushEntityDialogue(ONE_DARWIN_2, { ONE_DARWIN_2_A, ONE_DARWIN_2_B });
            }

            if (u.hasDialogueFinihsed(ONE_DARWIN_2))
            {
                u.pushCellphoneDialogue(ONE_DARWIN_3);
                _darwinConversationStageData.canGetNearTable = true;
            }

            if (u.hasDialogueFinihsed(ONE_DARWIN_3))
            {
                u.pushCellphoneDialogue(ONE_DARWIN_5);
            }

            if (_darwinConversationStageData.canGetNearTable)
            {
                if (moveEntityUntilXPosition(darwinT, darwinM, darwinS, 440.f))
                {
                    _darwinConversationStageData.canGetNearTable = false;
                }
            }

            /*if (u.hasDialogueFinihsed(ONE_DARWIN_4))
            {
                u.pushCellphoneDialogue(ONE_DARWIN_5);
            }*/

            if (u.hasDialogueFinihsed(ONE_DARWIN_5))
            {
                u.pushEntityDialogue(ONE_DARWIN_6);
            }

            if (u.hasDialogueFinihsed(ONE_DARWIN_6))
            {
                u.pushEntityDialogue(ONE_DARWIN_7);
                _darwinConversationStageData.canGetEvenNearTable = true;
            }

            if (_darwinConversationStageData.canGetEvenNearTable)
            {
                if (moveEntityUntilXPosition(darwinT, darwinM, darwinS, 480.f))
                {
                    _darwinConversationStageData.canGetEvenNearTable = false;
                }
            }

            if (u.hasDialogueFinihsed(ONE_DARWIN_7))
            {
                u.pushEntityDialogue(ONE_DARWIN_8);
            }

            if (u.hasDialogueFinihsed(ONE_DARWIN_8))
            {
                u.pushEntityDialogue(ONE_DARWIN_9);
            }

            if (u.hasDialogueFinihsed(ONE_DARWIN_9))
            {
                startTimer(_darwinConversationStageData.waitToAskIfPaHeardUs);
            }

            if (isTimerOngoing(_darwinConversationStageData.waitToAskIfPaHeardUs))
            {
                _darwinConversationStageData.waitToAskIfPaHeardUs += k_deltaTime;
                if (_darwinConversationStageData.waitToAskIfPaHeardUs >= 2.f)
                {
                    u.pushEntityDialogue(ONE_DARWIN_10, { ONE_DARWIN_10_A, ONE_DARWIN_10_B, ONE_DARWIN_10_C });
                    invalidateTimer(_darwinConversationStageData.waitToAskIfPaHeardUs);
                }
            }

            if (u.hasDialogueFinihsed(ONE_DARWIN_10))
            {
                Vec2 darwinPos = getComponentFromEntity<TransformComponent>(darwin)->position;
                u.pushCellphoneDialogue(ONE_DARWIN_11);
            }

            if (u.hasDialogueFinihsed(ONE_DARWIN_11))
            {
                u.hangupPhone();
                startTimer(_darwinConversationStageData.waitAfterCallEndsTimer);
            }

            if (isTimerOngoing(_darwinConversationStageData.waitAfterCallEndsTimer))
            {
                _darwinConversationStageData.waitAfterCallEndsTimer += k_deltaTime;
                if (_darwinConversationStageData.waitAfterCallEndsTimer >= 2.f)
                {
                    u.pushEntityDialogue(ONE_DARWIN_12);
                    invalidateTimer(_darwinConversationStageData.waitAfterCallEndsTimer);
                }
            }

            if (u.hasDialogueFinihsed(ONE_DARWIN_12))
            {
                u.pushEntityDialogue(ONE_DARWIN_13, { ONE_DARWIN_13_A, ONE_DARWIN_13_B, ONE_DARWIN_13_C });
            }

            if (u.hasDialogueFinihsed(ONE_DARWIN_13))
            {
                u.pushEntityDialogue(ONE_DARWIN_14);
            }

            if (u.hasDialogueFinihsed(ONE_DARWIN_14))
            {
                _darwinConversationStageData.canMoveBack = true;
            }

            if (_darwinConversationStageData.canMoveBack)
            {
                if (moveEntityUntilXPosition(darwinT, darwinM, darwinS, 440.f))
                {
                    u.pushEntityDialogue(ONE_DARWIN_15);
                    _darwinConversationStageData.canMoveBack = false;
                }
            }

            if (u.hasDialogueFinihsed(ONE_DARWIN_15))
            {
                u.popTensionBar();
                _currentLevelStage = GANGSTER_CONFRONTATION_STAGE;
                player.entityState = IDLE_STATE;
            }

            break;
        }

        case GANGSTER_CONFRONTATION_STAGE:
        {
            Entity& darwin = getEntityById(s_darwinEntityId);
            Entity& hugo = getEntityById(s_hugoEntityId);
            auto* hugoT = getComponentFromEntity<TransformComponent>(hugo);

            Entity& oskar = getEntityById(s_oskarEntityId);

            // Move darwin close to gangsters
            auto* darwinT = getComponentFromEntity<TransformComponent>(darwin);
            auto* darwinS = getComponentFromEntity<SpriteComponent>(darwin);
            auto* darwinM = getComponentFromEntity<MovementComponent>(darwin);
            if (_gangsterConfrontationStageData.canDarwinMoveToKitchen)
            {
                darwinM->maxHorizontalSpeed = 0.6f;
                if (moveEntityUntilXPosition(darwinT, darwinM, darwinS, 293.f))
                {
                    darwinM->maxHorizontalSpeed = 0.3f;
                    _gangsterConfrontationStageData.canDarwinMoveToKitchen = false;
                }
            }

            // As soon as we move, push the dialogue
            if (playerTransform->position.x < 488.f && !_gangsterConfrontationStageData.hasStartedConfrontationDialogue)
            {
                u.pushEntityDialogue(C_1);
                _gangsterConfrontationStageData.hasStartedConfrontationDialogue = true;
                D_LOG(MINI, "Started confrontation dialogue");
            }

            if (u.hasDialogueFinihsed(C_1))
            {
                u.pushEntityDialogue(C_2);
            }

            if (u.hasDialogueFinihsed(C_2))
            {
                u.pushEntityDialogue(C_3);
            }

            // Tell rostov to not get involved when he gets near
            if (playerTransform->position.x <= 315.f && !_gangsterConfrontationStageData.hasToldRostovToNotGetInvolved)
            {
                u.pushTensionBar();
                u.pushEntityDialogue(C_4, {C_4_A, C_4_B, C_4_C });
                _gangsterConfrontationStageData.hasToldRostovToNotGetInvolved = true;
                player.entityState = ON_CUTSCENE_STATE;
            }

            if (u.hasChosenOption(C_4_B) || u.hasChosenOption(C_4_C))
            {
                u.pushEntityDialogue(C_4_BC_1);
            }

            if (u.hasDialogueFinihsed(C_4_BC_1))
            {
                u.pushEntityDialogue(C_4_BC_2);
            }

            if (u.hasDialogueFinihsed(C_4_BC_2))
            {
                _gangsterConfrontationStageData.canOskarMoveClose = true;
            }

            if (u.hasChosenOption(C_4_BC_3_A))
            {
                u.pushEntityDialogue(C_4_A_5);
            }
            else if (u.hasChosenOption(C_4_BC_3_B))
            {
                u.pushEntityDialogue(C_4_BC_2_3_1);
            }
            else if (u.hasChosenOption(C_4_BC_3_C))
            {
                u.popTensionBar();
                player.entityState = IDLE_STATE;

                pushGolfCueControntationDialogue(u);
            }

            if (u.hasDialogueFinihsed(C_4_BC_2_3_1))
            {
                u.pushEntityDialogue(C_4_BC_2_3_2);
            }

            if (u.hasDialogueFinihsed(C_4_BC_2_3_2))
            {
                u.pushEntityDialogue(C_4_BC_2_3_3);
            }

            if (u.hasDialogueFinihsed(C_4_BC_2_3_3))
            {
                u.pushEntityDialogue(C_4_A_6_AB_3);
            }

            if (u.hasChosenOption(C_4_A))
            {
                u.pushEntityDialogue(C_4_A_1);
            }

            if (u.hasDialogueFinihsed(C_4_A_1))
            {
                u.pushEntityDialogue(C_4_A_2);
            }

            if (u.hasDialogueFinihsed(C_4_A_2))
            {
                u.pushEntityDialogue(C_4_A_3);
            }

            if (u.hasDialogueFinihsed(C_4_A_3))
            {
                u.pushEntityDialogue(C_4_A_4);
            }

            if (u.hasDialogueFinihsed(C_4_A_4))
            {
                u.pushEntityDialogue(C_4_A_5);
            }

            if (u.hasDialogueFinihsed(C_4_A_5))
            {
                u.pushEntityDialogue(C_4_A_6, { C_4_A_6_A, C_4_A_6_B, C_4_A_6_C });
            }

            if (u.hasChosenOption(C_4_A_6_A) || u.hasChosenOption(C_4_A_6_B))
            {
                u.pushEntityDialogue(C_4_A_6_AB_1);
            }

            if (u.hasDialogueFinihsed(C_4_A_6_AB_1))
            {
                _gangsterConfrontationStageData.canOskarMoveClose = true;
            }

            if (_gangsterConfrontationStageData.canOskarMoveClose)
            {
                if (moveEntityUntilXPosition(getComponentFromEntity<TransformComponent>(oskar), getComponentFromEntity<MovementComponent>(oskar),
                    getComponentFromEntity<SpriteComponent>(oskar), 290.f))
                {
                    switch (u._lastDialogueType)
                    {
                    case C_4_A_6_AB_1:
                        u.pushEntityDialogue(C_4_A_6_AB_2);
                        break;
                    case C_4_A_6:
                        if (u._lastOptionChosen == C_4_A_6_B || u._lastOptionChosen == C_4_A_6_A)
                        {
                            u.pushEntityDialogue(C_4_A_6_AB_2);
                        }
                        else if (u._lastOptionChosen == C_4_A_6_C)
                        {
                            pushGolfCueControntationDialogue(u);
                        }
                        break;
                    case C_4_BC_2:
                        u.pushEntityDialogue(C_4_BC_3, { C_4_BC_3_A, C_4_BC_3_B, C_4_BC_3_C });
                        break;
                    default:
                        break;
                    }

                    _gangsterConfrontationStageData.canOskarMoveClose = false;

                }
            }

            if (u.hasDialogueFinihsed(C_4_A_6_AB_2))
            {
                u.pushEntityDialogue(C_4_A_6_AB_3);
            }

            if (u.hasDialogueFinihsed(C_4_A_6_AB_3))
            {
                u.popTensionBar();
                player.entityState = IDLE_STATE;
            }

            if (u.hasChosenOption(C_4_A_6_C))
            {
                u.popTensionBar();
                player.entityState = IDLE_STATE;

                _gangsterConfrontationStageData.canOskarMoveClose = true;
            }

            if (u.hasDialogueFinihsed(C_4_BC_3_C_1))
            {
                u.pushEntityDialogue(C_4_BC_3_C_2);
            }

            // If rostov attacks oskar stop the dialogues
            auto* oskarA = getComponentFromEntity<AttackingComponent>(oskar);
            if (oskarA->damageCounter > 0 && !_gangsterConfrontationStageData.hasRostovAttackedEnemy)
            {
                _gangsterConfrontationStageData.hasRostovAttackedEnemy = true;

                if (u.isCurrentDialogue(C_4_BC_3_C_3) || u.isCurrentDialogue(C_4_BC_3_C_4) || u.isCurrentDialogue(C_4_BC_3_C_5) ||
                    u.isCurrentDialogue(C_4_BC_3_C_CUE_1) || u.isCurrentDialogue(C_4_BC_3_C_CUE_2) || u.isCurrentDialogue(C_4_BC_3_C_CUE_3))
                {
                    u.destroyCurrentDialogue();
                }
            }

            if (!_gangsterConfrontationStageData.hasRostovAttackedEnemy)
            {
                if (u.hasDialogueFinihsed(C_4_BC_3_C_2))
                {
                    u.pushEntityDialogue(C_4_BC_3_C_3);
                }

                if (u.hasDialogueFinihsed(C_4_BC_3_C_3))
                {
                    u.pushEntityDialogue(C_4_BC_3_C_4);
                }

                if (u.hasDialogueFinihsed(C_4_BC_3_C_4))
                {
                    u.pushEntityDialogue(C_4_BC_3_C_5);
                }

                if (u.hasDialogueFinihsed(C_4_BC_3_C_CUE_1))
                {
                    u.pushEntityDialogue(C_4_BC_3_C_CUE_2);
                }

                if (u.hasDialogueFinihsed(C_4_BC_3_C_CUE_2))
                {
                    u.pushEntityDialogue(C_4_BC_3_C_CUE_3);
                }
            }

            if (oskar.entityState == DEAD_STATE && !_gangsterConfrontationStageData.hasHugoHelpedBrother)
            {
                u.pushEntityDialogue(D_1);
                _gangsterConfrontationStageData.hasHugoHelpedBrother = true;
            }

            if (u.hasDialogueFinihsed(D_1))
            {
                entityLookAtAnother(&hugo, &oskar);
                u.pushEntityDialogue(D_2);
            }

            if (u.hasDialogueFinihsed(D_2))
            {
                _gangsterConfrontationStageData.canHugoReachBrother = true;
                getComponentFromEntity<AttackingComponent>(hugo)->canBeAttacked = true;
            }

            bool hasRostovAttackedHugo = getComponentFromEntity<AttackingComponent>(hugo)->damageCounter > 0;

            // Interrupt dialogue if we attack hugo
            if (hasRostovAttackedHugo)
            {
                getComponentFromEntity<MovementComponent>(hugo)->isMovingOnFloor = false;
                if (u.isCurrentDialogue(D_2) || u.isCurrentDialogue(D_3) || u.isCurrentDialogue(D_4) || u.isCurrentDialogue(D_5) || u.isCurrentDialogue(D_6))
                {
                    u.destroyCurrentDialogue();
                }
            }

            if (!hasRostovAttackedHugo)
            {
                if (_gangsterConfrontationStageData.canHugoReachBrother)
                {
                    // Move hugo until where brother died
                    float oskarXPos = getComponentFromEntity<TransformComponent>(oskar)->position.x + 10.f;
                    if (moveEntityUntilXPosition(getComponentFromEntity<TransformComponent>(hugo), getComponentFromEntity<MovementComponent>(hugo),
                        getComponentFromEntity<SpriteComponent>(hugo), oskarXPos))
                    {
                        u.pushEntityDialogue(D_3);
                        _gangsterConfrontationStageData.canHugoReachBrother = false;
                    }
                }

                if (u.hasDialogueFinihsed(D_3))
                {
                    startTimer(_gangsterConfrontationStageData.waitToCheckIfOskarIsDead);
                }

                if (isTimerOngoing(_gangsterConfrontationStageData.waitToCheckIfOskarIsDead))
                {
                    _gangsterConfrontationStageData.waitToCheckIfOskarIsDead += k_deltaTime;
                    if (_gangsterConfrontationStageData.waitToCheckIfOskarIsDead >= 2.f)
                    {
                        entityLookAtAnother(&hugo, &player);
                        u.pushEntityDialogue(D_4);
                        invalidateTimer(_gangsterConfrontationStageData.waitToCheckIfOskarIsDead);
                    }
                }

                if (u.hasDialogueFinihsed(D_4))
                {
                    u.pushEntityDialogue(D_5);
                }

                if (u.hasDialogueFinihsed(D_5))
                {
                    u.pushEntityDialogue(D_6);
                    _gangsterConfrontationStageData.canHugoReachRostov = true;
                }

                if (_gangsterConfrontationStageData.canHugoReachRostov)
                {
                    // Move hugo until rostov x pos
                    float rostovXPos = getComponentFromEntity<TransformComponent>(player)->position.x;
                    if (moveEntityUntilXPosition(getComponentFromEntity<TransformComponent>(hugo), getComponentFromEntity<MovementComponent>(hugo),
                        getComponentFromEntity<SpriteComponent>(hugo), rostovXPos))
                    {
                        _gangsterConfrontationStageData.canHugoReachRostov = false;
                    }
                }
            }

            if (!_gangsterConfrontationStageData.hasDarwinAskedToNotKillHugo)
            {
                bool willHugoDieOnNextHit = canKillyEntityFromCurrentState(hugo.entityState);
                if (willHugoDieOnNextHit)
                {
                    getComponentFromEntity<AttackingComponent>(hugo)->canBeAttacked = false;
                }

                bool isCloseToPretendKillHugo = abs(playerTransform->position.x - hugoT->position.x) < 40.f;
                if (wasAttackKeyPressedThisFrame() && willHugoDieOnNextHit && isCloseToPretendKillHugo)
                {
                    u.pushTensionBar();
                    player.entityState = ON_CUTSCENE_STATE;
                    u.pushEntityDialogue(D_7);
                    _gangsterConfrontationStageData.hasDarwinAskedToNotKillHugo = true;

                    // Darwin should walk near us
                    _gangsterConfrontationStageData.canDarwinComeClose = true;
                }
            }

            // This ensures that darwin moves to a correct spot to talk, taking into consideration if hugo is in front or behind darwin
            if (_gangsterConfrontationStageData.canDarwinComeClose)
            {
                bool shouldWalkRight = playerTransform->position.x > darwinT->position.x;
                float offsetFromTarget = shouldWalkRight ? 0.f : 45.f;

                float targetPosition = 0.f;
                if (shouldWalkRight)
                {
                    targetPosition = (hugoT->position.x > playerTransform->position.x) ? playerTransform->position.x : hugoT->position.x;
                }
                else
                {
                    targetPosition = (hugoT->position.x > playerTransform->position.x) ? hugoT->position.x : playerTransform->position.x;
                }

                if (moveEntityUntilXPosition(darwinT, darwinM, darwinS, targetPosition + offsetFromTarget))
                {
                    darwinS->flipX = true;
                    u.pushEntityDialogue(D_8);
                    _gangsterConfrontationStageData.canDarwinComeClose = false;
                }
            }

            if (u.hasDialogueFinihsed(D_8))
            {
                u.pushEntityDialogue(D_9);
            }

            if (u.hasDialogueFinihsed(D_9))
            {
                u.pushEntityDialogue(D_10);
            }

            if (u.hasDialogueFinihsed(D_10))
            {
                u.pushEntityDialogue(D_11);
            }

            if (u.hasDialogueFinihsed(D_11))
            {
                u.pushEntityDialogue(D_12, { D_12_A, D_12_B });
            }

            if (u.hasChosenOption(D_12_A))
            {
                u.pushEntityDialogue(D_12_A_1);
            }
            else if (u.hasChosenOption(D_12_B))
            {
                u.pushEntityDialogue(D_12_B_1);
            }

            if (u.hasDialogueFinihsed(D_12_A_1))
            {
                u.pushEntityDialogue(D_12_A_2, { D_12_A_2_A, D_12_A_2_B });
            }

            if (u.hasDialogueFinihsed(D_12_B_1))
            {
                u.pushEntityDialogue(D_12_B_2, { D_12_B_2_A, D_12_B_2_B });
            }

            if (u.hasChosenOption(D_12_A_2_A) || u.hasChosenOption(D_12_B_2_A))
            {
                u.pushEntityDialogue(D_13_HIGH_TENSION);
            }

            if (u.hasChosenOption(D_12_A_2_B) || u.hasChosenOption(D_12_B_2_B))
            {
                u.pushEntityDialogue(D_13_LOW_TENSION);
            }

            if (u.hasDialogueFinihsed(D_13_HIGH_TENSION))
            {
                u.pushEntityDialogue(D_14);
            }

            if (u.hasDialogueFinihsed(D_14))
            {
                _currentLevelStage = PHONE_CONFRONTATION_STAGE;
            }

            break;
        }

        case PHONE_CONFRONTATION_STAGE:
        {
            Entity& darwin = getEntityById(s_darwinEntityId);
            Entity& hugo = getEntityById(s_hugoEntityId);

            if (isTimerOngoing(_phoneConfrontationStageData.waitForHugoCallTimer) && !_phoneConfrontationStageData.hasHugoPhoneStartedRinging)
            {
                _phoneConfrontationStageData.waitForHugoCallTimer += k_deltaTime;
                if (_phoneConfrontationStageData.waitForHugoCallTimer >= 3.f)
                {
                    _phoneConfrontationStageData.hasHugoPhoneStartedRinging = true;
                    invalidateTimer(_phoneConfrontationStageData.waitForHugoCallTimer);
                    startTimer(_phoneConfrontationStageData.lookAtDarwinTimer);
                }
            }

            if (isTimerOngoing(_phoneConfrontationStageData.lookAtDarwinTimer))
            {
                _phoneConfrontationStageData.lookAtDarwinTimer += k_deltaTime;
                if (_phoneConfrontationStageData.lookAtDarwinTimer >= 0.5f)
                {
                    entityLookAtAnother(&player, &darwin);
                    
                    if (_phoneConfrontationStageData.lookAtDarwinTimer >= 1.5f)
                    {
                        u.pushEntityDialogue(E_1);
                        invalidateTimer(_phoneConfrontationStageData.lookAtDarwinTimer);
                    }
                }
            }

            if (u.hasDialogueFinihsed(E_1))
            {
                u.pushEntityDialogue(E_2);
            }

            if (u.hasDialogueFinihsed(E_2))
            {
                entityLookAtAnother(&player, &hugo);
                u.pushEntityDialogue(E_3);
            }

            if (u.hasDialogueFinihsed(E_3))
            {
                u.pushEntityDialogue(E_4);
            }

            if (u.hasDialogueFinihsed(E_4))
            {
                u.pushEntityDialogue(E_5);
            }

            if (u.hasDialogueFinihsed(E_5))
            {
                u.pushEntityDialogue(E_6);
            }

            if (u.hasDialogueFinihsed(E_6))
            {
                u.pushEntityDialogue(E_7, {E_7_A, E_7_B });
            }

            if (u.hasChosenOption(E_7_A))
            {
                u.pushEntityDialogue(E_7_A_1);
            }
            else if (u.hasChosenOption(E_7_B))
            {
                u.pushEntityDialogue(E_N_1);
            }

            // Picked up call
            if (u.hasDialogueFinihsed(E_7_A_1))
            {
                u.pushEntityDialogue(E_7_A_2, {E_7_A_2_A, E_7_A_2_B });
            }

            if (u.hasChosenOption(E_7_A_2_A))
            {
                u.pushEntityDialogue(E_7_A_2_A_1);
            }
            else if (u.hasChosenOption(E_7_A_2_B))
            {
                u.pushEntityDialogue(E_7_A_2_B_1);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_1))
            {
                u.pushEntityDialogue(E_7_A_2_A_2);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_2) || u.hasDialogueFinihsed(E_7_A_2_B_1))
            {
                startTimer(_phoneConfrontationStageData.askForDieselRepeatTimer);
            }

            if (isTimerOngoing(_phoneConfrontationStageData.askForDieselRepeatTimer))
            {
                _phoneConfrontationStageData.askForDieselRepeatTimer += k_deltaTime;
                if (_phoneConfrontationStageData.askForDieselRepeatTimer >= 2.f)
                {
                    u.pushEntityDialogue(E_7_A_2_A_3, { E_7_A_2_A_3_A, E_7_A_2_A_3_B });
                    invalidateTimer(_phoneConfrontationStageData.askForDieselRepeatTimer);
                }
            }

            if (u.hasChosenOption(E_7_A_2_A_3_A))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_A_1);
            }
            else if (u.hasChosenOption(E_7_A_2_A_3_B))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_B_1);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_B_1))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_B_2);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_B_2))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_B_3);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_B_3))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_B_4, { E_7_A_2_A_3_B_4_A, E_7_A_2_A_3_B_4_B });
            }

            if (u.hasChosenOption(E_7_A_2_A_3_B_4_A) || u.hasChosenOption(E_7_A_2_A_3_B_4_B))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_B_5);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_B_5))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_B_6);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_B_6))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_B_7);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_B_7))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_B_8, { E_7_A_2_A_3_B_8_A, E_7_A_2_A_3_B_8_B });
            }

            if (u.hasChosenOption(E_7_A_2_A_3_B_8_A) || u.hasChosenOption(E_7_A_2_A_3_B_8_B))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_B_9);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_B_9))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_B_10);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_B_10))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_B_11);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_B_11))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_B_12);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_B_12))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_B_13, { E_7_A_2_A_3_B_13_A, E_7_A_2_A_3_B_13_B });
            }

            if (u.hasChosenOption(E_7_A_2_A_3_B_13_A) || u.hasChosenOption(E_7_A_2_A_3_B_13_B))
            {
                u.interruptCurrentDialogue();
            }

            if (u.hasDialogueFinishedInterrupting(E_7_A_2_A_3_B_13))
            {
                u.pushEntityDialogue(E_8);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_A_1))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_A_2);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_A_2))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_A_3);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_A_3))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_A_4);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_A_4))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_A_5, { E_7_A_2_A_3_A_5_A, E_7_A_2_A_3_A_5_B });
            }

            if (u.hasChosenOption(E_7_A_2_A_3_A_5_A))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_A_5_A_1);
            }
            else if (u.hasChosenOption(E_7_A_2_A_3_A_5_B))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_A_5_B_1);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_A_5_B_1))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_A_5_B_2);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_A_5_B_2))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_A_5_B_3);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_A_5_B_3))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_A_5_B_4);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_A_5_B_4))
            {
                u.pushEntityDialogue(E_7_1);
                finishHugoConversation(u, player, hugo, _phoneConfrontationStageData.canHugoProvokeDarwin);
            }

            if (u.hasDialogueFinihsed(E_7_1))
            {
                u.pushEntityDialogue(E_11);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_A_5_A_1))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_A_5_A_2);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_A_5_A_2))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_A_5_A_3, { E_7_A_2_A_3_A_5_A_3_A, E_7_A_2_A_3_A_5_A_3_B, E_7_A_2_A_3_A_5_A_3_C });
            }

            if (u.hasChosenOption(E_7_A_2_A_3_A_5_A_3_A) || u.hasChosenOption(E_7_A_2_A_3_A_5_A_3_B) || u.hasChosenOption(E_7_A_2_A_3_A_5_A_3_C))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_A_5_A_4);
            }

            if (u.hasDialogueFinihsed(E_7_A_2_A_3_A_5_A_4))
            {
                u.pushEntityDialogue(E_7_A_2_A_3_A_5_A_5, { E_7_A_2_A_3_A_5_A_5_A, E_7_A_2_A_3_A_5_A_5_B });
            }

            if (u.hasChosenOption(E_7_A_2_A_3_A_5_A_5_A) || u.hasChosenOption(E_7_A_2_A_3_A_5_A_5_B))
            {
                u.interruptCurrentDialogue();
            }

            if (u.hasDialogueFinishedInterrupting(E_7_A_2_A_3_A_5_A_5))
            {
                u.pushEntityDialogue(E_8);
            }

            // Not picked up call
            if (u.hasDialogueFinihsed(E_N_1))
            {
                u.pushEntityDialogue(E_N_2, { E_N_2_A, E_N_2_B });
            }

            if (u.hasChosenOption(E_N_2_A))
            {
                u.pushEntityDialogue(E_N_2_A_1, { E_N_2_A_1_A, E_N_2_A_1_B });
            }

            if (u.hasChosenOption(E_N_2_B) || u.hasChosenOption(E_N_2_A_1_A) || u.hasChosenOption(E_N_2_A_1_B))
            {
                u.interruptCurrentDialogue();
            }
            
            if (u.hasDialogueFinishedInterrupting(E_N_2) || u.hasDialogueFinishedInterrupting(E_N_2_A_1))
            {
                u.pushEntityDialogue(E_8);
            }

            if (u.hasDialogueFinihsed(E_8))
            {
                u.pushEntityDialogue(E_9);
            }

            if (u.hasDialogueFinihsed(E_9))
            {
                u.pushEntityDialogue(E_10);
            }

            if (u.hasDialogueFinihsed(E_10))
            {
                u.pushEntityDialogue(E_11);
                finishHugoConversation(u, player, hugo, _phoneConfrontationStageData.canHugoProvokeDarwin);
            }

            if (_phoneConfrontationStageData.canHugoProvokeDarwin)
            {
                float doorXPos = 70.f;
                auto* hugoTransform = getComponentFromEntity<TransformComponent>(hugo);
                bool isRostovCloseToDoor = hugoTransform->position.x - playerTransform->position.x >= 70.f;

                if (isRostovCloseToDoor)
                {
                    u.pushEntityDialogue(E_12);
                    _phoneConfrontationStageData.canHugoProvokeDarwin = false;
                }
            }

            if (u.hasDialogueFinihsed(E_12))
            {
                u.pushEntityDialogue(E_13);
            }

            if (u.hasDialogueFinihsed(E_13))
            {
                u.pushEntityDialogue(E_14);
            }

            if (u.hasDialogueFinihsed(E_14))
            {
                u.pushEntityDialogue(E_15);
            }

            if (u.hasDialogueFinihsed(E_15))
            {
                u.pushEntityDialogue(E_16);
            }

            if (u.hasDialogueFinihsed(E_16))
            {
                u.pushEntityDialogue(E_17);
            }

            if (u.hasDialogueFinihsed(E_17))
            {
                u.pushEntityDialogue(E_18);
            }

            if (u.hasDialogueFinihsed(E_18))
            {
                u.pushEntityDialogue(E_19);
            }

            if (u.hasDialogueFinihsed(E_19))
            {
                u.pushEntityDialogue(E_20);
            }

            if (u.hasDialogueFinihsed(E_20))
            {
                u.pushEntityDialogue(E_21);
            }

            bool isHugoDead = hugo.entityState == DEAD_STATE;
            if (isHugoDead && !_phoneConfrontationStageData.hasDarwinComplainedAboutKillingHugo)
            {
                if (u.isCurrentDialogue(E_21))
                {
                    u.interruptCurrentDialogue();
                }
                else
                {
                    u.pushEntityDialogue(E_22);
                    _phoneConfrontationStageData.hasDarwinComplainedAboutKillingHugo = true;
                }

                if (u.hasDialogueFinishedInterrupting(E_21))
                {
                    u.pushEntityDialogue(E_22);
                    _phoneConfrontationStageData.hasDarwinComplainedAboutKillingHugo = true;
                }
            }

            if (u.hasDialogueFinihsed(E_22))
            {
                u.pushEntityDialogue(E_23);
            }

            if (u.hasDialogueFinihsed(E_23))
            {
                startTimer(_phoneConfrontationStageData.darwinFinalDialogueTimer);
            }

            if (isTimerOngoing(_phoneConfrontationStageData.darwinFinalDialogueTimer))
            {
                _phoneConfrontationStageData.darwinFinalDialogueTimer += k_deltaTime;
                if (_phoneConfrontationStageData.darwinFinalDialogueTimer >= 3.f)
                {
                    u.pushEntityDialogue(E_24);
                    invalidateTimer(_phoneConfrontationStageData.darwinFinalDialogueTimer);
                }
            }

            break;
        }
    }

    // Dynamic ambient color
    {
        float xPositionWherePlayerIsInsidePantry = 407.f;

        SDL_Color targetAmbientColor = { 0,0,0 };

        if (playerTransform->position.x >= xPositionWherePlayerIsInsidePantry)
        {
            targetAmbientColor = { 64, 64, 64 };
        }
        else
        {
            targetAmbientColor = { 115, 115, 115 };
        }

        bool isCurrentColorDifferentFromTarget = targetAmbientColor.r != s_renderingSystem._currentAmbientColor.r ||
            targetAmbientColor.b != s_renderingSystem._currentAmbientColor.b ||
            targetAmbientColor.g != s_renderingSystem._currentAmbientColor.g;

        if (isCurrentColorDifferentFromTarget)
        {
            s_renderingSystem.setTargetAmbientColor(targetAmbientColor.r, targetAmbientColor.g, targetAmbientColor.b);

            float k_ambientColorChangeSpeed = 0.025f;
            s_renderingSystem._currentAmbientColor.r = lerp((float)s_renderingSystem._currentAmbientColor.r, (float)s_renderingSystem._targetAmbientColor.r, k_ambientColorChangeSpeed);
            s_renderingSystem._currentAmbientColor.b = lerp((float)s_renderingSystem._currentAmbientColor.b, (float)s_renderingSystem._targetAmbientColor.b, k_ambientColorChangeSpeed);
            s_renderingSystem._currentAmbientColor.g = lerp((float)s_renderingSystem._currentAmbientColor.g, (float)s_renderingSystem._targetAmbientColor.g, k_ambientColorChangeSpeed);
        }
    }

    // Update systems
    {
        overrideColliderOffsetsBasedOnCurrentSprite();
        s_characterMovementSystem.update();
        s_attackingSystem.update();
        s_animationSystem.update();
        s_crosshairSystem.update();
        s_uiSystem.update();

        Entity& playerEffects = getEntityById(k_playerEffectsEntityId);
        getComponentFromEntity<TransformComponent>(playerEffects)->position = playerTransform->position;
        getComponentFromEntity<SpriteComponent>(playerEffects)->flipX = getComponentFromEntity<SpriteComponent>(player)->flipX;
    }

    static float cameraOffsetXFromPlayer = 20.f;
    static bool testShake = false;

    // After all systems, update camera
    {
        _levelCamera.minX = s_isInsideRestaurant ? 160 : -320;
        _levelCamera.maxX = s_isInsideRestaurant ? 540 : 0;
        _levelCamera.followTargetRatio = 0.06f;

        if (_levelCamera.canFollowTarget)
        {
            _levelCamera.targetPosition = { playerTransform->position.x + cameraOffsetXFromPlayer, 90.f };
            _levelCamera.targetPosition.x = clamp(_levelCamera.targetPosition.x, _levelCamera.minX, _levelCamera.maxX);
        }

        handleCameraShake(_levelCamera);

        // Try to catch up with target position
        if (abs(_levelCamera.targetPosition.x - _levelCamera.position.x) > 0.5f || abs(_levelCamera.targetPosition.y - _levelCamera.position.y) > 0.f)
        {
            _levelCamera.position = lerp(_levelCamera.position, _levelCamera.targetPosition, _levelCamera.followTargetRatio);
        }
    }

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

        // Debug level blocks
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

        // Toggle imgui
        if (_wasKeyPressedThisFrame(SDL_SCANCODE_TAB))
        {
            s_isImGuiOpen = !s_isImGuiOpen;
        }

        if (s_isImGuiOpen)
        {
            return;
        }

        // Destroy dialogue and reconstruct
        if (_wasKeyPressedThisFrame(SDL_SCANCODE_I))
        {
            D_LOG(LOG, "Dialogue recreated");
            s_uiSystem._cellphone.state = UISystem::CELLPHONE_TALKING;
            s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_1);
            _currentLevelStage = MARKETING_PHONE_STAGE;
            //startTimer(s_multiPurpuseTimer);
            s_playerTension = 0;
        }

        // Debug to not have to wait x seconds for things to happen
        if (_wasKeyPressedThisFrame(SDL_SCANCODE_K))
        {
            s_multiPurpuseTimer = 100.f;
        }
    }
}

void Level::render(float renderAlpha)
{
    s_renderingSystem.render(renderAlpha);
    s_uiSystem.render(&s_renderingSystem);
    s_debugCollidersSystem.render();
    s_uiSystem.debugColliders();
    s_renderingSystem.renderCrosshair(renderAlpha);
}

void iterateOnLastPlacedEntity()
{
    //Entity& last = getEntityById(17);
    //if (!entityHasComponent<SpriteComponent>(last))
    //{
    //    addComponentToEntity<SpriteComponent>(last);
    //}

    //auto* s = getComponentFromEntity<SpriteComponent>(last);
    //s->setupSpriteForLayer(BIG_ROUND_LIGHT_SPRITE, FRONT_LIGHTS_LAYER);
}

void Level::imguiRender()
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

    // Misc panel
    {
        ImGui::Begin("Misc");

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

        if (ImGui::Button("Add Debug White Entity"))
        {
            Entity& entity = addEntity("debugEntity", getComponentFromEntity<TransformComponent>(player)->position);
            addComponentToEntity<SpriteComponent>(entity)->setupSpriteForLayer(WHITE_ONE_BY_ONE_SPRITE, BEHIND_LIGHTS_LAYER);
        }

        if (ImGui::Button("Iterate on last added entity"))
        {
            Entity& e = addEntity("debugEntity");

            //_gangsterConfrontationStageData.canDarwinComeClose = true;

            //player.entityState = IDLE_STATE;
            //Entity& hugo = getEntityById(s_hugoEntityId);
            //getComponentFromEntity<TransformComponent>(hugo)->position.x = getComponentFromEntity<TransformComponent>(player)->position.x;

            //getComponentFromEntity<TransformComponent>(hugo)->position.x += 4.f;
            //Entity& entity = getLastAddedEntity();
            //getComponentFromEntity<SpriteComponent>(entity)->setupSpriteForLayer(GANGSTER_OSKAR_SPRITE, CHARACTERS_LAYER);
            //addComponentToEntity<MovementComponent>(entity);
            //addComponentToEntity<RectColliderComponent>(entity)->collider = RectCollider({ 0,0 }, { 13, 19 });
            //getComponentFromEntity<TransformComponent>(entity)->position = { 270.f, 100.f };

              //Entity& darwin = getEntityById(s_darwinEntityId);
            //getComponentFromEntity<TransformComponent>(darwin)->position = {293.f, 117.f };
            //getComponentFromEntity<SpriteComponent>(darwin)->flipX = true;
        }

        ImGui::SeparatorText("Levels");

        static int s_levelStageToChangeTo = MARKETING_PHONE_STAGE;
        ImGui::Combo("Level Stages", &s_levelStageToChangeTo, s_levelStagesString);
        if (ImGui::Button("Change to selected level stage"))
        {
            s_uiSystem.destroyCurrentDialogue();
            s_uiSystem.popTensionBar();

            switch (s_levelStageToChangeTo)
            {
            case MARKETING_PHONE_STAGE:
                _currentLevelStage = MARKETING_PHONE_STAGE;
                s_uiSystem.pushCellphoneDialogue(MARKETING_PHONE_1);
                s_uiSystem._cellphone.state = s_uiSystem.CELLPHONE_TALKING;
                s_playerTension = 0;
                break;
            case FIRST_DAD_PHONE_STAGE:
                _currentLevelStage = FIRST_DAD_PHONE_STAGE;
                s_uiSystem.pushCellphoneDialogue(ONE_DAD_PHONE_1);
                s_uiSystem._cellphone.state = s_uiSystem.CELLPHONE_TALKING;
                s_playerTension = 80;
                break;
            case DARWIN_CONVERSATION_STAGE:
            {
                _currentLevelStage = FIRST_DAD_PHONE_STAGE;
                s_uiSystem.pushCellphoneDialogue(ONE_DAD_PHONE_9);
                s_uiSystem._cellphone.state = s_uiSystem.CELLPHONE_TALKING;
                startTimer(s_multiPurpuseTimer);
                s_playerTension = 20;
                Entity& darwin = getEntityById(s_darwinEntityId);
                getComponentFromEntity<TransformComponent>(darwin)->position = getComponentFromEntity<TransformComponent>(darwin)->startingPosition;
                _darwinConversationStageData.reset();
                break;
            }
            case GANGSTER_CONFRONTATION_STAGE:
            {
                _currentLevelStage = GANGSTER_CONFRONTATION_STAGE;
                s_uiSystem._cellphone.state = s_uiSystem.CELLPHONE_NOT_VISIBLE_STATE;

                Entity& darwin = getEntityById(s_darwinEntityId);
                getComponentFromEntity<TransformComponent>(darwin)->position = { 293.f, 117.f };
                getComponentFromEntity<SpriteComponent>(darwin)->flipX = true;

                Entity& player = getEntityById(k_playerEntityId);
                getComponentFromEntity<TransformComponent>(player)->position.x = getComponentFromEntity<TransformComponent>(player)->startingPosition.x;
                player.entityState = IDLE_STATE;

                Entity& oskar = getEntityById(s_oskarEntityId);
                getComponentFromEntity<TransformComponent>(oskar)->position = getComponentFromEntity<TransformComponent>(oskar)->startingPosition;
                getComponentFromEntity<AttackingComponent>(oskar)->damageCounter = 0;
                oskar.entityState = IDLE_STATE;

                Entity& hugo = getEntityById(s_hugoEntityId);
                getComponentFromEntity<TransformComponent>(hugo)->position = getComponentFromEntity<TransformComponent>(hugo)->startingPosition;
                getComponentFromEntity<SpriteComponent>(hugo)->flipX = false;
                getComponentFromEntity<AttackingComponent>(hugo)->damageCounter = 0;
                getComponentFromEntity<AttackingComponent>(hugo)->canBeAttacked = false;
                getComponentFromEntity<AttackingComponent>(hugo)->shouldWaitToDie = true;
                hugo.entityState = IDLE_STATE;

                _gangsterConfrontationStageData.reset();

                break;
            }
            case PHONE_CONFRONTATION_STAGE:
            {
                _currentLevelStage = PHONE_CONFRONTATION_STAGE;

                s_uiSystem._cellphone.state = s_uiSystem.CELLPHONE_NOT_VISIBLE_STATE;
                s_uiSystem.pushTensionBar();

                Entity& player = getEntityById(k_playerEntityId);
                getComponentFromEntity<TransformComponent>(player)->position.x = 200.f;
                getComponentFromEntity<SpriteComponent>(player)->flipX = true;
                player.entityState = ON_CUTSCENE_STATE;

                Entity& darwin = getEntityById(s_darwinEntityId);
                getComponentFromEntity<TransformComponent>(darwin)->position = { 245.f, 117.f };
                getComponentFromEntity<SpriteComponent>(darwin)->flipX = true;

                Entity& oskar = getEntityById(s_oskarEntityId);
                getComponentFromEntity<TransformComponent>(oskar)->position = getComponentFromEntity<TransformComponent>(oskar)->startingPosition;
                oskar.entityState = DEAD_STATE;

                Entity& hugo = getEntityById(s_hugoEntityId);
                getComponentFromEntity<TransformComponent>(hugo)->position = getComponentFromEntity<TransformComponent>(hugo)->startingPosition;
                getComponentFromEntity<SpriteComponent>(hugo)->flipX = false;
                getComponentFromEntity<AttackingComponent>(hugo)->damageCounter = getComponentFromEntity<AttackingComponent>(hugo)->numberOfHitsToFall + 1;
                hugo.entityState = WAIT_TO_DIE_STATE;

                _phoneConfrontationStageData.reset();
                break;
            }
            case FREE_STAGE:
                Entity& player = getEntityById(k_playerEntityId);
                player.entityState = IDLE_STATE;
                s_uiSystem.hangupPhone();
            }
        }

        if (ImGui::ColorEdit3("Ambient Color", s_renderingSystem._debugAmbientColorPicker, ImGuiColorEditFlags_NoInputs))
        {
            s_renderingSystem.setTargetAmbientColor(s_renderingSystem._debugAmbientColorPicker[0] * 255,
                s_renderingSystem._debugAmbientColorPicker[1] * 255,
                s_renderingSystem._debugAmbientColorPicker[2] * 255);
        }


        ImGui::Checkbox("Toogle Camera Follow Player", &_levelCamera.canFollowTarget);
        ImGui::DragFloat("Camera Pos X", &_levelCamera.targetPosition.x);
        ImGui::DragFloat("Camera Pos Y", &_levelCamera.targetPosition.y);
        ImGui::DragFloat("Camera zoom", &_levelCamera.zoom, 0.01f, 1.f, 2.f);
        if (ImGui::Button("Reset camera"))
        {
            _levelCamera.canFollowTarget = true;
            _levelCamera.zoom = 1.f;
        }

        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
        if (ImGui::Button("Reload Atlas"))
        {
            s_renderingSystem.reloadAtlas(GAME_ATLAS);
        }
        ImGui::PopStyleColor();

        ImGui::End();
    }

    // Entity Inspector
    {
        ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
        ImGui::Begin("Entity Looker");

        static Entity* selectedEntityToInspect = nullptr;
        static bool isSelectedEntityOutlineVisible = false;

        // Left side -> entities tree
        {
            static char name[256] = "";
            ImGui::SetNextItemWidth(130);
            ImGui::InputTextWithHint("##", "Enter entity name", name, IM_ARRAYSIZE(name));
            ImGui::SameLine();
            if (ImGui::Button("Add entity"))
            {
                addEntity(name);
                name[0] = '\0';
            }

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
            if (ImGui::Button("Copy all changes to clipboard"))
            {
                copyAllChangesToClipboard();
            }
            ImGui::PopStyleColor();

            ImGui::SameLine();

            if (ImGui::Button("Debug"))
            {
                iterateOnLastPlacedEntity();
            }

            ImGui::BeginChild("##tree", ImVec2(300, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened);
            if (ImGui::BeginTable("##bg", 1, ImGuiTableFlags_RowBg))
            {
                if (selectedEntityToInspect && isSelectedEntityOutlineVisible)
                {
                    Vec2 position = getComponentFromEntity<TransformComponent>(*selectedEntityToInspect)->position;
                    auto* s = entityHasComponent<SpriteComponent>(*selectedEntityToInspect) ? getComponentFromEntity<SpriteComponent>(*selectedEntityToInspect) : nullptr;
                    IVec2 debugSize = s ? s->getCurrentSize() : IVec2(1, 1);

                    RectCollider outline = { {0,0}, debugSize };
                    DebugSystem::debugRect(position, outline, { 255, 135, 0 });
                }

                for (Entity& entity : getAllEntities())
                {
                    if (entity.id == k_invalidId)
                    {
                        continue;
                    }

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::PushID(entity.id);

                    ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth;

                    bool isSelectedEntity = selectedEntityToInspect && (selectedEntityToInspect->id == entity.id);
                    if (isSelectedEntity)
                    {
                        treeFlags |= ImGuiTreeNodeFlags_Selected;
                    }

                    ImGui::TreeNodeEx(entity.debugName, treeFlags);

                    if (ImGui::IsItemFocused())
                    {
                        selectedEntityToInspect = &entity;
                        isSelectedEntityOutlineVisible = true;
                    }

                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();
        }

        ImGui::SameLine();

        // Right Side -> Properties
        ImGui::BeginGroup();
        if (selectedEntityToInspect)
        {
            ImGui::Text("Entity: %s", selectedEntityToInspect->debugName);
            ImGui::TextDisabled("Id: %i", selectedEntityToInspect->id);

            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(5 / 7.0f, 0.6f, 0.6f));
            if (ImGui::Button("Toggle Focus"))
            {
                _levelCamera.canFollowTarget = !_levelCamera.canFollowTarget;

                if (!_levelCamera.canFollowTarget)
                {
                    _levelCamera.targetPosition.x = getComponentFromEntity<TransformComponent>(*selectedEntityToInspect)->position.x;
                }
            }
            ImGui::PopStyleColor();

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(6 / 7.0f, 0.6f, 0.6f));
            if (ImGui::Button("Toggle Outline"))
            {
                isSelectedEntityOutlineVisible = !isSelectedEntityOutlineVisible;
            }
            ImGui::PopStyleColor();

            if (startInspectorComponentSection<TransformComponent>(selectedEntityToInspect))
            {
                auto* t = getComponentFromEntity<TransformComponent>(*selectedEntityToInspect);
                inspectFloatProperty("position.x", &t->position.x, selectedEntityToInspect);
                inspectFloatProperty("position.y", &t->position.y, selectedEntityToInspect);
                inspectFloatProperty("scale.x", &t->scale.x, selectedEntityToInspect);
                inspectFloatProperty("scale.y", &t->scale.y, selectedEntityToInspect);

                endInspectorComponentSection();
            }

            if (startInspectorComponentSection<SpriteComponent>(selectedEntityToInspect))
            {
                auto* s = getComponentFromEntity<SpriteComponent>(*selectedEntityToInspect);
                inspectColorProperty("color", &s->color, selectedEntityToInspect);
                inspectEnumProperty("layer", &s->layer, s_allLayersAsString, selectedEntityToInspect);
                inspectSpriteProperty("sprite", s, selectedEntityToInspect);

                endInspectorComponentSection();
            }

            if (startInspectorComponentSection<RectColliderComponent>(selectedEntityToInspect))
            {
                endInspectorComponentSection();
            }
            
            if (startInspectorComponentSection<MovementComponent>(selectedEntityToInspect))
            {
                endInspectorComponentSection();
            }

            if (startInspectorComponentSection<AttackingComponent>(selectedEntityToInspect))
            {
                endInspectorComponentSection();
            }
        }
        ImGui::EndGroup();
        ImGui::End();
    }
}
