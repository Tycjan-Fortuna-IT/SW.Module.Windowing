#include "Window.hpp"

#include <GLFW/glfw3.h>

#ifdef WINDOWING_EXPOSE_NATIVE_WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32
	#include <GLFW/glfw3native.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace SW::Windowing
{
	std::unordered_map<GLFWwindow*, Window*> Window::s_WINDOWS;

	Window::Window(const Device* device, const WindowSpecification& spec)
	    : m_Device(device), m_Title(spec.Title), m_Size{spec.Width, spec.Height},
	      m_MinimumSize{spec.MinimumWidth, spec.MinimumHeight}, m_MaximumSize{spec.MaximumWidth, spec.MaximumHeight},
	      m_IsFullScreen(spec.IsFullScreen), m_CursorMode(spec.CursorMode), m_CursorShape(spec.CursorShape)
	{
		GLFWmonitor* selectedMonitor = nullptr;

		if (m_IsFullScreen)
			selectedMonitor = glfwGetPrimaryMonitor();

		glfwWindowHint(GLFW_RESIZABLE, spec.IsResizeable);
		glfwWindowHint(GLFW_DECORATED, spec.IsDecorated);
		glfwWindowHint(GLFW_TITLEBAR, spec.HasTitlebar);
		glfwWindowHint(GLFW_FOCUSED, spec.IsFocused);
		glfwWindowHint(GLFW_MAXIMIZED, spec.IsMaximized);
		glfwWindowHint(GLFW_FLOATING, spec.IsFloating);
		glfwWindowHint(GLFW_VISIBLE, spec.IsVisible);
		glfwWindowHint(GLFW_AUTO_ICONIFY, spec.AutoIconify);
		glfwWindowHint(GLFW_REFRESH_RATE, spec.RefreshRate);

		m_Handle = glfwCreateWindow(spec.Width, spec.Height, spec.Title.c_str(), selectedMonitor, nullptr);

		VERIFY(m_Handle, "Failed to create GLFW window");

		UpdateSizeLimit();
		SetCursorMode(spec.CursorMode);
		SetCursorShape(spec.CursorShape);

		if (spec.Icon.Data != nullptr)
		{
			GLFWimage icon;
			int channels;

			icon.pixels =
			    stbi_load_from_memory(spec.Icon.Data, spec.Icon.Size, &icon.width, &icon.height, &channels, 4);

			glfwSetWindowIcon(m_Handle, 1, &icon);

			stbi_image_free(icon.pixels);
		}

		glfwGetWindowPos(m_Handle, &m_Position.first, &m_Position.second);

		s_WINDOWS[m_Handle] = this;

		glfwSetKeyCallback(m_Handle, [](GLFWwindow* glfwWindow, int key, int /*scancode*/, int action, int
		                                /*mods*/) {
			Window* window  = FindInstance(glfwWindow);
			KeyCode keyCode = (KeyCode)key;

			ASSERT(window, "Window handle is null!");

			switch (action)
			{
			case GLFW_RELEASE: {
				window->KeyReleasedEvent.Invoke(keyCode);
				break;
			}
			case GLFW_PRESS: {
				window->KeyPressedEvent.Invoke(keyCode);
				break;
			}
			case GLFW_REPEAT: {
				window->KeyRepeatEvent.Invoke(keyCode);
				break;
			}
			default:
				ASSERT(false, "Unsupported key event action: {}");
			}
		});

		glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow* glfwWindow, int button, int action, int /*mods*/) {
			Window* window      = FindInstance(glfwWindow);
			MouseCode mouseCode = (MouseCode)button;

			ASSERT(window, "Window handle is null!");

			switch (action)
			{
			case GLFW_RELEASE: {
				window->MouseButtonReleasedEvent.Invoke(mouseCode);

				break;
			}
			case GLFW_PRESS: {
				window->MouseButtonPressedEvent.Invoke(mouseCode);

				break;
			}
			default:
				ASSERT(false, "Unsupported mouse event action: {}");
			}
		});

		glfwSetWindowIconifyCallback(m_Handle, [](GLFWwindow* glfwWindow, int iconified) {
			Window* window = FindInstance(glfwWindow);

			ASSERT(window, "Window handle is null!");

			if (iconified == GLFW_TRUE)
				window->MinimizeEvent.Invoke();

			if (iconified == GLFW_FALSE)
				window->MaximizeEvent.Invoke();
		});

		glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow* glfwWindow) {
			Window* window = FindInstance(glfwWindow);

			ASSERT(window, "Window handle is null!");

			window->CloseEvent.Invoke();
		});

		glfwSetWindowSizeCallback(m_Handle, [](GLFWwindow* glfwWindow, int width, int height) {
			Window* window = FindInstance(glfwWindow);

			ASSERT(window, "Window handle is null!");

			if (glfwWindow != glfwGetCurrentContext())
				window->MakeContextCurrent();

			window->ResizeEvent.Invoke(width, height);

			// TODO: Move this to event listener
#ifdef WINDOWING_OPENGL_CONTEXT
			glViewport(0, 0, width, height);
#endif
		});

		glfwSetFramebufferSizeCallback(m_Handle, [](GLFWwindow* glfwWindow, int width, int height) {
			Window* window = FindInstance(glfwWindow);

			ASSERT(window, "Window handle is null!");

			window->FramebufferResizeEvent.Invoke(width, height);
		});

		glfwSetCursorPosCallback(m_Handle, [](GLFWwindow* glfwWindow, double x, double y) {
			Window* window = FindInstance(glfwWindow);

			ASSERT(window, "Window handle is null!");

			window->CursorMoveEvent.Invoke((float)x, (float)y);
		});

		glfwSetWindowPosCallback(m_Handle, [](GLFWwindow* glfwWindow, int x, int y) {
			Window* window = FindInstance(glfwWindow);

			ASSERT(window, "Window handle is null!");

			window->MoveEvent.Invoke(x, y);
		});

		glfwSetWindowFocusCallback(m_Handle, [](GLFWwindow* glfwWindow, int focused) {
			Window* window = FindInstance(glfwWindow);

			ASSERT(window, "Window handle is null!");

			if (focused == GLFW_TRUE)
				window->GainFocusEvent.Invoke();

			if (focused == GLFW_FALSE)
				window->LostFocusEvent.Invoke();
		});

		glfwSetTitlebarHitTestCallback(m_Handle, [](GLFWwindow* glfwWindow, int /*xPos*/, int /*yPos*/, int* hit) {
			Window* window = FindInstance(glfwWindow);

			ASSERT(window, "Window handle is null!");

			*hit = window->IsOverTitleBar() ? 1 : 0;
		});

		glfwSetScrollCallback(m_Handle, [](GLFWwindow* glfwWindow, double xOffset, double yOffset) {
			Window* window = FindInstance(glfwWindow);

			window->MouseScrollWheelEvent.Invoke(xOffset, yOffset);
		});

		ResizeEvent += std::bind_front(&Window::OnResize, this);
		MoveEvent += std::bind_front(&Window::OnMove, this);
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Handle);
	}

	Window* Window::FindInstance(GLFWwindow* glfwWindow)
	{
		return s_WINDOWS.find(glfwWindow) != s_WINDOWS.end() ? s_WINDOWS[glfwWindow] : nullptr;
	}

	void Window::SetSize(int width, int height)
	{
		glfwSetWindowSize(m_Handle, width, height);
	}

	void Window::SetMinimumSize(int minimumWidth, int minimumHeight)
	{
		m_MinimumSize.first  = minimumWidth;
		m_MinimumSize.second = minimumHeight;

		UpdateSizeLimit();
	}

	void Window::SetMaximumSize(int maximumWidth, int maximumHeight)
	{
		m_MaximumSize.first  = maximumWidth;
		m_MaximumSize.second = maximumHeight;

		UpdateSizeLimit();
	}

	void Window::SetPosition(int x, int y)
	{
		glfwSetWindowPos(m_Handle, x, y);
	}

	bool Window::IsMinimized() const
	{
		return glfwGetWindowAttrib(m_Handle, GLFW_MAXIMIZED) == GLFW_FALSE;
	}

	void Window::Minimize() const
	{
		glfwIconifyWindow(m_Handle);
	}

	bool Window::IsMaximized() const
	{
		return glfwGetWindowAttrib(m_Handle, GLFW_MAXIMIZED) == GLFW_TRUE;
	}

	void Window::Maximize() const
	{
		glfwMaximizeWindow(m_Handle);
	}

	void Window::Restore() const
	{
		glfwRestoreWindow(m_Handle);
	}

	bool Window::IsHidden() const
	{
		return glfwGetWindowAttrib(m_Handle, GLFW_VISIBLE) == GLFW_FALSE;
	}

	void Window::Hide() const
	{
		glfwHideWindow(m_Handle);
	}

	bool Window::IsVisible() const
	{
		return glfwGetWindowAttrib(m_Handle, GLFW_VISIBLE) == GLFW_TRUE;
	}

	void Window::Show() const
	{
		glfwShowWindow(m_Handle);
	}

	bool Window::IsFocused() const
	{
		return glfwGetWindowAttrib(m_Handle, GLFW_FOCUSED) == GLFW_TRUE;
	}

	void Window::Focus() const
	{
		glfwFocusWindow(m_Handle);
	}

	bool Window::IsFullscreen() const
	{
		return m_IsFullScreen;
	}

	void Window::SetFullscreen(bool value)
	{
		if (value)
			m_IsFullScreen = true;

		glfwSetWindowMonitor(m_Handle, value ? glfwGetPrimaryMonitor() : nullptr, m_Position.first, m_Position.second,
		                     m_Size.first, m_Size.second, m_RefreshRate);

		if (!value)
			m_IsFullScreen = false;
	}

	void Window::ToggleFullscreen()
	{
		SetFullscreen(!m_IsFullScreen);
	}

	bool Window::IsResizable() const
	{
		return glfwGetWindowAttrib(m_Handle, GLFW_RESIZABLE) == GLFW_TRUE;
	}

	bool Window::IsDecorated() const
	{
		return glfwGetWindowAttrib(m_Handle, GLFW_DECORATED) == GLFW_TRUE;
	}

	bool Window::HasTitlebar() const
	{
		return glfwGetWindowAttrib(m_Handle, GLFW_TITLEBAR) == GLFW_TRUE;
	}

	void Window::MakeContextCurrent() const
	{
		glfwMakeContextCurrent(m_Handle);
	}

	void Window::SwapBuffers() const
	{
		glfwSwapBuffers(m_Handle);
	}

	void Window::SetCursorMode(CursorMode cursorMode)
	{
		m_CursorMode = cursorMode;
		glfwSetInputMode(m_Handle, GLFW_CURSOR, (int)cursorMode);
	}

	void Window::SetCursorShape(CursorShape cursorShape)
	{
		m_CursorShape = cursorShape;
		glfwSetCursor(m_Handle, m_Device->GetCursorInstance(cursorShape));
	}

	void Window::SetCursorPosition(int x, int y)
	{
		glfwSetCursorPos(m_Handle, (double)x, (double)y);
	}

	void Window::SetTitle(const std::string& title)
	{
		m_Title = title;
		glfwSetWindowTitle(m_Handle, title.c_str());
	}

	std::pair<int, int> Window::GetFramebufferSize() const
	{
		int width, height;
		glfwGetFramebufferSize(m_Handle, &width, &height);

		return {width, height};
	}

	void Window::OnResize(int width, int height)
	{
		m_Size.first  = width;
		m_Size.second = height;
	}

	void Window::OnMove(int x, int y)
	{
		if (!m_IsFullScreen)
		{
			m_Position.first  = x;
			m_Position.second = y;
		}
	}

	void Window::UpdateSizeLimit() const
	{
		glfwSetWindowSizeLimits(m_Handle, m_MinimumSize.first, m_MinimumSize.second, m_MaximumSize.first,
		                        m_MaximumSize.second);
	}

	void Window::SetShouldClose(bool value) const
	{
		glfwSetWindowShouldClose(m_Handle, value);
	}

	bool Window::ShouldClose() const
	{
		return (bool)glfwWindowShouldClose(m_Handle);
	}

	void Window::RegisterOverTitlebar(bool over)
	{
		m_IsOverTitleBar = over;
	}

	float32 Window::GetDPIApproximate() const
	{
		float xscale, yscale;
		
		glfwGetWindowContentScale(m_Handle, &xscale, &yscale);
		
		return xscale;
	}

#ifdef WINDOWING_EXPOSE_NATIVE_WIN32
	HWND Window::GetWin32WindowHandle() const
	{
		return glfwGetWin32Window(m_Handle);
	}

#endif

} // namespace SW::Windowing
