function(blue_find_dependencies_android)

    set(OpenGL_GL_PREFERENCE GLVND)

    find_package(glm REQUIRED)
    find_package(SDL2 REQUIRED)
    find_package(spdlog REQUIRED)
    find_package(Assimp REQUIRED CONFIG)

    find_library(ASSIMP_LIB NAMES "libassimpd.so")
    message(STATUS "[BLUE] Assimp library path: ${ASSIMP_LIB}")

    target_link_libraries(Dependencies INTERFACE
            SDL2::SDL2-static
            glm
            glad
            spdlog::spdlog
            ${OPENGL_LIBRARIES}
            ${ASSIMP_LIB}
            stb
            imgui
            "dl"
            "z"
            "EGL"
            # v2 meaning second version of the library - not that it will not support OpenGL ES 3.0
            "GLESv2"
            )

    target_compile_definitions(Dependencies INTERFACE IMGUI_IMPL_OPENGL_ES2)

endfunction()

# Android-specific part; create "lib" folder in the root of blue,
# and put dynamic dependencies (i.e assimp) into it, which will be deployed in the APK file.
# "lib" folder is temporary and can be deleted afterwards.
function(blue_android_postbuild)
    add_custom_command(TARGET blue PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_LIST_DIR}/lib"
            )

    add_custom_command(TARGET blue PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_LIST_DIR}/lib/armeabi-v7a"
            )

    add_custom_command(TARGET blue PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${ASSIMP_LIB} "${CMAKE_CURRENT_LIST_DIR}/lib/armeabi-v7a/"
            )
endfunction()

# Call this function on your resource, to copy it into resulting APK's assets directory.
# This will produce "assets" directory in the root of blue, that can be deleted afterwards.
function(blue_android_deploy_asset target_name asset_dir_name asset_path)

    add_custom_command(TARGET ${target_name} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${BLUE_ROOT_PATH}/assets"
            )

    add_custom_command(TARGET ${target_name} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${BLUE_ROOT_PATH}/assets/${asset_dir_name}"
            )

    add_custom_command(TARGET ${target_name} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${asset_path} "${BLUE_ROOT_PATH}/assets/${asset_dir_name}"
            )

endfunction()
