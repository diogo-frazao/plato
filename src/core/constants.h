#pragma once

#include <stdint.h>
#include <string>

// General
inline constexpr int32_t k_invalidId = -1;
inline constexpr float k_invalidTime = -1.f;
inline constexpr uint16_t k_maxAtlasFiles = 3;
inline constexpr uint16_t k_maxLevels = 1;
inline bool s_isImGuiOpen = true;

// ECS Related
inline constexpr uint8_t k_entityMaxNameCharacters = 64;
inline constexpr int32_t k_maxNumberOfEntities = 500;
inline constexpr uint16_t k_maxNumberOfSystems = 10;
inline constexpr uint32_t k_playerEntityId = 0;
inline constexpr uint32_t k_crosshairEntityId = 1;
inline constexpr uint32_t k_cellphoneEntityId = 2;

// These entities ids change at runtime depending on the level
// When the entities are created, we must update these values to point to the correct entity
inline int32_t s_darwinEntityId = k_invalidId;
inline int32_t s_oskarEntityId = k_invalidId;
inline int32_t s_hugoEntityId = k_invalidId;

inline constexpr uint8_t k_maxNumberOfMovementAnimations = 1;

// Dialogue
// Expand as we support more languages/characters
inline const uint16_t k_maxFontGlyphs = 128;
inline const char* k_fontAtlasLayout = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.:,;''(!?)+-*/=__[]{}|#$%&<>^@~";
inline const uint16_t k_maxCharactersPerDialogue = 200;
inline const uint8_t k_maxDialogueOptions = 3;

// Frame rate related
// Milliseconds divided by target frame rate for physics/game logic
constexpr float k_targetFrameRateForGameLogic = 60.f;
constexpr float k_targetMillisecondsBetweenFrames = 1000.f / k_targetFrameRateForGameLogic;
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
inline bool s_debugUICollidersEnabled = false;
inline bool s_debugGridEnabled = false;

//Gameplay related
inline int8_t s_playerTension = 0;