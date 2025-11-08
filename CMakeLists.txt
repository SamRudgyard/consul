cmake_minimum_required(VERSION 3.24)
project(Consul)

# Prefer the NEW behavior for FetchContent extraction timestamps
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# json already present in your file; keep that
FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
FetchContent_MakeAvailable(json)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLM (header only)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.2
)
FetchContent_MakeAvailable(glm)

# STB (header-only)
FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# GLAD - fetch and create a small target from the single-source generator output
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG v2.0.8
  GIT_PROGRESS   TRUE
  SOURCE_SUBDIR  cmake
)
FetchContent_MakeAvailable(glad)
glad_add_library(glad STATIC REPRODUCIBLE LOADER API gl:core=4.6)

# ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.92.4
)
FetchContent_MakeAvailable(imgui)

# Build ImGui as a static lib from the sources we need
set(IMGUI_SRC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
# imgui backends require linking to glfw; use the glfw target from FetchContent
target_link_libraries(imgui PUBLIC glfw)

# --- end FetchContent block ---


if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    if(NOT WIN32)
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

# Set bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>)

# Set source directory
set(SRC_DIR src)

# Recursively find all source files
file(GLOB_RECURSE CPP_SOURCES "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE C_SOURCES "${SRC_DIR}/*.c")

# Recursively find all header files
file(GLOB_RECURSE HPP_HEADERS "${SRC_DIR}/*.hpp")
file(GLOB_RECURSE H_HEADERS "${SRC_DIR}/*.h")

# Add all source files to the project
include_directories(${SRC_DIR})

add_executable(${PROJECT_NAME} ${CPP_SOURCES} ${C_SOURCES} ${HPP_HEADERS} ${H_HEADERS})

# Link the library targets created above:
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    glfw
    glad
    imgui
    glm::glm
    nlohmann_json::nlohmann_json
)

# Ensure includes for stb header-only are available (stb uses headers directly)
target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})
