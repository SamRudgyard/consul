cmake_minimum_required(VERSION 3.24)

project(ConsulExample2DCamera
    DESCRIPTION "Example of using Consul's 2D camera"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>)

set(BUILD_TESTS OFF CACHE BOOL "Disable Consul tests for examples" FORCE)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../.. ${CMAKE_CURRENT_BINARY_DIR}/consul)

add_executable(2d_camera
    ${CMAKE_CURRENT_LIST_DIR}/2d_camera.cpp
)

target_link_libraries(2d_camera
    PRIVATE
        consul::consul
)
