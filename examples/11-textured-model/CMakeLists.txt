cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(TexturedModelLoadingSample)

set(SOURCES src/Main.cpp)

if(${BLUE_PLATFORM} STREQUAL "Android")
    add_library(TexturedModelLoadingSample SHARED ${SOURCES})
else()
    add_executable(TexturedModelLoadingSample ${SOURCES})
endif()

target_link_libraries(TexturedModelLoadingSample blue SDL2::SDL2main)
target_include_directories(TexturedModelLoadingSample PRIVATE include)

add_custom_command(TARGET TexturedModelLoadingSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:TexturedModelLoadingSample>/resources
        )

if(BLUE_PLATFORM STREQUAL "Windows")
# TODO: Copy assimp dll
endif()

install(TARGETS TexturedModelLoadingSample DESTINATION bin)

if(BLUE_PLATFORM_ANDROID)
    blue_android_deploy_asset(TexturedModelLoadingSample resources ${CMAKE_CURRENT_LIST_DIR}/resources)
endif()

if(BLUE_PLATFORM_WINDOWS)
    blue_windows_deploy_dynamic_dependencies(TexturedModelLoadingSample)
endif()
