function(blue_find_dependencies_windows)

    set(OpenGL_GL_PREFERENCE GLVND)

    find_package(OpenGL REQUIRED)
    find_package(Assimp REQUIRED)
    find_package(glm REQUIRED)
    find_package(SDL2 REQUIRED)
    find_package(spdlog REQUIRED)

    find_library(BLUE_ASSIMP_LIB NAMES "assimp-vc140-mt.lib")
    get_target_property(ASSIMP_DLL assimp::assimp IMPORTED_LOCATION_RELEASE)
    set(BLUE_ASSIMP_DLL ${ASSIMP_DLL} PARENT_SCOPE)
    set(BLUE_ASSIMP_DLL ${ASSIMP_DLL})

    target_link_libraries(Dependencies INTERFACE
            SDL2::SDL2-static
            glm
            glad
            spdlog::spdlog
            ${OPENGL_LIBRARIES}
            ${BLUE_ASSIMP_LIB}
            stb
            imgui
            )

    # Globally disabling MSDOS relicts:
    add_definitions(-DNOMINMAX)
    add_definitions(-DWIN32_LEAN_AND_MEAN)

    message(STATUS "[BLUE] Assimp lib path: ${BLUE_ASSIMP_LIB}")
    message(STATUS "[BLUE] Assimp dll path: ${BLUE_ASSIMP_DLL}")

endfunction()

function(blue_windows_deploy_dynamic_dependencies target_name)
    add_custom_command(TARGET ${target_name} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${BLUE_ASSIMP_DLL} "$<TARGET_FILE_DIR:${target_name}>/../"
            )
endfunction()
