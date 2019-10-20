cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(KernelBlurSample)

set(SOURCES src/Main.cpp)

if(${BLUE_PLATFORM} STREQUAL "Android")
    add_library(KernelBlurSample SHARED ${SOURCES})
else()
    add_executable(KernelBlurSample ${SOURCES})
endif()

target_link_libraries(KernelBlurSample blue SDL2::SDL2main)
target_include_directories(KernelBlurSample PRIVATE include)

add_custom_command(TARGET KernelBlurSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:KernelBlurSample>/resources
        )

install(TARGETS KernelBlurSample DESTINATION bin)

if(BLUE_PLATFORM_ANDROID)
    blue_android_deploy_asset(KernelBlurSample resources ${CMAKE_CURRENT_LIST_DIR}/resources)
endif()
