#pragma once

#include <stdint.h>
#include <string>

// ECS Related
inline constexpr int32_t k_maxNumberOfEntities = 5;
inline constexpr int8_t k_maxNumberOfComponents = 3;

inline constexpr int32_t k_invalidId = -1;
inline const std::string k_atlasFilePath = "art/atlas.png";

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
inline constexpr int32_t k_displayWindowWidth = 960;
inline constexpr int32_t k_displayWindowHeight = 540;

inline struct SDL_Renderer* s_renderer = nullptr;