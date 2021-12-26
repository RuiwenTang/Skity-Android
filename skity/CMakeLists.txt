cmake_minimum_required(VERSION 3.14)

project(skity_wrapper)

# skity options
set(BUILD_EXAMPLE OFF)
set(BUILD_TEST OFF)
set(VULKAN_BACKEND ON)
set(ENABLE_LOG OFF)

add_subdirectory(third_party/freetype)

set(FREETYPE_FOUND True)
set(FREETYPE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/freetype/include)

add_subdirectory(external)

add_library(skity_android SHARED
        src/cpp/skity_wrapper.cc)

target_include_directories(skity_android PRIVATE
        external/include
        external/module/svg/include
        external/third_party/glm
        )

target_link_libraries(skity_android
        skity::skity
        skity::svg
        android
        EGL
        GLESv3
        log
        m
        )