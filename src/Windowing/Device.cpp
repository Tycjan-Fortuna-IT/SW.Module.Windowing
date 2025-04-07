#include "Device.hpp"

#include <GLFW/glfw3.h>
#include <stb_image.h>

namespace SW::Windowing
{

	SW::Eventing::Event<int, std::string> Device::ErrorEvent;

	Device::Device(const DeviceSpecification& spec)
	{
		glfwSetErrorCallback([](int code, const char* description) { ErrorEvent.Invoke(code, description); });

		ASSERT(glfwInit(), "Failed to initialize GLFW");

		auto loadCursor = [this](const char* path, CursorShape shape) -> void {
			if (path != nullptr)
			{
				GLFWimage icon;

				int channels;

				icon.pixels = stbi_load(path, &icon.width, &icon.height, &channels, 4);
				VERIFY(icon.pixels, "Failed to load cursor image: {}", path);

				m_Cursors[shape] = glfwCreateCursor(&icon, 0, 0);
			}
			else
			{
				m_Cursors[shape] = glfwCreateStandardCursor((int)shape);
			}
		};

		loadCursor(spec.CrosshairSpec.CursorArrowTexturePath, CursorShape::ARROW);
		loadCursor(spec.CrosshairSpec.CursorIBeamTexturePath, CursorShape::IBEAM);
		loadCursor(spec.CrosshairSpec.CursorCrosshairTexturePath, CursorShape::CROSSHAIR);
		loadCursor(spec.CrosshairSpec.CursorPointingHandTexturePath, CursorShape::POINTING_HAND);
		loadCursor(spec.CrosshairSpec.CursorResizeEWTexturePath, CursorShape::RESIZE_EW);
		loadCursor(spec.CrosshairSpec.CursorResizeNSTexturePath, CursorShape::RESIZE_NS);
		loadCursor(spec.CrosshairSpec.CursorResizeNWSEResizeTexturePath, CursorShape::RESIZE_NWSE);
		loadCursor(spec.CrosshairSpec.CursorResizeNESWTexturePath, CursorShape::RESIZE_NESW);
		loadCursor(spec.CrosshairSpec.CursorResizeAllTexturePath, CursorShape::RESIZE_ALL);
		loadCursor(spec.CrosshairSpec.CursorNotAllowedTexturePath, CursorShape::NOT_ALLOWED);

		int clientApi = (spec.Api == ClientApi::OpenGL)     ? GLFW_OPENGL_API
		                : (spec.Api == ClientApi::OpenGLES) ? GLFW_OPENGL_ES_API
		                                                    : GLFW_NO_API;

		glfwWindowHint(GLFW_CLIENT_API, clientApi);

#ifdef WINDOWING_OPENGL_CONTEXT
		if (spec.DebugProfile)
		{
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, spec.GLMajorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, spec.GLMinorVersion);

		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, spec.Samples);
#endif
	}

	Device::~Device()
	{
		glfwDestroyCursor(m_Cursors[CursorShape::ARROW]);
		glfwDestroyCursor(m_Cursors[CursorShape::IBEAM]);
		glfwDestroyCursor(m_Cursors[CursorShape::CROSSHAIR]);
		glfwDestroyCursor(m_Cursors[CursorShape::POINTING_HAND]);
		glfwDestroyCursor(m_Cursors[CursorShape::RESIZE_EW]);
		glfwDestroyCursor(m_Cursors[CursorShape::RESIZE_NS]);
		glfwDestroyCursor(m_Cursors[CursorShape::RESIZE_NWSE]);
		glfwDestroyCursor(m_Cursors[CursorShape::RESIZE_NESW]);
		glfwDestroyCursor(m_Cursors[CursorShape::RESIZE_ALL]);
		glfwDestroyCursor(m_Cursors[CursorShape::NOT_ALLOWED]);

		glfwTerminate();
	}

	std::pair<int, int> Device::GetPrimaryMonitorSize() const
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		return {mode->width, mode->height};
	}

	GLFWcursor* Device::GetCursorInstance(CursorShape shape) const
	{
		return m_Cursors.at(shape);
	}

	bool Device::IsVSyncEnabled() const
	{
		return m_VSync;
	}

	void Device::SetVSync(bool enabled)
	{
		glfwSwapInterval(enabled ? 1 : 0);

		m_VSync = enabled;
	}

	void Device::PollEvents() const
	{
		glfwPollEvents();
	}

	float Device::GetElapsedTime() const
	{
		return (float)glfwGetTime();
	}

} // namespace SW::Windowing
