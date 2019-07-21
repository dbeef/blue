add_library(
        imgui STATIC
        src/imgui.cpp
        src/imgui_demo.cpp
        src/imgui_draw.cpp
        src/imgui_impl_opengl3.cpp
        src/imgui_impl_sdl.cpp
        src/imgui_widgets.cpp
        )

target_include_directories(
        imgui
        INTERFACE
        include
        PRIVATE
        include/imgui
        )

target_compile_definitions(
        imgui
        PRIVATE
        IMGUI_IMPL_OPENGL_LOADER_GLAD
        INTERFACE
        IMGUI_IMPL_OPENGL_LOADER_GLAD
        )

target_link_libraries(
        imgui
        PRIVATE
        Dependencies
        )

set_target_properties(imgui PROPERTIES
        FOLDER vendor
)
