cmake_minimum_required(VERSION 3.14.0)
project(MetalRenderer)
set (CMAKE_CXX_STANDARD 17)
add_executable(MetalRenderer MACOSX_BUNDLE "")

find_library(cocoa_lib Cocoa)
find_library(metal_lib Metal)
find_library(metal_kit_lib MetalKit)

set(assimp_includes "" CACHE FILEPATH "Assimp include directory")
set(assimp_debug_lib "" CACHE FILEPATH "Assimp static debug library")
set(IrrXML_debug_lib "" CACHE FILEPATH "IrrXML static debug library")
set(LibZ_debug_lib "" CACHE FILEPATH "LibZ static debug library")
set(assimp_release_lib "" CACHE FILEPATH "Assimp static release library")
set(IrrXML_release_lib "" CACHE FILEPATH "IrrXML static release library")
set(LibZ_release_lib "" CACHE FILEPATH "LibZ static release library")

target_link_libraries(MetalRenderer
    general ${cocoa_lib}
    general ${metal_lib}
    general ${metal_kit_lib}
    debug ${assimp_debug_lib}
    debug ${IrrXML_debug_lib}
    debug ${LibZ_debug_lib}
    optimized ${assimp_release_lib}
    optimized ${IrrXML_release_lib}
    optimized ${LibZ_release_lib}
)

target_include_directories(MetalRenderer SYSTEM
    PRIVATE
        ${assimp_includes}
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
