cmake_minimum_required(VERSION 3.14)

project(skity_wrapper)

# skity options
set(BUILD_EXAMPLE OFF)
set(BUILD_TEST OFF)
set(VULKAN_BACKEND ON)
set(ENABLE_LOG ON)

add_subdirectory(external)

add_library(skity_wrapper SHARED
        src/cpp/skity_wrapper.cc)

target_include_directories(skity_wrapper PRIVATE
        external/include
        external/third_party/glm
        )

target_link_libraries(skity_wrapper
        skity::skity
        android
        EGL
        GLESv3
        log
        m
        )