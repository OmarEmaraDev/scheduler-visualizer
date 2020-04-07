cmake_minimum_required(VERSION 3.17)

project("Scheduler Visualizer")

add_library(imgui
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_widgets.cpp
)

find_package(glfw3 REQUIRED)

add_library(imguiGLFW
  imgui/examples/imgui_impl_glfw.cpp
)

target_include_directories(imguiGLFW
  PUBLIC imgui
)

target_link_libraries(imguiGLFW
  PUBLIC imgui
  PRIVATE glfw
)

include(FindOpenGL)
include(FindGLEW)

add_library(imguiOpenGL
  imgui/examples/imgui_impl_opengl3.cpp
)

target_include_directories(imguiOpenGL
  PUBLIC imgui
)

target_link_libraries(imguiOpenGL
  PUBLIC imgui
  PRIVATE OpenGL::GL
  PRIVATE GLEW::glew
)

add_executable(schedularVisualizer
  main.cpp
)

target_include_directories(schedularVisualizer
  PUBLIC imgui
  PUBLIC imgui/examples
)

target_link_libraries(schedularVisualizer
  PRIVATE imgui
  PRIVATE imguiGLFW
  PRIVATE imguiOpenGL
  PRIVATE GLEW::glew
)

target_compile_options(schedularVisualizer
  PRIVATE -Wall
  PRIVATE -Wextra
  PRIVATE -pedantic
  PRIVATE -Werror
)
