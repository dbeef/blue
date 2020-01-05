function(blue_find_dependencies_linux)

    set(OpenGL_GL_PREFERENCE GLVND)

    find_package(OpenGL REQUIRED)
    find_package(Assimp REQUIRED)
    find_package(glm REQUIRED)
    find_package(SDL2 REQUIRED)
    find_package(spdlog REQUIRED)
    find_package(Assimp REQUIRED)

    target_link_libraries(Dependencies INTERFACE
            SDL2::SDL2-static
            dl
            glm
            glad
            assimp
            spdlog::spdlog
            ${OPENGL_LIBRARIES}
            stb
            imgui
            )

endfunction()
