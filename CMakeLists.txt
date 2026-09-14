cmake_minimum_required(VERSION 3.10)
project(dx11)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

file (GLOB_RECURSE SOURCES
     "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(dx11 ${SOURCES})
target_link_libraries(dx11 PRIVATE d3d11 dxgi)

target_link_options(dx11 PRIVATE /ENTRY:WinMainCRTStartup)
