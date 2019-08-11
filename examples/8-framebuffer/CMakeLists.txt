cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(FramebufferSample)

set(SOURCES src/Main.cpp)

if(${BLUE_PLATFORM} STREQUAL "Android")
    add_library(FramebufferSample SHARED ${SOURCES})
else()
    add_executable(FramebufferSample ${SOURCES})
endif()

target_link_libraries(FramebufferSample blue SDL2::SDL2main)
target_include_directories(FramebufferSample PRIVATE include)

add_custom_command(TARGET FramebufferSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:FramebufferSample>/resources
        )

if(BLUE_PLATFORM STREQUAL "Windows")
# TODO: Copy assimp dll
endif()

install(TARGETS FramebufferSample DESTINATION bin)

if(BLUE_PLATFORM_ANDROID)
    blue_android_deploy_asset(FramebufferSample resources ${CMAKE_CURRENT_LIST_DIR}/resources)
endif()

if(BLUE_PLATFORM_WINDOWS)
    blue_windows_deploy_dynamic_dependencies(FramebufferSample)
endif()
