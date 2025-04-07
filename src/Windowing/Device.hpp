/**
 * @file Device.hpp
 *
 * @copyright Copyright (c) 2024 Tycjan Fortuna
 */
#pragma once

#include <Eventing/Eventing.hpp>

struct GLFWwindow;
struct GLFWcursor;

namespace SW::Windowing
{

	enum class CursorMode
	{
		NORMAL   = 0x00034001,
		HIDDEN   = 0x00034002,
		DISABLED = 0x00034003,
		CAPTURED = 0x00034004,
	};

	enum class CursorShape
	{
		ARROW         = 0x00036001,
		IBEAM         = 0x00036002,
		CROSSHAIR     = 0x00036003,
		POINTING_HAND = 0x00036004,
		RESIZE_EW     = 0x00036005,
		RESIZE_NS     = 0x00036006,
		RESIZE_NWSE   = 0x00036007,
		RESIZE_NESW   = 0x00036008,
		RESIZE_ALL    = 0x00036009,
		NOT_ALLOWED   = 0x0003600A,
	};

	enum class ClientApi
	{
		OpenGL,
		OpenGLES,
		Vulkan,
		DirectX11,
		DirectX12,
		Metal,
	};

	struct CrosshairSpecification
	{
		// If not provided, the default cursor for that shape will be used.

		const char* CursorArrowTexturePath            = nullptr;
		const char* CursorIBeamTexturePath            = nullptr;
		const char* CursorCrosshairTexturePath        = nullptr;
		const char* CursorPointingHandTexturePath     = nullptr;
		const char* CursorResizeEWTexturePath         = nullptr;
		const char* CursorResizeNSTexturePath         = nullptr;
		const char* CursorResizeNWSEResizeTexturePath = nullptr;
		const char* CursorResizeNESWTexturePath       = nullptr;
		const char* CursorResizeAllTexturePath        = nullptr;
		const char* CursorNotAllowedTexturePath       = nullptr;
	};

	struct DeviceSpecification
	{
#ifdef WINDOWING_OPENGL_CONTEXT
		// Enable debug OpenGL context, which may have additional error and performance issue reporting functionality
		bool DebugProfile = false;

		// Specify the client API major version that the created context must be compatible with.
		int GLMajorVersion = 4;

		// Specify the client API minor version that the created context must be compatible with.
		int GLMinorVersion = 5;

		// Defines the amount of samples to use (Required for multi-sampling)
		int Samples = 4;
#endif
		// The API to use for rendering, very important to be set correctly!
		ClientApi Api = ClientApi::OpenGL;

		// Path to the crosshair textures for different shapes (not required)
		CrosshairSpecification CrosshairSpec;
	};

	class Device
	{
	public:
		Device(const DeviceSpecification& spec);
		~Device();

		// in pixels {width, height}
		std::pair<int, int> GetPrimaryMonitorSize() const;

		GLFWcursor* GetCursorInstance(CursorShape shape) const;

		bool IsVSyncEnabled() const;

		// You must call this method after creating and defining a window as the current context
		void SetVSync(bool enabled);

		// Enable the inputs and events managements with created windows
		// Call this every frame
		void PollEvents() const;

		// Returns the elapsed time (in seconds) since the device startup
		float GetElapsedTime() const;

	public:
		static Eventing::Event<int, std::string> ErrorEvent;

	private:
		bool m_VSync = true;

		std::unordered_map<CursorShape, GLFWcursor*> m_Cursors;
	};

} // namespace SW::Windowing
