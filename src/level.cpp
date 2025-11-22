#include "level.h"

#include "core/input.h"
#include "renderingComponents.h"
#include "imgui.h"

int playerEntityId;

void createBlockAtPosition(IVec2 position)
{
    Entity& block = addEntity();
    addComponentToEntity<SpriteComponent>(block)->setup({ 336, 0 }, { 8,8 }, LEVEL_GEOMETRY);
    getComponentFromEntity<TransformComponent>(block)->previousPosition = { (float)position.x, (float)position.y };
    getComponentFromEntity<TransformComponent>(block)->position = { (float)position.x, (float)position.y };
    addComponentToEntity<RectColliderComponent>(block)->collider = RectCollider({ 0, 0 }, { 8, 8 });
    getComponentFromEntity<RectColliderComponent>(block)->isLevelGeometry = true;
}

void ECSLevel::start()
{
	_renderingSystem.createLightsBuffers();

    Entity& bg = addEntity();
    addComponentToEntity<SpriteComponent>(bg)->setup({ 0,0 }, { 320, 180 }, BEHIND_CHAR);

    Entity& player = addEntity();
    playerEntityId = player.id;
    SpriteComponent* playerSprite = addComponentToEntity<SpriteComponent>(player);
    auto* movementComponent = addComponentToEntity<MovementComponent>(player);
    movementComponent->maxHorizontalSpeed = 1.5f;
    movementComponent->runAcceleration = 9.185f;
    movementComponent->friction = 10.f;
    movementComponent->maxVerticalSpeed = 3.6f;
    movementComponent->gravity = 13.f;
    movementComponent->jumpSpeed = 3.f;

    playerSprite->setup({ 321, 0 }, { 14, 19 }, CHARACTER);
    addComponentToEntity<RectColliderComponent>(player)->collider = RectCollider({ 2, 2 }, { 9, 17 });

    Entity& geometry = addEntity({0, 135});
    addComponentToEntity<SpriteComponent>(geometry)->setup({ 0, 180 }, {320, 45}, LEVEL_GEOMETRY);

    Entity& fg = addEntity({34, 129});
    addComponentToEntity<SpriteComponent>(fg)->setup({ 0, 225 }, { 320, 23 }, IN_FRONT_CHAR);

    Entity& darwinLettersLight = addEntity();
    addComponentToEntity<SpriteComponent>(darwinLettersLight)->setup({ 45, 0 }, { 95, 86 }, FRONT_LIGHTS, LIGHTS);
    getComponentFromEntity<TransformComponent>(darwinLettersLight)->position = { 220, 55 };
    getComponentFromEntity<TransformComponent>(darwinLettersLight)->scale = { 1.5f, 0.4f };
    getComponentFromEntity<SpriteComponent>(darwinLettersLight)->color = { 200, 0, 0 , 255};

    Entity& darwinLettersSmallGlow = addEntity();
    addComponentToEntity<SpriteComponent>(darwinLettersSmallGlow)->setup({ 45, 0 }, { 95, 86 }, FRONT_LIGHTS, LIGHTS);
    getComponentFromEntity<TransformComponent>(darwinLettersSmallGlow)->position = { 225, 20 };
    getComponentFromEntity<TransformComponent>(darwinLettersSmallGlow)->scale = { 1.35f, 2.f };
    getComponentFromEntity<SpriteComponent>(darwinLettersSmallGlow)->color = { 80, 0, 0 , 255};

    Entity& streetLampLight = addEntity();
    addComponentToEntity<SpriteComponent>(streetLampLight)->setup({ 0, 0 }, { 44, 86 }, FRONT_LIGHTS, LIGHTS);
    getComponentFromEntity<TransformComponent>(streetLampLight)->position = { 24, 86 };
    getComponentFromEntity<TransformComponent>(streetLampLight)->scale = { 1.f, 1.f };
    getComponentFromEntity<SpriteComponent>(streetLampLight)->color = { 43, 15, 0 , 255};

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

    if (wasKeyPressedThisFrame(SDL_SCANCODE_O))
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

	_savePositionSystem.update();
	_characterMovementSystem.update();
}

void ECSLevel::imguiRender()
{
    ImGui::Begin("Player");

    Entity& player = getEntityById(playerEntityId);

    auto* movement = getComponentFromEntity<MovementComponent>(player);
    ImGui::SliderFloat("Player Acceleration", &(movement->runAcceleration), 1, 30);
    ImGui::SliderFloat("Player Friction", &(movement->friction), 0.5f, 10);
    ImGui::SliderFloat("Player Max Horizontal Speed", &(movement->maxHorizontalSpeed), 0.1f, 10.f);

    ImGui::Text("Player Speed X: %f", getComponentFromEntity<MovementComponent>(player)->currentSpeed.x);
    ImGui::Text("Player X: %f", getComponentFromEntity<TransformComponent>(player)->position.x);
    ImGui::Text("Player Y: %f", getComponentFromEntity<TransformComponent>(player)->position.y);

    ImGui::Checkbox("Debug colliders", &s_debugCollidersEnabled);
    ImGui::Checkbox("Debug grid", &s_debugGridEnabled);

    ImGui::Text("Mouse X: %i", (int)s_mousePositionThisFrame.x);
    ImGui::Text("Mouse Y: %i", (int)s_mousePositionThisFrame.y);

    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Average %.1f FPS", io.Framerate);
    ImGui::Text("V-sync is %s", s_vsyncEnabled ? "enabled" : "disabled");

    ImGui::End();
}

void ECSLevel::render(float renderAlpha)
{
	_renderingSystem.render(renderAlpha);
	_debugCollidersSystem.render();
}