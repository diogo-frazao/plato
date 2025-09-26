#pragma once

#include <stdint.h>
#include <array>
#include "lib.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_scancode.h>

struct KeyState
{
	bool isDown = false;
	bool justPressed = false;
	bool justReleased = false;
	uint8_t pressedCounter = 0;
};

struct MouseState
{
	// 0 = LEFT, 1 = MIDDLE, 2 = RIGHT
	std::array<KeyState, 3> mouseButtonsState;
	float mouseWheelScroll = 0.f;
};

enum MouseButton
{
	LEFT,
	MIDDLE,
	RIGHT
};

void handleKeyboardInput(SDL_Event& ev);
void handleMouseInput(SDL_Event& ev);
void resetKeyboardAndMouseInput();

bool isKeyDown(SDL_Scancode index);
bool wasKeyPressedThisFrame(SDL_Scancode index);
bool wasKeyReleasedThisFrame(SDL_Scancode index);

bool isMouseButtonDown(MouseButton mouseButtonIndex);
bool wasMouseButtonPressedThisFrame(MouseButton mouseButtonIndex);
bool wasMouseButtonReleasedThisFrame(MouseButton mouseButtonIndex);
Vec2 getMousePosition();

inline Vec2 s_mousePositionThisFrame;

static MouseState s_mouseInputState;
static std::array<KeyState, 290> s_keyboardInputState;
