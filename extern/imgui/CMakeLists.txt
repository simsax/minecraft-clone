cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(imgui)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

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
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/glfw/CMakeLists.txt")
    message(FATAL_ERROR "The glfw submodule was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

if (NOT TARGET glfw)
	add_subdirectory(glfw)
endif()

file(GLOB IMGUI_SOURCES "./*.cpp")
add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(${PROJECT_NAME}
	PUBLIC ${CMAKE_BINARY_DIR}
	PUBLIC glfw/include
)
