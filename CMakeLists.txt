cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(minecraft-clone)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(Config.h.in ${CMAKE_BINARY_DIR}/Config.h)

# profiler
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

# download the submodules
find_package(Git QUIET)

if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)

    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT)

        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

# check the submodules
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/glfw/CMakeLists.txt")
    message(FATAL_ERROR "The glfw submodule was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/glew/CMakeLists.txt")
    message(FATAL_ERROR "The glew submodule was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/glm/CMakeLists.txt")
    message(FATAL_ERROR "The glm submodule was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/fastnoise2/CMakeLists.txt")
    message(FATAL_ERROR "The glm submodule was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

add_executable(${PROJECT_NAME}
    src/gui/GuiManager.cpp
    src/utils/Timer.cpp
    src/camera/Frustum.cpp
    src/camera/Camera.cpp
    src/world/Chunk.cpp
    src/world/ChunkManager.cpp
    src/Game.cpp
    src/graphics/IndexBuffer.cpp
    src/main.cpp
    src/utils/Matrix3D.hpp
    src/utils/Noise.cpp
    src/graphics/Renderer.cpp
    src/graphics/Shader.cpp
    src/graphics/Texture.cpp
    src/utils/ThreadPool.hpp
    src/graphics/VertexArray.cpp
    src/graphics/VertexBuffer.cpp
    src/graphics/VertexBufferLayout.hpp
    src/graphics/Window.cpp
    src/utils/Physics.h
    src/utils/Physics.cpp
    src/camera/Constants.h
    src/utils/Random.hpp
    src/sky/Sun.cpp
    src/sky/Moon.cpp
    src/utils/Logger.cpp
    src/entities/Player.cpp
    src/utils/InputHandler.cpp
    src/utils/Subject.hpp
    src/utils/RayCast.hpp
    src/graphics/ChunkRenderer.cpp
    src/graphics/TextureAtlas.cpp
    src/graphics/GuiRenderer.cpp
    src/graphics/QuadRenderer.cpp
    src/graphics/SkyRenderer.cpp
    src/sky/SkyEntities.cpp
    src/gui/Cursor.cpp
    src/world/Light.cpp
    src/sky/Stars.cpp 
    src/sky/Clouds.cpp
    src/utils/ImGui.cpp)

if (NOT TARGET glfw)
	add_subdirectory(extern/glfw)
endif()
add_subdirectory(extern/glew)
add_subdirectory(extern/glm)
add_subdirectory(extern/fastnoise2)
add_subdirectory(extern/spdlog)
add_subdirectory(extern/imgui)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_BINARY_DIR}
    PUBLIC extern/glfw/include
    PUBLIC extern/glew/include
    PUBLIC extern/glm
    PUBLIC extern/stb_image
    PUBLIC extern/fastnoise2/include
    PUBLIC extern/spdlog/include
    PUBLIC extern/imgui
)

target_link_libraries(${PROJECT_NAME} glfw libglew_static glm FastNoise spdlog imgui)
