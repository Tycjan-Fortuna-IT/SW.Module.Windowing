cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(SW.Module.Windowing VERSION 0.2.1 LANGUAGES CXX)

include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE HEADERS "src/Windowing/*.hpp")
file(GLOB_RECURSE SOURCES "src/Windowing/*.cpp")

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

option(WINDOWING_OPENGL_CONTEXT "Use OpenGL context" ON)
option(WINDOWING_EXPOSE_NATIVE_WIN32 "Expose native Win32 window handle" OFF)

# Link GLFW statically
# set(BUILD_SHARED_LIBS OFF)
# set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_subdirectory(vendor/GLFW)
target_link_libraries(${PROJECT_NAME} glfw)
target_include_directories(${PROJECT_NAME} PUBLIC vendor/GLFW/include)

target_include_directories(${PROJECT_NAME} PUBLIC vendor/stb_image)

# target_include_directories(${PROJECT_NAME} PUBLIC ../SW.Module.Logger/src)
# target_link_libraries(${PROJECT_NAME} SW.Module.Logger)
target_include_directories(${PROJECT_NAME} PUBLIC ../SW.Module.Eventing/src)
target_link_libraries(${PROJECT_NAME} SW.Module.Eventing)

if(WINDOWING_OPENGL_CONTEXT)
    target_compile_definitions(${PROJECT_NAME} PUBLIC WINDOWING_OPENGL_CONTEXT)
endif()

if(WINDOWING_EXPOSE_NATIVE_WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC WINDOWING_EXPOSE_NATIVE_WIN32)
endif()
