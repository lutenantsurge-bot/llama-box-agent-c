cmake_minimum_required(VERSION 3.14)
project(llama-box-agent C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Options
option(AGENT_CPP_BUNDLED_LLAMA "Bundle llama.cpp (vs find installed)" ON)
option(BOX_PATCH_CI "box: patch CI" OFF)
option(BOX_PATCH_DEBUG "box: enable debug patches" OFF)

# Debug options
option(LLAMA_ALL_WARNINGS "llama: enable all compiler warnings" ON)
option(LLAMA_FATAL_WARNINGS "llama: enable -Werror flag" OFF)

# Build options
set(LLAMA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_SERVER OFF CACHE BOOL "" FORCE)

# 3rd party libs
set(LLAMA_CURL OFF CACHE BOOL "" FORCE)
set(LLAMA_LLGUIDANCE OFF CACHE BOOL "" FORCE)

# Paths
set(LLAMA_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp")
set(LLAMA_BOX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/llama-box/llama-box")

# llama.cpp
add_subdirectory(${LLAMA_SOURCE_DIR} llama-build)
set_target_properties(llama-common PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(llama PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(ggml PROPERTIES POSITION_INDEPENDENT_CODE ON)

# stable-diffusion.cpp
add_subdirectory(stable-diffusion.cpp sd-build)
set_target_properties(stable-diffusion PROPERTIES POSITION_INDEPENDENT_CODE ON)

# llama-box server (multimodal, tool calls)
add_subdirectory(${LLAMA_BOX_SOURCE_DIR} llama-box-build)

# agent.cpp library
add_library(agent STATIC src/agent.cpp)
target_include_directories(agent
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${LLAMA_SOURCE_DIR}/common>
        $<BUILD_INTERFACE:${LLAMA_SOURCE_DIR}/ggml/include>
        $<BUILD_INTERFACE:${LLAMA_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${LLAMA_SOURCE_DIR}/vendor>
)
target_link_libraries(agent PRIVATE llama-box common ggml llama)
target_compile_features(agent PUBLIC cxx_std_17)

# agent model library
add_library(model STATIC src/model.cpp)
target_include_directories(model
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${LLAMA_SOURCE_DIR}/common>
        $<BUILD_INTERFACE:${LLAMA_SOURCE_DIR}/ggml/include>
        $<BUILD_INTERFACE:${LLAMA_SOURCE_DIR}/include>
)
target_link_libraries(model PRIVATE llama-box common ggml llama)
target_compile_features(model PUBLIC cxx_std_17)

# Main executable
add_executable(llama-box-agent main.cpp)
target_link_libraries(llama-box-agent PRIVATE agent model common llama-box)
target_compile_features(llama-box-agent PRIVATE cxx_std_17)
