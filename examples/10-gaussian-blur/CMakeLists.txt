cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(GaussianBlurSample)

set(SOURCES src/Main.cpp)

if(${BLUE_PLATFORM} STREQUAL "Android")
    add_library(GaussianBlurSample SHARED ${SOURCES})
else()
    add_executable(GaussianBlurSample ${SOURCES})
endif()

target_link_libraries(GaussianBlurSample blue SDL2::SDL2main)
target_include_directories(GaussianBlurSample PRIVATE include)

add_custom_command(TARGET GaussianBlurSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:GaussianBlurSample>/resources
        )

install(TARGETS GaussianBlurSample DESTINATION bin)

if(BLUE_PLATFORM_ANDROID)
    blue_android_deploy_asset(GaussianBlurSample resources ${CMAKE_CURRENT_LIST_DIR}/resources)
endif()
