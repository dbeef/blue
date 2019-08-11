cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(CameraSample)

set(SOURCES src/Main.cpp)

if(${BLUE_PLATFORM} STREQUAL "Android")
    add_library(CameraSample SHARED ${SOURCES})
else()
    add_executable(CameraSample ${SOURCES})
endif()

target_link_libraries(CameraSample blue SDL2::SDL2main)
target_include_directories(CameraSample PRIVATE include)

add_custom_command(TARGET CameraSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:CameraSample>/resources
        )

if(BLUE_PLATFORM STREQUAL "Windows")
# TODO: Copy assimp dll
endif()

install(TARGETS CameraSample DESTINATION bin)

if(BLUE_PLATFORM_ANDROID)
    blue_android_deploy_asset(CameraSample resources ${CMAKE_CURRENT_LIST_DIR}/resources)
endif()
