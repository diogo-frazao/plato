
#include "app.h"

#include "log.h"
#include "imgui.h"
#include "imguiThemes.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include <stdio.h>
#include <SDL3/SDL.h>

static constexpr uint16_t k_baseGameWidth = 320;
static constexpr uint16_t k_baseGameHeight = 180;

static constexpr int32_t k_displayWindowWidth = 1280;
static constexpr int32_t k_displayWindowHeight = 720;

void drawImguiDockingPreview();

void App::run()
{
    init();
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

    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);

            bool didRequestWindowClose = event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(_window);
            if (event.type == SDL_EVENT_QUIT || didRequestWindowClose)
            {
                return;
            }
        }

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        drawImguiDockingPreview();
        imguiThemes::green();

        if (showDemoWindow)
        {
            ImGui::ShowDemoWindow(&showDemoWindow);
        }

        render();
    }
}

void App::render()
{
    static constexpr ImVec4 backgroundColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui::Render();
    SDL_SetRenderDrawColorFloat(_renderer, backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
    SDL_RenderClear(_renderer);

    // Disable logical size for ImGui rendering at native resolution
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(_window, &windowWidth, &windowHeight);
    SDL_SetRenderLogicalPresentation(_renderer, windowWidth, windowHeight, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), _renderer);

    // Restore logical size for game rendering
    SDL_SetRenderLogicalPresentation(_renderer, k_baseGameWidth, k_baseGameHeight, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    SDL_RenderPresent(_renderer);
}

void App::quit()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(_renderer);
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

    _window = SDL_CreateWindow("plato", k_baseGameWidth, k_baseGameHeight, SDL_WINDOW_RESIZABLE);
    if (!_window)
    {
        D_ASSERT(false, "SDL_CreateWindow(): %s", SDL_GetError());
        return;
    }

    _renderer = SDL_CreateRenderer(_window, nullptr);
    if (!_renderer)
    {
        D_ASSERT(false, "SDL_CreateRenderer(): %s", SDL_GetError());
        return;
    }

    SDL_SetRenderVSync(_renderer, true);

    SDL_SetRenderLogicalPresentation(_renderer, k_baseGameWidth, k_baseGameHeight, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
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
    ImGui_ImplSDL3_InitForSDLRenderer(_window, _renderer);
    ImGui_ImplSDLRenderer3_Init(_renderer);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.DisplaySize = ImVec2(static_cast<float>(k_displayWindowWidth), static_cast<float>(k_displayWindowHeight));
}