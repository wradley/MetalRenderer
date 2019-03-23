target_sources(MetalRenderer
    PRIVATE
        AppDelegate.h
        AppDelegate.m
        main.m
        Shaders.metal
        ShaderTypes.h
        ViewController.h
        ViewController.m
        ViewDelegate.h
        ViewDelegate.mm
)

set_source_files_properties(Shaders.metal PROPERTIES LANGUAGE METAL)

add_subdirectory(Math)
add_subdirectory(Mesh)
add_subdirectory(Renderer)
