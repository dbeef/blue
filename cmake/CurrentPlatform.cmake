macro(blue_detect_platform)

    if (DEFINED BLUE_PLATFORM)
        # Nothing to do - BLUE_PLATFORM was already defined in the toolchain file.
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        set(BLUE_PLATFORM "Linux")
        set(BLUE_PLATFORM_LINUX TRUE)
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        set(BLUE_PLATFORM "Windows")
        set(BLUE_PLATFORM_WINDOWS TRUE)
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
        set(BLUE_PLATFORM "Android")
        set(BLUE_PLATFORM_ANDROID TRUE)
    else ()
        message(FATAL_ERROR "Your platform (${BLUE_PLATFORM}) is not supported")
    endif ()

    if(NOT DEFINED BLUE_PLATFORM)
        message(FATAL_ERROR "No platform detected.")
    endif()

    message(STATUS "[BLUE] Target platform is: ${BLUE_PLATFORM}")

endmacro()
