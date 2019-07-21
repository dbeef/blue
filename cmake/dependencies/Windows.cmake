macro(blue_find_dependencies_windows)

    set(OpenGL_GL_PREFERENCE GLVND)

    find_package(OpenGL REQUIRED)
    find_package(Assimp REQUIRED)
    find_package(glm REQUIRED)
    find_package(SDL2 REQUIRED VERSION EXACT 2.0.9)
    find_package(spdlog REQUIRED)

    find_library(ASSIMP_LIB NAMES "assimp-vc140-mt.lib")
    message(STATUS "[BLUE] Assimp library path: ${ASSIMP_LIB}")

    target_link_libraries(Dependencies INTERFACE
            SDL2::SDL2main
            SDL2::SDL2-static
            glm
            glad
            spdlog::spdlog
            ${OPENGL_LIBRARIES}
            ${ASSIMP_LIB}
            stb
            imgui
            )

    # Globally disabling MSDOS relicts:
    add_definitions(-DNOMINMAX)
    add_definitions(-DWIN32_LEAN_AND_MEAN)

endmacro()
