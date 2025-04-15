/**
 * @file Window.hpp
 *
 * @copyright Copyright (c) 2024 Tycjan Fortuna
 */
#pragma once

#include <string>

#ifdef WINDOWING_EXPOSE_NATIVE_WIN32
	#include <Windows.h>
#endif

#include <Eventing/Eventing.hpp>

#include "Windowing/Device.hpp"
#include "Windowing/KeyCode.hpp"
#include "Windowing/MouseCode.hpp"

struct GLFWwindow;

namespace SW::Windowing
{
	using uchar = unsigned char;

	struct EmbeddedIcon
	{
		// Embedded binary icon of the application.
		const uchar* Data = nullptr;

		// Number of embedded binary fragments.
		const int Size = 0;
	};

	struct WindowSpecification
	{
		static const int DontCare = -1;

		std::string Title = "No title";

		int Width  = 1280;
		int Height = 720;

		// Window's icon, shown only if toolbar is enabled
		EmbeddedIcon Icon = {};

		int MinimumWidth  = WindowSpecification::DontCare;
		int MinimumHeight = WindowSpecification::DontCare;
		int MaximumWidth  = WindowSpecification::DontCare;
		int MaximumHeight = WindowSpecification::DontCare;

		// Specifies if the window is by default in full screen or windowed mode
		bool IsFullScreen = false;

		// Specifies whether the windowed mode window will have window decorations such as a border, widgets etc.
		// This hint is ignored for full screen windows
		bool IsDecorated = true;

		// Specifies whether the window will have the titlebar
		bool HasTitlebar = true;

		// Specifies whether the windowed mode window will be resizable by the user. (SetSize method)
		// This hint is ignored for full screen windows
		bool IsResizeable = true;

		// Specifies whether the windowed mode window will be given input focus when created
		// This hint is ignored for full screen and initially hidden windows
		bool IsFocused = true;

		// Specifies whether the windowed mode window will be maximized when created
		// This hint is ignored for full screen windows
		bool IsMaximized = false;

		// Specifies whether the windowed mode window will be floating above other regular windows
		// This hint is ignored for full screen windows
		bool IsFloating = false;

		// Specifies whether the windowed mode window will be initially visible
		// This hint is ignored for full screen windows
		bool IsVisible = true;

		// Specifies whether the full screen window will automatically iconify and restore
		// the previous video mode on input focus loss.
		// This hint is ignored for windowed mode windows
		bool AutoIconify = true;

		int RefreshRate = WindowSpecification::DontCare;

		CursorMode CursorMode = CursorMode::NORMAL;

		CursorShape CursorShape = CursorShape::ARROW;
	};

	class Window
	{
	public:
		Window(const Device* device, const WindowSpecification& spec);
		~Window();

		static Window* FindInstance(GLFWwindow* glfwWindow);

		int GetWidth() const { return m_Size.first; }
		int GetHeight() const { return m_Size.second; }

		std::pair<int, int> GetSize() const { return m_Size; }
		void SetSize(int width, int height);

		// Use WindowSpecification::DontCare for no limitation
		std::pair<int, int> GetMinimumSize() const { return m_MinimumSize; }
		void SetMinimumSize(int minimumWidth, int minimumHeight);

		std::pair<int, int> GetMaximumSize() const { return m_MaximumSize; }
		void SetMaximumSize(int maximumWidth, int maximumHeight);

		std::pair<int, int> GetPosition() const { return m_Position; }
		void SetPosition(int x, int y);

		bool IsMinimized() const;
		void Minimize() const;

		bool IsMaximized() const;
		void Maximize() const;

		// (un-maximize)
		void Restore() const;

		bool IsHidden() const;
		void Hide() const;

		bool IsVisible() const;
		void Show() const;

		bool IsFocused() const;
		void Focus() const;

		// Set the should close flag of the window
		void SetShouldClose(bool value) const;
		// Return true if the window should close
		bool ShouldClose() const;

		// Set the state of titlebar drag, used for the window handle to allow for custom way of drag
		// (Whether to enable or disable custom drag)
		void RegisterOverTitlebar(bool over);
		bool IsOverTitleBar() const { return m_IsOverTitleBar; }

		bool IsFullscreen() const;
		void SetFullscreen(bool value);
		void ToggleFullscreen();

		bool IsResizable() const;
		bool IsDecorated() const;
		bool HasTitlebar() const;

		// Define the window as the current context
		void MakeContextCurrent() const;

		// Handle the buffer swapping with the current window
		void SwapBuffers() const;

		CursorMode GetCursorMode() const { return m_CursorMode; }
		void SetCursorMode(CursorMode cursorMode);

		CursorShape GetCursorShape() const { return m_CursorShape; }
		void SetCursorShape(CursorShape cursorShape);

		void SetCursorPosition(int x, int y);

		std::string GetTitle() const { return m_Title; }
		void SetTitle(const std::string& title);

		int GetRefreshRate() const { return m_RefreshRate; }
		void SetRefreshRate(int refreshRate) { m_RefreshRate = refreshRate; }

		// Return the framebuffer size (Viewport size)
		std::pair<int, int> GetFramebufferSize() const;

		// Return the framebuffer size (Viewport size) in pixels per inch (DPI)
		// This is an approximation, as the DPI can be different for each axis
		float32 GetDPIApproximate() const;

		GLFWwindow* GetWindowHandle() const { return m_Handle; }

#ifdef WINDOWING_EXPOSE_NATIVE_WIN32
		HWND GetWin32WindowHandle() const;
#endif

	public:
		// Inputs relative events
		Eventing::Event<Windowing::KeyCode> KeyPressedEvent;
		Eventing::Event<Windowing::KeyCode> KeyReleasedEvent;
		Eventing::Event<Windowing::KeyCode> KeyRepeatEvent;

		Eventing::Event<Windowing::MouseCode> MouseButtonPressedEvent;
		Eventing::Event<Windowing::MouseCode> MouseButtonReleasedEvent;
		Eventing::Event<float, float> MouseScrollWheelEvent;

		// Window events
		Eventing::Event<int, int> ResizeEvent;
		Eventing::Event<int, int> FramebufferResizeEvent;
		Eventing::Event<int, int> MoveEvent;
		Eventing::Event<float, float> CursorMoveEvent;

		Eventing::Event<> MinimizeEvent;
		Eventing::Event<> MaximizeEvent;
		Eventing::Event<> GainFocusEvent;
		Eventing::Event<> LostFocusEvent;
		Eventing::Event<> CloseEvent;

	private:
		void OnResize(int width, int height);
		void OnMove(int x, int y);

		void UpdateSizeLimit() const;

	private:
		const Device* m_Device = nullptr;
		GLFWwindow* m_Handle   = nullptr;

		std::string m_Title;

		std::pair<int, int> m_Size;
		std::pair<int, int> m_MinimumSize;
		std::pair<int, int> m_MaximumSize;
		std::pair<int, int> m_Position;

		bool m_IsFullScreen;
		bool m_IsOverTitleBar;

		int m_RefreshRate;

		CursorMode m_CursorMode;
		CursorShape m_CursorShape;

	private:
		static std::unordered_map<GLFWwindow*, Window*> s_WINDOWS;
	};
} // namespace SW::Windowing
