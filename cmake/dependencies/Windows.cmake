function(blue_find_dependencies_windows)

    set(OpenGL_GL_PREFERENCE GLVND)

    find_package(OpenGL REQUIRED)
    find_package(Assimp REQUIRED)
    find_package(glm REQUIRED)
    find_package(SDL2 REQUIRED)
    find_package(spdlog REQUIRED)

    find_library(ASSIMP_LIB NAMES "assimp-vc140-mt.lib")
    message(STATUS "[BLUE] Assimp library path: ${ASSIMP_LIB}")

    target_link_libraries(Dependencies INTERFACE
            SDL2::SDL2-static
            glm
            glad
            spdlog::spdlog
            ${OPENGL_LIBRARIES}
            # TODO: Use BLUE_ASSIMP_PATH instead.
            ${ASSIMP_LIB}
            stb
            imgui
            )

    # Globally disabling MSDOS relicts:
    add_definitions(-DNOMINMAX)
    add_definitions(-DWIN32_LEAN_AND_MEAN)

    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        get_property(BLUE_ASSIMP_PATH TARGET assimp::assimp PROPERTY IMPORTED_LOCATION_DEBUG)
    else ()
        get_property(BLUE_ASSIMP_PATH TARGET assimp::assimp PROPERTY IMPORTED_LOCATION_RELEASE)
    endif ()

    set(BLUE_ASSIMP_PATH ${ASSIMP_PATH} PARENT_SCOPE)
    set(BLUE_ASSIMP_PATH ${ASSIMP_PATH})

    message(STATUS "[BLUE] Path to assimp: ${BLUE_ASSIMP_PATH}")


endfunction()
