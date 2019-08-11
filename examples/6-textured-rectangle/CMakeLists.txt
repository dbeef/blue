cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(TexturedRectangleSample)

set(SOURCES src/Main.cpp)

if(${BLUE_PLATFORM} STREQUAL "Android")
    add_library(TexturedRectangleSample SHARED ${SOURCES})
else()
    add_executable(TexturedRectangleSample ${SOURCES})
endif()

target_link_libraries(TexturedRectangleSample blue SDL2::SDL2main)
target_include_directories(TexturedRectangleSample PRIVATE include)

add_custom_command(TARGET TexturedRectangleSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:TexturedRectangleSample>/resources
        )

install(TARGETS TexturedRectangleSample DESTINATION bin)

if(BLUE_PLATFORM_ANDROID)
    blue_android_deploy_asset(TexturedRectangleSample resources ${CMAKE_CURRENT_LIST_DIR}/resources)
endif()
