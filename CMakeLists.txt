cmake_minimum_required(VERSION 3.26)
project(CPP_Game_Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS, -m32)
set(CMAKE_C_FLAGS, -m32)

set(CMAKE_VERBOSE_MAKEFILE ON)

# Set the path to the dependencies
set(DEPENDENCIES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/TD Game/Dependencies")

# Create an executable target called CPP_Game_Engine
file(GLOB_RECURSE SOURCES
        "${DEPENDENCIES_PATH}/glm/*.hpp"

        "${DEPENDENCIES_PATH}/imgui/*.cpp"
        "${DEPENDENCIES_PATH}/imgui/*.h"
        "${DEPENDENCIES_PATH}/imgui/*.hpp"

        "TD Game/src/*.cpp"
        "TD Game/src/*.h"
        "TD Game/src/*.hpp"

)
add_executable(CPP_Game_Engine ${SOURCES}
        "TD Game/src/Camera.cpp"
        "TD Game/src/Camera.h"
        "TD Game/src/Components/Transform.cpp"
        "TD Game/src/Components/Transform.h"
        "TD Game/src/Components/Component.cpp"
        "TD Game/src/Components/Component.h"
        "TD Game/src/Components/Mesh.cpp"
        "TD Game/src/Components/Mesh.h")

# Include the dependencies
target_include_directories(CPP_Game_Engine PUBLIC
        "${DEPENDENCIES_PATH}"
        "${DEPENDENCIES_PATH}/glm"
        "${DEPENDENCIES_PATH}/imgui"
)

# import GLFW as a dynamic library
add_library(glfw SHARED IMPORTED)
set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION "${DEPENDENCIES_PATH}/GLFW/lib-mingw/glfw3.dll"
        IMPORTED_IMPLIB "${DEPENDENCIES_PATH}/GLFW/lib-mingw/libglfw3dll.a"
        INTERFACE_INCLUDE_DIRECTORIES "${DEPENDENCIES_PATH}/GLFW/include"
)

add_library(glew STATIC IMPORTED)
set_target_properties(glew PROPERTIES
        IMPORTED_LOCATION "${DEPENDENCIES_PATH}/GLEW/lib/Release/Win32/glew32.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${DEPENDENCIES_PATH}/GLEW/include"
)

# Link against the dependencies
target_link_libraries(CPP_Game_Engine
        glfw
        glew
        opengl32
)