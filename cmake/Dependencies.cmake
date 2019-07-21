macro(blue_find_dependencies)

    include(cmake/dependencies/Android.cmake)
    include(cmake/dependencies/Linux.cmake)
    include(cmake/dependencies/Windows.cmake)

    add_library(Dependencies INTERFACE)

    if (BLUE_PLATFORM STREQUAL "Android")
        blue_find_dependencies_android()
    elseif (BLUE_PLATFORM STREQUAL "Linux")
        blue_find_dependencies_linux()
    elseif (BLUE_PLATFORM STREQUAL "Windows")
        blue_find_dependencies_windows()
    endif ()

    target_compile_definitions(Dependencies INTERFACE BLUE_PLATFORM="${BLUE_PLATFORM}")

    if (BLUE_PLATFORM STREQUAL "Android")
        target_compile_definitions(Dependencies INTERFACE BLUE_ANDROID)
    elseif (BLUE_PLATFORM STREQUAL "Linux")
        target_compile_definitions(Dependencies INTERFACE BLUE_LINUX)
    elseif (BLUE_PLATFORM STREQUAL "Windows")
        target_compile_definitions(Dependencies INTERFACE BLUE_WINDOWS)
    endif ()

    # Globally unlock some GLM features
    add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

endmacro()
