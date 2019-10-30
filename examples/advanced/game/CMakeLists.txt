cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(GameSample)

set(SOURCES
        src/Main.cpp
        include/Game.hpp
        src/Game.cpp
        include/Callbacks.hpp
        src/Callbacks.cpp
        include/jobs/MapIntersectionJob.h
        src/jobs/MapIntersectionJob.cpp
        include/states/BaseState.hpp
        include/states/Playing.hpp
        src/states/Playing.cpp
        include/states/MainMenu.hpp
        src/states/MainMenu.cpp
        )

if(${BLUE_PLATFORM} STREQUAL "Android")
    add_library(GameSample SHARED ${SOURCES})
else()
    add_executable(GameSample ${SOURCES})
endif()

target_link_libraries(GameSample SDL2::SDL2main blue GameCommons)
target_include_directories(GameSample PRIVATE include)

add_custom_command(TARGET GameSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:GameSample>/resources
        )

install(TARGETS GameSample DESTINATION bin)

if(BLUE_PLATFORM_ANDROID)
    blue_android_deploy_asset(GameSample resources ${CMAKE_CURRENT_LIST_DIR}/resources)
endif()

if(BLUE_PLATFORM_WINDOWS)
    blue_windows_deploy_dynamic_dependencies(GameSample)
endif()
