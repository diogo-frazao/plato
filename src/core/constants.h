#pragma once

#include <stdint.h>
#include <string>

// ECS Related
inline constexpr int32_t k_maxNumberOfEntities = 5;
inline constexpr int8_t k_maxNumberOfComponents = 3;

inline constexpr int32_t k_invalidId = -1;
inline const std::string k_atlasFilePath = "art/atlas.png";

inline constexpr uint16_t k_baseGameWidth = 320;
inline constexpr uint16_t k_baseGameHeight = 180;
inline constexpr int32_t k_displayWindowWidth = 960;
inline constexpr int32_t k_displayWindowHeight = 540;

inline struct SDL_Renderer* s_renderer = nullptr;