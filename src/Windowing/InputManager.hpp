/**
 * @file InputManager.hpp
 *
 * @copyright Copyright (c) 2024 Tycjan Fortuna
 */
#pragma once

#include <map>

#include <Eventing/Eventing.hpp>

#include "Windowing/KeyCode.hpp"
#include "Windowing/MouseCode.hpp"
#include "Windowing/Window.hpp"

namespace SW::Windowing
{
	enum class ClickableState
	{
		// The key or mouse button was released
		Released,

		// The key or mouse button was pressed
		Pressed,

		// The key or mouse button was repeated
		Repeated,

		// The key or mouse button was not touched
		None
	};

	class InputManager
	{
	public:
		InputManager(Window* window);
		~InputManager();

		// Update pressed keys state to held if pressed for one frame. Updates the state faster that GLFW wait after
		// press.
		// Be aware that this function should be called at the beginning of the frame.
		void UpdateKeysStateIfNecessary();

		// Clears the released keys.
		// Be aware that this function should be called at the end of the frame.
		void ClearReleasedKeys();

		// Checks if the specified key is pressed. Only once per frame.
		bool IsKeyPressed(KeyCode key) const;

		// Checks if the specified key is held down.
		bool IsKeyHeld(KeyCode key) const;

		// Checks if the specified key is down. Meaning it's either pressed or held down.
		bool IsKeyDown(KeyCode key) const;

		// Checks if the specified key is released.
		bool IsKeyReleased(KeyCode key) const;

		// Checks if the specified mouse button is pressed.
		bool IsMouseButtonPressed(MouseCode button) const;

		// Checks if the specified mouse button is held down.
		bool IsMouseButtonHeld(MouseCode button) const;

		// Checks if the specified mouse button is down. Meaning it's either pressed or held down.
		bool IsMouseButtonDown(MouseCode button) const;

		// Checks if the specified mouse button is released.
		bool IsMouseButtonReleased(MouseCode button) const;

		std::pair<float, float> GetMousePosition();
		void SetMousePosition(const std::pair<float, float>& position);

	private:
		// Updates the state of the specified key.
		void UpdateKeyState(KeyCode code, ClickableState state);

		// Updates the state of the specified mouse button.
		void UpdateMouseState(MouseCode code, ClickableState state);

	private:
		Window* m_Window = nullptr;

		Eventing::ListenerID m_KeyPressedListener;
		Eventing::ListenerID m_KeyReleasedListener;
		Eventing::ListenerID m_KeyRepeatListener;

		Eventing::ListenerID m_MouseButtonPressedListener;
		Eventing::ListenerID m_MouseButtonReleasedListener;

	private:
		// The cached states of the keys
		std::map<KeyCode, ClickableState> m_KeyStates;

		// The cached states of the mouse buttons
		std::map<MouseCode, ClickableState> m_MouseStates;
	};
} // namespace SW::Windowing
