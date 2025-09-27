
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

SDL_Texture* texture;

ECSLevel firstLevel;

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

void App::update()
{
    bool showDemoWindow = true;

    Entity& player = firstLevel.addEntity();
    firstLevel.addComponentToEntity<TransformComponent>(player);
    SpriteComponent* playerSprite = firstLevel.addComponentToEntity<SpriteComponent>(player);
    auto* movementComponent = firstLevel.addComponentToEntity<MovementComponent>(player);
    movementComponent->velocity = 5.f;
    playerSprite->setupWithOffsetAndSize({ 0, 0 }, { 28, 28 });

    uint64_t lastFrameTimestamp = SDL_GetTicks();
    float accumulator = 0.0f;

    while (true)
    {
        uint64_t currentFrameTimeStamp = SDL_GetTicks();
        uint64_t millisecondsSinceLastFrame = currentFrameTimeStamp - lastFrameTimestamp;
        millisecondsSinceLastFrame = max(millisecondsSinceLastFrame, k_maxFrameTimeAllowed);

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
            firstLevel.update(k_deltaTime);

            //TODO: Remove test different res
            {
                if (wasKeyPressedThisFrame(SDL_SCANCODE_0))
                {
                    SDL_SetWindowSize(_window, 1280, 720);
                }

                if (wasKeyPressedThisFrame(SDL_SCANCODE_1))
                {
                    SDL_SetWindowSize(_window, 1920, 1080);
                }

                if (wasKeyPressedThisFrame(SDL_SCANCODE_2))
                {
                    SDL_SetWindowSize(_window, k_displayWindowWidth, k_displayWindowHeight);
                }
            }

            //TODO: Remove test ECS
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                /*
                if (event.key.key == SDLK_3)
                {
                    Entity player = firstLevel.addEntity();
                    TestComponent* component = firstLevel.addComponentToEntity<TestComponent>(player);
                    if (!component)
                    {
                        D_ASSERT(false, "Ups");
                    }
                    else
                    {
                        component->velocity = 3;
                        D_LOG(MINI, "%i", component->velocity);
                    }

                    TestComponent* component2 = firstLevel.addComponentToEntity<TestComponent>(player);
                }

                if (event.key.key == SDLK_4)
                {
                    TestComponent* component = firstLevel.addComponentToEntity<TestComponent>(enemy);
                    AnotherTestComponent* anotherComponent = firstLevel.addComponentToEntity<AnotherTestComponent>(enemy);
                    anotherComponent->smell = true;

                }

                if (event.key.key == SDLK_5)
                {
                    if (firstLevel.entityHasComponent<TestComponent>(enemy))
                    {
                        D_LOG(LOG, "Enemy has Test Component");
                        if (firstLevel.entityHasComponent<AnotherTestComponent>(enemy))
                        {
                            D_LOG(LOG, "Enemy has Another Component");
                        }
                    }
                    else
                    {
                        D_LOG(ERROR, "Enemy doesn't have any components");
                    }
                }

                if (event.key.key == SDLK_6)
                {
                    firstLevel.removeComponentFromEntity<AnotherTestComponent>(enemy);
                    firstLevel.removeComponentFromEntity<TestComponent>(enemy);
                    D_LOG(MINI, "Deleted Test component from enemy");
                }

                if (event.key.key == SDLK_7)
                {
                    TestComponent* comp = firstLevel.getComponentFromEntity<TestComponent>(enemy);
                    comp->velocity = 16;
                    D_LOG(MINI, "Velocity: %i", comp->velocity);
                }

                if (event.key.key == SDLK_8)
                {
                    firstLevel.removeAllComponentsForAllEntities();
                    D_LOG(MINI, "Deleted all components");
                }
                */

            }

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
        ImGui::SliderFloat("Player Velocity", &(movement->velocity), 5, 1000);
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

    // Disable logical size for ImGui rendering at native resolution
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

void drawImguiDockingPreview()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
    ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
    ImGui::DockSpaceOverViewport();
    ImGui::PopStyleColor(2);
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

    SDL_SetRenderVSync(s_renderer, true);

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