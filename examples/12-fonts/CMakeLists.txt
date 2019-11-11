cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(FontsSample)

set(SOURCES src/Main.cpp)

if(${BLUE_PLATFORM} STREQUAL "Android")
    add_library(FontsSample SHARED ${SOURCES})
else()
    add_executable(FontsSample ${SOURCES})
endif()

target_link_libraries(FontsSample blue SDL2::SDL2main)
target_include_directories(FontsSample PRIVATE include)

add_custom_command(TARGET FontsSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:FontsSample>/resources
        )

install(TARGETS FontsSample DESTINATION bin)

if(BLUE_PLATFORM_ANDROID)
    blue_android_deploy_asset(FontsSample resources ${CMAKE_CURRENT_LIST_DIR}/resources)
endif()
