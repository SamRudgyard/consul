cmake_minimum_required(VERSION 3.10)
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
  GIT_TAG c
)
FetchContent_MakeAvailable(glad)

# The glad repo doesn't always create a CMake target usable by consumers; create one:
add_library(glad_lib STATIC ${glad_SOURCE_DIR}/src/glad.c)
target_include_directories(glad_lib PUBLIC ${glad_SOURCE_DIR}/include)
if(NOT WIN32)
  target_link_libraries(glad_lib PUBLIC ${CMAKE_DL_LIBS})
endif()

# ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG docking
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

add_library(imgui_lib STATIC ${IMGUI_SRC})
target_include_directories(imgui_lib PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
# imgui backends require linking to glfw; use the glfw target from FetchContent
target_link_libraries(imgui_lib PUBLIC glfw)

# --- end FetchContent block ---


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

# Set bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Set source directory
set(SRC_DIR src)

# Recursively find all source files
file(GLOB_RECURSE CPP_SOURCES "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE C_SOURCES "${SRC_DIR}/*.c")

# Recursively find all header files
file(GLOB_RECURSE HPP_HEADERS "${SRC_DIR}/*.hpp")

# Find all header files in include directorys
file(GLOB_RECURSE INC_HEADERS "${INC_DIR}/*.h")

# Add all source files to the project
include_directories(${SRC_DIR})

# Add all header files to the project
include_directories(${INC_DIR})

add_executable(${PROJECT_NAME} ${CPP_SOURCES} ${C_SOURCES} ${HPP_HEADERS} ${H_HEADERS} ${INC_HEADERS})

# Link the library targets created above:
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    glfw        # glfw target from FetchContent
    glad_lib    # our local static target that compiles glad.c
    imgui_lib   # imgui static lib built above
    glm::glm    # header-only glm target (if available)
    nlohmann_json::nlohmann_json
)

# Ensure includes for stb header-only are available (stb uses headers directly)
target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})
