#include "input.h"

#include "log.h"
#include "constants.h"
#include <SDL3/SDL_Render.h>

void _calculateMousePositionThisFrame()
{
	s_mousePositionThisFrameInScreenSpace = _getMousePosition();
}

Vec2 _getMousePosition()
{
	float windowMouseX;
	float windowMouseY;
	SDL_GetMouseState(&windowMouseX, &windowMouseY);

	float logicalMouseX;
	float logicalMouseY;
	SDL_RenderCoordinatesFromWindow(s_renderer, windowMouseX, windowMouseY, &logicalMouseX, &logicalMouseY);

	return Vec2(logicalMouseX, logicalMouseY);
}

void _handleKeyboardInput(SDL_Event& ev)
{
	if (ev.type != SDL_EVENT_KEY_DOWN && ev.type != SDL_EVENT_KEY_UP)
	{
		return;
	}

	SDL_Scancode key = ev.key.scancode;
	if (key == SDL_SCANCODE_UNKNOWN)
	{
		D_ASSERT(false, "Unkown key pressed");
		return;
	}

	KeyState& keyState = s_keyboardInputState[key];

	if (ev.type == SDL_EVENT_KEY_DOWN)
	{
		keyState.isDown = true;
		keyState.pressedCounter++;
		keyState.justPressed = keyState.pressedCounter == 1;
	}
	else if (ev.type == SDL_EVENT_KEY_UP)
	{
		keyState.isDown = false;
		keyState.pressedCounter = 0;
		keyState.justReleased = true;
	}
}

void _handleMouseInput(SDL_Event& ev)
{
	_calculateMousePositionThisFrame();

	if (ev.type == SDL_EVENT_MOUSE_WHEEL)
	{
		s_mouseInputState.mouseWheelScroll = static_cast<float>(ev.wheel.y);
	}

	if (ev.type != SDL_EVENT_MOUSE_BUTTON_DOWN && ev.type != SDL_EVENT_MOUSE_BUTTON_UP)
	{
		return;
	}

	uint8_t buttonIndex = ev.button.button;
	KeyState& mouseButtonState = s_mouseInputState.mouseButtonsState[buttonIndex - 1];

	if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
	{
		mouseButtonState.isDown = true;
		mouseButtonState.pressedCounter++;
		mouseButtonState.justPressed = mouseButtonState.pressedCounter == 1;
	}
	else if (ev.type == SDL_EVENT_MOUSE_BUTTON_UP)
	{
		mouseButtonState.isDown = false;
		mouseButtonState.pressedCounter = 0;
		mouseButtonState.justReleased = true;
	}
}

void _resetKeyboardAndMouseInput()
{
	// Reset was just press / was just release keys
	for (KeyState& keyState : s_keyboardInputState)
	{
		keyState.justPressed = 0;
		keyState.justReleased = 0;
	}

	for (KeyState& keyState : s_mouseInputState.mouseButtonsState)
	{
		keyState.justPressed = 0;
		keyState.justReleased = 0;
	}

	s_mouseInputState.mouseWheelScroll = 0;
}

bool _isKeyDown(SDL_Scancode index)
{
	if (index < 0 || index > s_keyboardInputState.size() - 1)
	{
		D_ASSERT(false, "Trying to access invalid array index");
		return false;
	}

	return s_keyboardInputState[index].isDown;
}

bool _wasKeyPressedThisFrame(SDL_Scancode index)
{
	if (index < 0 || index > s_keyboardInputState.size() - 1)
	{
		D_ASSERT(false, "Trying to access invalid array index");
		return false;
	}

	return s_keyboardInputState[index].justPressed;
}

bool _wasKeyReleasedThisFrame(SDL_Scancode index)
{
	if (index < 0 || index > s_keyboardInputState.size() - 1)
	{
		D_ASSERT(false, "Trying to access invalid array index");
		return false;
	}

	return s_keyboardInputState[index].justReleased;
}

bool _isMouseButtonDown(MouseButton mouseButtonIndex)
{
	if (mouseButtonIndex < 0 || mouseButtonIndex > s_mouseInputState.mouseButtonsState.size() - 1)
	{
		D_ASSERT(false, "Trying to access invalid array index");
		return false;
	}

	return s_mouseInputState.mouseButtonsState[mouseButtonIndex].isDown;
}

bool _wasMouseButtonPressedThisFrame(MouseButton mouseButtonIndex)
{
	if (mouseButtonIndex < 0 || mouseButtonIndex > s_mouseInputState.mouseButtonsState.size() - 1)
	{
		D_ASSERT(false, "Trying to access invalid array index");
		return false;
	}

	return s_mouseInputState.mouseButtonsState[mouseButtonIndex].justPressed;
}

bool _wasMouseButtonReleasedThisFrame(MouseButton mouseButtonIndex)
{
	if (mouseButtonIndex < 0 || mouseButtonIndex > s_mouseInputState.mouseButtonsState.size() - 1)
	{
		D_ASSERT(false, "Trying to access invalid array index");
		return false;
	}

	return s_mouseInputState.mouseButtonsState[mouseButtonIndex].justReleased;
}

// Game related
// TODO: Expand to allow gamepad support

static SDL_Scancode s_jumpKey = SDL_SCANCODE_W;
static SDL_Scancode s_alternateJumpKey = SDL_SCANCODE_UP;

static SDL_Scancode s_moveRightKey = SDL_SCANCODE_D;
static SDL_Scancode s_alternateMoveRightKey = SDL_SCANCODE_RIGHT;
static SDL_Scancode s_moveLeftKey = SDL_SCANCODE_A;
static SDL_Scancode s_alternateMoveLeftKey = SDL_SCANCODE_LEFT;

static SDL_Scancode s_rollKey = SDL_SCANCODE_DOWN;

bool wasRollKeyPressedThisFrame()
{
	return _wasKeyPressedThisFrame(s_rollKey);
}

bool wasJumpKeyPressedThisFrame()
{
	return _wasKeyPressedThisFrame(s_jumpKey) || _wasKeyPressedThisFrame(s_alternateJumpKey);
}

bool wasJumpKeyReleasedThisFrame()
{
	return _wasKeyReleasedThisFrame(s_jumpKey) || _wasKeyReleasedThisFrame(s_alternateJumpKey);
}

bool isMoveRightKeyDown()
{
	return _isKeyDown(s_moveRightKey) || _isKeyDown(s_alternateMoveRightKey);
}

bool isMoveLeftKeyDown()
{
	return _isKeyDown(s_moveLeftKey) || _isKeyDown(s_alternateMoveLeftKey);
}

bool wasMoveRightPressedThisFrame()
{
	return _wasKeyPressedThisFrame(s_moveRightKey) || _wasKeyPressedThisFrame(s_alternateMoveRightKey);
}

bool wasMoveLeftPressedThisFrame()
{
	return _wasKeyPressedThisFrame(s_moveLeftKey) || _wasKeyPressedThisFrame(s_alternateMoveLeftKey);
}

bool wasAttackKeyPressedThisFrame()
{
	return _wasMouseButtonPressedThisFrame(LEFT) || _wasKeyPressedThisFrame(SDL_SCANCODE_RETURN) || _wasKeyPressedThisFrame(SDL_SCANCODE_Z); 
}

bool wasSkipDialogueKeyPressedThisFrame()
{
	return wasAttackKeyPressedThisFrame() || _wasKeyPressedThisFrame(SDL_SCANCODE_RETURN);
}

bool wasPickupPhoneKeyPressedThisFrame()
{
	return wasJumpKeyPressedThisFrame();
}

bool wasChooseDialogueOptionKeyPressedThisFrame()
{
	return wasSkipDialogueKeyPressedThisFrame();
}

