cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(imgui)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (NOT TARGET glfw)
	add_subdirectory(../glfw)
endif()

file(GLOB IMGUI_SOURCES "./*.cpp")
add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(${PROJECT_NAME}
	PUBLIC ${CMAKE_BINARY_DIR}
	PUBLIC ../glfw/include
)
