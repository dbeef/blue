cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(InstancingSample)

set(SOURCES src/Main.cpp)

if(${BLUE_PLATFORM} STREQUAL "Android")
    add_library(InstancingSample SHARED ${SOURCES})
else()
    add_executable(InstancingSample ${SOURCES})
endif()

target_link_libraries(InstancingSample blue SDL2::SDL2main)
target_include_directories(InstancingSample PRIVATE include)

add_custom_command(TARGET InstancingSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:InstancingSample>/resources
        )

install(TARGETS InstancingSample DESTINATION bin)

if(BLUE_PLATFORM_ANDROID)
    blue_android_deploy_asset(InstancingSample resources ${CMAKE_CURRENT_LIST_DIR}/resources)
endif()

if(BLUE_PLATFORM_WINDOWS)
    blue_windows_deploy_dynamic_dependencies(InstancingSample)
endif()
