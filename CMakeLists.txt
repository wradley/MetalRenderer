cmake_minimum_required(VERSION 3.14.0)
project(MetalRenderer)
set (CMAKE_CXX_STANDARD 17)
add_executable(MetalRenderer MACOSX_BUNDLE "")

find_library(cocoa_lib Cocoa)
find_library(metal_lib Metal)
find_library(metal_kit_lib MetalKit)

set(fbx_sdk_includes "" CACHE FILEPATH "FBX SDK include directory")
set(fbx_sdk_debug_lib "" CACHE FILEPATH "FBX SDK static debug library")
set(fbx_sdk_release_lib "" CACHE FILEPATH "FBX SDK static release library")

target_link_libraries(MetalRenderer
    general ${cocoa_lib}
    general ${metal_lib}
    general ${metal_kit_lib}
    debug ${fbx_sdk_debug_lib}
    optimized ${fbx_sdk_release_lib}
)

target_include_directories(MetalRenderer SYSTEM
    PRIVATE
        ${fbx_sdk_includes}
)

set_target_properties(MetalRenderer
    PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/plist.in
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
)

target_sources(MetalRenderer PRIVATE Resources/Main.storyboard)

set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Main.storyboard
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders.metal
    PROPERTIES
    MACOSX_PACKAGE_LOCATION
    Resources
)

add_subdirectory(Source)

file(GLOB_RECURSE all_source Source/*)
file(GLOB_RECURSE all_resources Resources/*)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${all_source})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${all_resources})
