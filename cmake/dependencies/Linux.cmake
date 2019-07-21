macro(blue_find_dependencies_linux)

    set(OpenGL_GL_PREFERENCE GLVND)

    find_package(OpenGL REQUIRED)
    find_package(Assimp REQUIRED)
    find_package(glm REQUIRED)
    find_package(SDL2 REQUIRED VERSION EXACT 2.0.9)
    find_package(spdlog REQUIRED)
    find_package(Assimp REQUIRED)

    target_link_libraries(Dependencies INTERFACE
            SDL2::SDL2main
            SDL2::SDL2-static
            glm
            glad
            assimp
            spdlog::spdlog
            ${OPENGL_LIBRARIES}
            stb
            imgui
            )

endmacro()
