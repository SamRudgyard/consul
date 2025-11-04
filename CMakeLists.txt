cmake_minimum_required(VERSION 3.10)

project(Consul)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

option(GLFW_BUILD_DOCS OFF)
add_subdirectory(submodules/glfw)

option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_INSTALL OFF)
option(ASSIMP_WARNINGS_AS_ERRORS OFF)

add_subdirectory(submodules/assimp)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

include_directories(submodules/assimp/include/
                    submodules/assimp/build/include/
                    submodules/glad/include/
                    submodules/glfw/include/
                    submodules/glm/
                    submodules/stb/
                    submodules/imgui/
                    submodules/imgui/backends/)

# Set bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Set include directory
set(INC_DIR include)

# Set source directory
set(SRC_DIR src)

# Recursively find all source files
file(GLOB_RECURSE CPP_SOURCES "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE C_SOURCES "${SRC_DIR}/*.c")

# Recursively find all header files
file(GLOB_RECURSE HPP_HEADERS "${SRC_DIR}/*.hpp")

# Find all header files in include directorys
file(GLOB_RECURSE INC_HEADERS "${INC_DIR}/*.h")

# Find required dependency files
file(GLOB DEP_SOURCES
    submodules/glad/src/glad.c
    submodules/imgui/imgui.cpp
    submodules/imgui/imgui_demo.cpp
    submodules/imgui/imgui_draw.cpp
    submodules/imgui/imgui_tables.cpp
    submodules/imgui/imgui_widgets.cpp
    submodules/imgui/backends/imgui_impl_opengl3.cpp
    submodules/imgui/backends/imgui_impl_glfw.cpp
)

# Add all source files to the project
include_directories(${SRC_DIR})

# Add all header files to the project
include_directories(${INC_DIR})

add_executable(${PROJECT_NAME} ${CPP_SOURCES} ${C_SOURCES} ${DEP_SOURCES} ${HPP_HEADERS} ${H_HEADERS} ${INC_HEADERS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} ${GLAD_LIBRARIES} assimp nlohmann_json::nlohmann_json)
