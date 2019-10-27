cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(GuiSample)

set(SOURCES src/Main.cpp)

if(${BLUE_PLATFORM} STREQUAL "Android")
    add_library(GuiSample SHARED ${SOURCES})
else()
    add_executable(GuiSample ${SOURCES})
endif()

target_link_libraries(GuiSample blue SDL2::SDL2main)
target_include_directories(GuiSample PRIVATE include)

add_custom_command(TARGET GuiSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:GuiSample>/resources
        )

install(TARGETS GuiSample DESTINATION bin)

if(BLUE_PLATFORM_ANDROID)
    blue_android_deploy_asset(GuiSample resources ${CMAKE_CURRENT_LIST_DIR}/resources)
endif()
