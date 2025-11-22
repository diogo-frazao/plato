#include "level.h"

#include "core/input.h"
#include "renderingComponents.h"
#include "imgui.h"

int playerEntityId;

void createBlockAtPosition(IVec2 position)
{
    Entity& block = addEntity();
    addComponentToEntity<TransformComponent>(block);
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
    addComponentToEntity<TransformComponent>(bg);
    addComponentToEntity<SpriteComponent>(bg)->setup({ 0,0 }, { 320, 180 }, BEHIND_CHAR);

    Entity& player = addEntity();
    playerEntityId = player.id;
    addComponentToEntity<TransformComponent>(player);
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

    Entity& geometry = addEntity();
    addComponentToEntity<TransformComponent>(geometry)->position = { 0, 135 };
    addComponentToEntity<SpriteComponent>(geometry)->setup({ 0, 180 }, {320, 45}, LEVEL_GEOMETRY);

    Entity& fg = addEntity();
    addComponentToEntity<TransformComponent>(fg)->position = { 34, 129 };
    addComponentToEntity<SpriteComponent>(fg)->setup({ 0, 225 }, { 320, 23 }, IN_FRONT_CHAR);

    Entity& letterLight = addEntity();
    addComponentToEntity<TransformComponent>(letterLight)->position = { 0,0 };
    addComponentToEntity<SpriteComponent>(letterLight)->setup({ 139, 0 }, { 130, 130 }, FRONT_LIGHTS, LIGHTS);
}

void ECSLevel::update()
{
    // TODO: remove, placeholder to place tiles
    if (wasMouseButtonPressedThisFrame(LEFT))
    {
        IVec2 closestGridPosition = { (int32_t)(s_mousePositionThisFrame.x / 8), (int32_t)(s_mousePositionThisFrame.y / 8) };
        IVec2 gridWorldPosition = { closestGridPosition.x * 8, closestGridPosition.y * 8 };
        createBlockAtPosition(gridWorldPosition);
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