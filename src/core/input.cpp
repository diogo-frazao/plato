#include "input.h"

#include "log.h"
#include "constants.h"
#include <SDL3/SDL_Render.h>

void calculateMousePositionThisFrame()
{
	s_mousePositionThisFrame = getMousePosition();
}

Vec2 getMousePosition()
{
	float windowMouseX;
	float windowMouseY;
	SDL_GetMouseState(&windowMouseX, &windowMouseY);

	float logicalMouseX;
	float logicalMouseY;
	SDL_RenderCoordinatesFromWindow(s_renderer, windowMouseX, windowMouseY, &logicalMouseX, &logicalMouseY);

	return Vec2(logicalMouseX, logicalMouseY);
}

void handleKeyboardInput(SDL_Event& ev)
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

void handleMouseInput(SDL_Event& ev)
{
	calculateMousePositionThisFrame();

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

void resetKeyboardAndMouseInput()
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

bool isKeyDown(SDL_Scancode index)
{
	if (index < 0 || index > s_keyboardInputState.size() - 1)
	{
		D_ASSERT(false, "Trying to access invalid array index");
		return false;
	}

	return s_keyboardInputState[index].isDown;
}

bool wasKeyPressedThisFrame(SDL_Scancode index)
{
	if (index < 0 || index > s_keyboardInputState.size() - 1)
	{
		D_ASSERT(false, "Trying to access invalid array index");
		return false;
	}

	return s_keyboardInputState[index].justPressed;
}

bool wasKeyReleasedThisFrame(SDL_Scancode index)
{
	if (index < 0 || index > s_keyboardInputState.size() - 1)
	{
		D_ASSERT(false, "Trying to access invalid array index");
		return false;
	}

	return s_keyboardInputState[index].justReleased;
}

bool isMouseButtonDown(MouseButton mouseButtonIndex)
{
	if (mouseButtonIndex < 0 || mouseButtonIndex > s_mouseInputState.mouseButtonsState.size() - 1)
	{
		D_ASSERT(false, "Trying to access invalid array index");
		return false;
	}

	return s_mouseInputState.mouseButtonsState[mouseButtonIndex].isDown;
}

bool wasMouseButtonPressedThisFrame(MouseButton mouseButtonIndex)
{
	if (mouseButtonIndex < 0 || mouseButtonIndex > s_mouseInputState.mouseButtonsState.size() - 1)
	{
		D_ASSERT(false, "Trying to access invalid array index");
		return false;
	}

	return s_mouseInputState.mouseButtonsState[mouseButtonIndex].justPressed;
}

bool wasMouseButtonReleasedThisFrame(MouseButton mouseButtonIndex)
{
	if (mouseButtonIndex < 0 || mouseButtonIndex > s_mouseInputState.mouseButtonsState.size() - 1)
	{
		D_ASSERT(false, "Trying to access invalid array index");
		return false;
	}

	return s_mouseInputState.mouseButtonsState[mouseButtonIndex].justReleased;
}


