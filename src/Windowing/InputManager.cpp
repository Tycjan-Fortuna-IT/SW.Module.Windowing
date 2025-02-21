#include "InputManager.hpp"

#include <GLFW/glfw3.h>

namespace SW::Windowing
{

	InputManager::InputManager(Window* window) : m_Window(window)
	{
		m_KeyPressedListener = m_Window->KeyPressedEvent +=
		    [this](KeyCode keyCode) { UpdateKeyState(keyCode, ClickableState::Pressed); };

		m_KeyReleasedListener = m_Window->KeyReleasedEvent +=
		    [this](KeyCode keyCode) { UpdateKeyState(keyCode, ClickableState::Released); };

		m_KeyRepeatListener = m_Window->KeyRepeatEvent +=
		    [this](KeyCode keyCode) { UpdateKeyState(keyCode, ClickableState::Repeated); };

		m_MouseButtonPressedListener = m_Window->MouseButtonPressedEvent +=
		    [this](MouseCode mouseCode) { UpdateMouseState(mouseCode, ClickableState::Repeated); };

		m_MouseButtonReleasedListener = m_Window->MouseButtonReleasedEvent +=
		    [this](MouseCode mouseCode) { UpdateMouseState(mouseCode, ClickableState::Repeated); };
	}

	InputManager::~InputManager()
	{
		m_Window->KeyPressedEvent -= m_KeyPressedListener;
		m_Window->KeyReleasedEvent -= m_KeyReleasedListener;
		m_Window->KeyRepeatEvent -= m_KeyRepeatListener;
		m_Window->MouseButtonPressedEvent -= m_MouseButtonPressedListener;
		m_Window->MouseButtonReleasedEvent -= m_MouseButtonReleasedListener;
	}

	void InputManager::UpdateKeysStateIfNecessary()
	{
		for (auto& [code, state] : m_KeyStates)
		{
			if (state == ClickableState::Pressed)
			{
				state = ClickableState::Repeated;
			}
		}

		for (auto& [code, state] : m_MouseStates)
		{
			if (state == ClickableState::Pressed)
			{
				state = ClickableState::Repeated;
			}
		}
	}

	void InputManager::ClearReleasedKeys()
	{
		for (auto& [code, state] : m_KeyStates)
		{
			if (state == ClickableState::Released)
			{
				state = ClickableState::None;
			}
		}

		for (auto& [code, state] : m_MouseStates)
		{
			if (state == ClickableState::Released)
			{
				state = ClickableState::None;
			}
		}
	}

	bool InputManager::IsKeyPressed(KeyCode key) const
	{
		auto val = m_KeyStates.find(key);

		return val == m_KeyStates.end() ? false : val->second == ClickableState::Pressed;
	}

	bool InputManager::IsKeyHeld(KeyCode key) const
	{
		auto val = m_KeyStates.find(key);

		return val == m_KeyStates.end() ? false : val->second == ClickableState::Repeated;
	}

	bool InputManager::IsKeyDown(KeyCode key) const
	{
		auto val = m_KeyStates.find(key);

		return val == m_KeyStates.end()
		           ? false
		           : (val->second == ClickableState::Pressed || val->second == ClickableState::Repeated);
	}

	bool InputManager::IsKeyReleased(KeyCode key) const
	{
		auto val = m_KeyStates.find(key);

		return val == m_KeyStates.end() ? false : val->second == ClickableState::Released;
	}

	bool InputManager::IsMouseButtonPressed(MouseCode button) const
	{
		auto val = m_MouseStates.find(button);

		return val == m_MouseStates.end() ? false : val->second == ClickableState::Pressed;
	}

	bool InputManager::IsMouseButtonHeld(MouseCode button) const
	{
		auto val = m_MouseStates.find(button);

		return val == m_MouseStates.end() ? false : val->second == ClickableState::Repeated;
	}

	bool InputManager::IsMouseButtonDown(MouseCode button) const
	{
		auto val = m_MouseStates.find(button);

		return val == m_MouseStates.end()
		           ? false
		           : (val->second == ClickableState::Pressed || val->second == ClickableState::Repeated);
	}

	bool InputManager::IsMouseButtonReleased(MouseCode button) const
	{
		auto val = m_MouseStates.find(button);

		return val == m_MouseStates.end() ? false : val->second == ClickableState::Released;
	}

	std::pair<float, float> InputManager::GetMousePosition()
	{
		double x, y;
		glfwGetCursorPos(m_Window->GetWindowHandle(), &x, &y);

		return {(float)x, (float)y};
	}

	void InputManager::SetMousePosition(const std::pair<float, float>& position)
	{
		glfwSetCursorPos(m_Window->GetWindowHandle(), (double)position.first, (double)position.second);
	}

	void InputManager::UpdateKeyState(KeyCode code, ClickableState state)
	{
		m_KeyStates[code] = state;
	}

	void InputManager::UpdateMouseState(MouseCode code, ClickableState state)
	{
		m_MouseStates[code] = state;
	}

} // namespace SW::Windowing
