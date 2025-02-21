# SW Windowing Module

The SW Windowing module provides a flexible and easy-to-use windowing system for creating and managing windows in a graphical application. It abstracts the platform-specific window creation and management code, allowing developers to focus on building the application's core functionality without worrying about the underlying platform details. The module supports creating multiple windows, handling input events, and managing window properties such as size, position, and title. It also provides a simple API for handling window events, such as resizing, moving, and closing windows, making it easy to build interactive applications with graphical user interfaces.

## Tested Platforms

Used C++ dialect version: `20`

- [x] Windows (`MSVC 2022`)
- [x] Linux (`clang 18.x`)

## Dependencies (SW modules)

| Module | Link                                        | Version |
| ------ | ------------------------------------------- | ------- |
| Logger | https://github.com/Tycjan-Fortuna-IT/Logger | latest  |
| Eventing | https://github.com/Tycjan-Fortuna-IT/Eventing | latest  |

Placed in the same directory as the Eventing module.
It is recommended to keep all modules in the same directory e.g. `Engine/modules/`.

<!-- ## Installation (Premake5 - FancyBuildSystem)

Add inside your main dependencies.lua following line:

```lua
FBS.ImportModule("Engine/modules/Eventing");
```

Replacing with correct path to the module. -->

## Installation (CMake)

Add the following to your CMakeLists.txt file:

```cmake
add_subdirectory(PATH_TO_THIS_MODULE)
target_link_libraries(${PROJECT_NAME} Windowing)
target_include_directories(${PROJECT_NAME} PUBLIC PATH_TO_THIS_MODULE/src)
```

Example:

```cmake
add_subdirectory(Modules/Windowing)
target_link_libraries(${PROJECT_NAME} Windowing)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Engine/Modules/Windowing/src)
```

With possible options:

- `WINDOWING_EXPOSE_NATIVE_WIN32` - Exposes native Win32 window handle for advanced usage.
- `WINDOWING_OPENGL_CONTEXT` - Enables OpenGL context creation. Disable for non-OpenGL projects.


### Example Usage

```cpp
#include <Logger/LogSystem.hpp>
#include <Windowing/Device.hpp>
#include <Windowing/InputManager.hpp>
#include <Windowing/Timestep.hpp>
#include <Windowing/Window.hpp>

using namespace SW;

int main()
{
	Logger::LogSystem::Initialize({
	    .LogFileName      = "logs/DX11.log",
	    .SystemLoggerName = "DX11",
	    .AppLoggerName    = "LAUNCHER",
	});

	Windowing::Device device(Windowing::DeviceSpecification{
	    .Api = Windowing::ClientApi::DirectX11,
	});

	Windowing::Window window(&device, Windowing::WindowSpecification{
	                                      .Title       = "Simple window",
	                                      .Width       = 800,
	                                      .Height      = 600,
	                                      .CursorMode  = Windowing::CursorMode::NORMAL,
	                                      .CursorShape = Windowing::CursorShape::ARROW,
	                                  });

	Windowing::InputManager inputManager(&window);

	window.MakeContextCurrent(); // if more than one window, remember to properly switch context
	device.SetVSync(true);

	float lastFrameTime = 0.0f;
	while (!window.ShouldClose())
	{
		const float frameStartTime   = device.GetElapsedTime();
		const Windowing::Timestep dt = frameStartTime - lastFrameTime;
		lastFrameTime                = frameStartTime;

		device.PollEvents();
		window.SwapBuffers();

		inputManager.ClearReleasedKeys();
	}

	Logger::LogSystem::Shutdown();

	return 0;
}
```
