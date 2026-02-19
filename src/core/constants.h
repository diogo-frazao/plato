#pragma once

#include <stdint.h>
#include <string>

// ECS Related
inline constexpr int32_t k_maxNumberOfEntities = 500;
inline constexpr uint16_t k_maxNumberOfSystems = 10;
inline constexpr uint32_t k_playerEntityId = 0;
inline constexpr uint32_t k_crosshairEntityId = 1;

// General
inline constexpr int32_t k_invalidId = -1;
inline constexpr float k_invalidTime = -1.f;
inline constexpr uint16_t k_maxAtlasFiles = 2;
inline constexpr uint16_t k_maxLevels = 1;
inline bool s_isImGuiOpen = true;

// Frame rate related
// Milliseconds divided by target frame rate for physics/game logic
constexpr float k_targetMillisecondsBetweenFrames = 1000.f / 60.f;
// Delta time in seconds, convert from ms to s
constexpr float k_deltaTime = k_targetMillisecondsBetweenFrames / 1000.f;
// Even if two frames take too long to render, never go above 250 milliseconds to avoid spiral of death
constexpr uint16_t k_maxFrameTimeAllowed = 250;

// Window Related
inline constexpr uint16_t k_baseGameWidth = 320;
inline constexpr uint16_t k_baseGameHeight = 180;
inline constexpr int32_t k_displayWindowWidth = 1280;
inline constexpr int32_t k_displayWindowHeight = 720;

inline struct SDL_Renderer* s_renderer = nullptr;
inline bool s_vsyncEnabled = true;
inline bool s_debugCollidersEnabled = false;
inline bool s_debugGridEnabled = false;