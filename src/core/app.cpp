
#include "app.h"

#include "log.h"
#include "imgui.h"
#include "imguiThemes.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <stdint.h>

#include "ecsLevel.h"
//TODO: remove
#include "../renderingComponents.h"

#include "input.h"

void drawImguiDockingPreview();
void testResolutions(SDL_Window* window);

ECSLevel firstLevel;
static bool s_isWindowFullscreen = false;
static bool s_vsyncEnabled = true;

void App::run()
{
    init();
    firstLevel.start();
    update();
    quit();
}

void App::init()
{
    initSDL();
    initImgui();
}

void createBlockAtPosition(IVec2 position)
{
    Entity& block = firstLevel.addEntity();
    firstLevel.addComponentToEntity<TransformComponent>(block);
    firstLevel.addComponentToEntity<SpriteComponent>(block)->setupWithOffsetAndSize({ 336, 0 }, { 8,8 });
    firstLevel.getComponentFromEntity<TransformComponent>(block)->previousPosition = { (float)position.x, (float)position.y };
    firstLevel.getComponentFromEntity<TransformComponent>(block)->position = { (float)position.x, (float)position.y};
    firstLevel.addComponentToEntity<RectColliderComponent>(block)->collider = RectCollider({ 0, 0 }, { 8, 8 });
    firstLevel.getComponentFromEntity<RectColliderComponent>(block)->isSolid = true;
}

void App::update()
{
    bool showDemoWindow = true;
    
    Entity& bg = firstLevel.addEntity();
    firstLevel.addComponentToEntity<TransformComponent>(bg);
    firstLevel.addComponentToEntity<SpriteComponent>(bg)->setupWithOffsetAndSize({ 0,0 }, { 320, 180 });

    Entity& player = firstLevel.addEntity();
    firstLevel.addComponentToEntity<TransformComponent>(player);
    SpriteComponent* playerSprite = firstLevel.addComponentToEntity<SpriteComponent>(player);
    auto* movementComponent = firstLevel.addComponentToEntity<MovementComponent>(player);
    movementComponent->maxHorizontalSpeed = 1.5f;
    movementComponent->runAcceleration = 9.185f;
    movementComponent->friction = 10.f;
    movementComponent->maxVerticalSpeed = 3.6f;
    movementComponent->gravity = 13.f;
    movementComponent->jumpSpeed = 3.f;

    playerSprite->setupWithOffsetAndSize({ 321, 0 }, { 14, 19 });
    firstLevel.addComponentToEntity<RectColliderComponent>(player)->collider = RectCollider({ 2, 2 }, { 9, 17 });

    createBlockAtPosition({ 48, 48 });

    uint64_t lastFrameTimestamp = SDL_GetTicks();
    float accumulator = 0.0f;

    Entity& enemy = firstLevel.addEntity();

    while (true)
    {
        uint64_t currentFrameTimeStamp = SDL_GetTicks();
        uint64_t millisecondsSinceLastFrame = currentFrameTimeStamp - lastFrameTimestamp;
        millisecondsSinceLastFrame = min(millisecondsSinceLastFrame, k_maxFrameTimeAllowed);

        accumulator += millisecondsSinceLastFrame;
        lastFrameTimestamp = currentFrameTimeStamp;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);

            bool didRequestWindowClose = event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(_window);
            if (event.type == SDL_EVENT_QUIT || didRequestWindowClose)
            {
                return;
            }

            handleKeyboardInput(event);
            handleMouseInput(event);
        }

        while (accumulator >= k_targetMillisecondsBetweenFrames)
        {
            firstLevel.update();

            // TODO: remove, placeholder to place tiles
            if (wasMouseButtonPressedThisFrame(LEFT))
            {
                IVec2 closestGridPosition = { (int32_t)(s_mousePositionThisFrame.x / 8), (int32_t)(s_mousePositionThisFrame.y / 8) };
                IVec2 gridWorldPosition = { closestGridPosition.x * 8, closestGridPosition.y * 8 };
                createBlockAtPosition(gridWorldPosition);
            }

#ifndef RELEASE_BUILD
            testResolutions(_window);
#endif // !RELEASE_BUILD

            resetKeyboardAndMouseInput();
            accumulator -= k_targetMillisecondsBetweenFrames;
        }

        float renderAlpha = accumulator / k_targetMillisecondsBetweenFrames;

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        drawImguiDockingPreview();
        imguiThemes::green();

        if (showDemoWindow)
        {
            ImGui::ShowDemoWindow(&showDemoWindow);
        }

        ImGui::Begin("Player");
        auto* movement = firstLevel.getComponentFromEntity<MovementComponent>(player);
        ImGui::SliderFloat("Player Acceleration", &(movement->runAcceleration), 1, 30);
        ImGui::SliderFloat("Player Friction", &(movement->friction), 0.5f, 10);
        ImGui::SliderFloat("Player Max Horizontal Speed", &(movement->maxHorizontalSpeed), 0.1f, 10.f);

        ImGui::Text("Player Speed X: %f", firstLevel.getComponentFromEntity<MovementComponent>(player)->currentSpeed.x);
        ImGui::Text("Player X: %f", firstLevel.getComponentFromEntity<TransformComponent>(player)->position.x);
        ImGui::Text("Player Y: %f", firstLevel.getComponentFromEntity<TransformComponent>(player)->position.y);

        ImGui::Checkbox("Debug colliders", &s_debugCollidersEnabled);

        ImGui::DragInt("Player Hitbox Offset X", &(firstLevel.getComponentFromEntity<RectColliderComponent>(player)->collider.topLeftPointOffset.x), 1.f, 0, 18);
        ImGui::DragInt("Player Hitbox Offset Y", &(firstLevel.getComponentFromEntity<RectColliderComponent>(player)->collider.topLeftPointOffset.y), 1.f, 0, 18);
        ImGui::DragInt("Player Hitbox size X", &(firstLevel.getComponentFromEntity<RectColliderComponent>(player)->collider.size.x), 1.f, 1, 18);
        ImGui::DragInt("Player Hitbox size Y", &(firstLevel.getComponentFromEntity<RectColliderComponent>(player)->collider.size.y), 1.f, 1, 18);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Average %.1f FPS", io.Framerate);
        ImGui::Text("V-sync is %s", s_vsyncEnabled ? "enabled" : "disabled");

        ImGui::End();

        render(renderAlpha);
    }
}

void App::render(float renderAlpha)
{
    static constexpr ImVec4 backgroundColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui::Render();
    SDL_SetRenderDrawColorFloat(s_renderer, backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
    SDL_RenderClear(s_renderer);

    firstLevel.render(renderAlpha);

    // TODO: improve this Disable logical size for ImGui rendering at native resolution
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(_window, &windowWidth, &windowHeight);
    SDL_SetRenderLogicalPresentation(s_renderer, windowWidth, windowHeight, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), s_renderer);

    // Restore logical size for game rendering
    SDL_SetRenderLogicalPresentation(s_renderer, k_baseGameWidth, k_baseGameHeight, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    SDL_RenderPresent(s_renderer);
}

void App::quit()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(s_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void App::initSDL()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        D_ASSERT(false, "Failed to init sdl %s", SDL_GetError());
        return;
    }

    _window = SDL_CreateWindow("plato", k_baseGameWidth, k_baseGameHeight, 0);
    if (!_window)
    {
        D_ASSERT(false, "SDL_CreateWindow(): %s", SDL_GetError());
        return;
    }

    s_renderer = SDL_CreateRenderer(_window, nullptr);
    if (!s_renderer)
    {
        D_ASSERT(false, "SDL_CreateRenderer(): %s", SDL_GetError());
        return;
    }

    SDL_SetRenderVSync(s_renderer, s_vsyncEnabled);

    SDL_SetRenderLogicalPresentation(s_renderer, k_baseGameWidth, k_baseGameHeight, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    // Change window size after we set the render size to the original 320x180
    // Only the mouse input needs to be adjusted taking into account the current window size.
    // Everything else adapts to the window size as if the size was still 320x180
    SDL_SetWindowSize(_window, k_displayWindowWidth, k_displayWindowHeight);
    SDL_SetWindowPosition(_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void App::initImgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForSDLRenderer(_window, s_renderer);
    ImGui_ImplSDLRenderer3_Init(s_renderer);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.DisplaySize = ImVec2(static_cast<float>(k_displayWindowWidth), static_cast<float>(k_displayWindowHeight));
}

void drawImguiDockingPreview()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
    ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
    ImGui::DockSpaceOverViewport();
    ImGui::PopStyleColor(2);
}

void testResolutions(SDL_Window* window)
{
    if (wasKeyPressedThisFrame(SDL_SCANCODE_1))
    {
        SDL_SetWindowSize(window, 960, 540);
    }

    if (wasKeyPressedThisFrame(SDL_SCANCODE_2))
    {
        SDL_SetWindowSize(window, k_displayWindowWidth, k_displayWindowHeight);
    }

    if (wasKeyPressedThisFrame(SDL_SCANCODE_3))
    {
        SDL_SetWindowSize(window, 1920, 1080);
    }

    if (wasKeyPressedThisFrame(SDL_SCANCODE_4))
    {
        s_isWindowFullscreen = !s_isWindowFullscreen;
        SDL_SetWindowFullscreen(window, s_isWindowFullscreen);
    }

    if (wasKeyPressedThisFrame(SDL_SCANCODE_5))
    {
        s_vsyncEnabled = !s_vsyncEnabled;
        SDL_SetRenderVSync(s_renderer, s_vsyncEnabled);

        D_LOG(LOG, "VSync is %s", s_vsyncEnabled ? "enabled" : "disabled");
    }
}