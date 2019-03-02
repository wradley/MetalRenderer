cmake_minimum_required(VERSION 3.14.0)
project(MetalRenderer)

add_executable(MetalRenderer MACOSX_BUNDLE "")

find_library(cocoa_lib Cocoa)
find_library(metal_lib Metal)
find_library(metal_kit_lib MetalKit)
find_library(model_io_lib ModelIO)

target_link_libraries(MetalRenderer
    ${cocoa_lib}
    ${metal_lib}
    ${metal_kit_lib}
    ${model_io_lib}
)

set_target_properties(MetalRenderer
    PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/plist.in
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
