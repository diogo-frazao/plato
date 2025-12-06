
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

#include "level.h"
#include "input.h"

void drawImguiDockingPreview();
void testResolutions(SDL_Window* window);

static bool s_isWindowFullscreen = false;

void App::run()
{
    init();
    LevelManager::getCurrentLevel()->start();
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

    uint64_t lastFrameTimestamp = SDL_GetTicks();
    float accumulator = 0.0f;

    while (true)
    {
        uint64_t currentFrameTimeStamp = SDL_GetTicks();
        uint64_t millisecondsSinceLastFrame = currentFrameTimeStamp - lastFrameTimestamp;
        millisecondsSinceLastFrame = min(millisecondsSinceLastFrame, (uint64_t)k_maxFrameTimeAllowed);

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

            _handleKeyboardInput(event);
            _handleMouseInput(event);

            //TODO: Remove press esc to close game
            if (_wasKeyPressedThisFrame(SDL_SCANCODE_ESCAPE))
            {
                return;
            }
        }

        while (accumulator >= k_targetMillisecondsBetweenFrames)
        {
            LevelManager::getCurrentLevel()->update();

#ifndef RELEASE_BUILD
            testResolutions(_window);
#endif // !RELEASE_BUILD

            _resetKeyboardAndMouseInput();
            accumulator -= k_targetMillisecondsBetweenFrames;
        }

        //TODO: Fix. Hacky fix for now for jittery movement. Force renderALpha to be 0.4 since it looks smooth
        float renderAlpha = 0.5f;

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        drawImguiDockingPreview();
        imguiThemes::green();

        if (showDemoWindow)
        {
            ImGui::ShowDemoWindow(&showDemoWindow);
        }

        LevelManager::getCurrentLevel()->imguiRender();

        render(renderAlpha);
    }
}

void App::render(float renderAlpha)
{
    ImGui::Render();
    SDL_SetRenderDrawColor(s_renderer, 255, 255, 255, 255);
    SDL_RenderClear(s_renderer);

    LevelManager::getCurrentLevel()->render(renderAlpha);

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
    if (_wasKeyPressedThisFrame(SDL_SCANCODE_1))
    {
        SDL_SetWindowSize(window, 960, 540);
    }

    if (_wasKeyPressedThisFrame(SDL_SCANCODE_2))
    {
        SDL_SetWindowSize(window, k_displayWindowWidth, k_displayWindowHeight);
    }

    if (_wasKeyPressedThisFrame(SDL_SCANCODE_3))
    {
        SDL_SetWindowSize(window, 1920, 1080);
    }

    if (_wasKeyPressedThisFrame(SDL_SCANCODE_4))
    {
        s_isWindowFullscreen = !s_isWindowFullscreen;
        SDL_SetWindowFullscreen(window, s_isWindowFullscreen);
    }

    if (_wasKeyPressedThisFrame(SDL_SCANCODE_5))
    {
        s_vsyncEnabled = !s_vsyncEnabled;
        SDL_SetRenderVSync(s_renderer, s_vsyncEnabled);

        D_LOG(LOG, "VSync is %s", s_vsyncEnabled ? "enabled" : "disabled");
    }
}