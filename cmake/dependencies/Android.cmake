macro(blue_find_dependencies_android)

    set(OpenGL_GL_PREFERENCE GLVND)

    find_package(glm REQUIRED)
    find_package(SDL2 REQUIRED VERSION EXACT 2.0.9)
    find_package(spdlog REQUIRED)
    find_package(Assimp REQUIRED)

    target_link_libraries(
            Dependencies INTERFACE
            SDL2::SDL2-static
            glm
            glad
            spdlog::spdlog
            ${OPENGL_LIBRARIES}
            "libassimpd.so"
            stb
            imgui
            "dl"
            "z"
            "EGL"
            # v2 meaning second version of the library - not that it will not support OpenGL ES 3.0
            "GLESv2"
    )

    target_compile_definitions(Dependencies INTERFACE IMGUI_IMPL_OPENGL_ES2)

endmacro()
