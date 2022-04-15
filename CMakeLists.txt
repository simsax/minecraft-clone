cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(minecraft-clone VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(MCConfig.h.in MCConfig.h)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})

add_subdirectory(minecraft-clone)

