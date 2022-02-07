cmake_minimum_required(VERSION 3.14)

project(skity_wrapper)

# skity options
set(BUILD_EXAMPLE OFF)
set(BUILD_TEST OFF)
set(VULKAN_BACKEND ON)
set(ENABLE_LOG OFF)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(third_party/freetype)

set(FREETYPE_FOUND True)
set(FREETYPE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/freetype/include)

add_subdirectory(external)
# volk
include_directories(third_party/volk)

# Fixme for share Skity example code
add_definitions(-DSKITY_ANDROID=1)
add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR=1)

add_library(skity_android SHARED
        external/example/example.cc
        external/example/frame_example.cc
        external/example/perf.cc
        src/cpp/renderer.cc
        src/cpp/renderer.hpp
        src/cpp/vk_renderer.cc
        src/cpp/vk_renderer.hpp
        src/cpp/static_renderer.cc
        src/cpp/static_renderer.hpp
        src/cpp/svg_renderer.cc
        src/cpp/svg_renderer.hpp
        src/cpp/frame_renderer.cc
        src/cpp/frame_renderer.hpp
        src/cpp/skity_wrapper.cc
        third_party/volk/volk.c
        )

target_include_directories(skity_android PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/external/example
        external/include
        external/module/svg/include
        external/third_party/glm
        )

target_link_libraries(skity_android
        skity::skity
        skity::svg
        android
        jnigraphics
        EGL
        GLESv3
        log
        m
        )