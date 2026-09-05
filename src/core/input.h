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

// Don't call these directly, use the game functions instead

void _handleKeyboardInput(SDL_Event& ev);
void _handleMouseInput(SDL_Event& ev);
void _resetKeyboardAndMouseInput();

bool _isKeyDown(SDL_Scancode index);
bool _wasKeyPressedThisFrame(SDL_Scancode index);
bool _wasKeyReleasedThisFrame(SDL_Scancode index);

bool _isMouseButtonDown(MouseButton mouseButtonIndex);
bool _wasMouseButtonPressedThisFrame(MouseButton mouseButtonIndex);
bool _wasMouseButtonReleasedThisFrame(MouseButton mouseButtonIndex);
Vec2 _getMousePosition();

inline Vec2 s_mousePositionThisFrameInScreenSpace;

static MouseState s_mouseInputState;
static std::array<KeyState, 290> s_keyboardInputState;

// Game related
bool wasJumpKeyPressedThisFrame();
bool wasJumpKeyReleasedThisFrame();
bool wasRollKeyPressedThisFrame();
bool isMoveRightKeyDown();
bool isMoveLeftKeyDown();
bool wasMoveRightPressedThisFrame();
bool wasMoveLeftPressedThisFrame();
bool wasAttackKeyPressedThisFrame();
bool wasSkipDialogueKeyPressedThisFrame();
bool wasPickupPhoneKeyPressedThisFrame();
bool wasChooseDialogueOptionKeyPressedThisFrame();