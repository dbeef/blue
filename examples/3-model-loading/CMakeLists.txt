cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(ModelLoadingSample)

set(SOURCES src/Main.cpp)

if(${BLUE_PLATFORM} STREQUAL "Android")
    add_library(ModelLoadingSample SHARED ${SOURCES})
else()
    add_executable(ModelLoadingSample ${SOURCES})
endif()

target_link_libraries(ModelLoadingSample blue SDL2::SDL2main)
target_include_directories(ModelLoadingSample PRIVATE include)

add_custom_command(TARGET ModelLoadingSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:ModelLoadingSample>/resources
        )

if(BLUE_PLATFORM STREQUAL "Windows")
# TODO: Copy assimp dll
endif()

install(TARGETS ModelLoadingSample DESTINATION bin)

if(BLUE_PLATFORM_ANDROID)
    blue_android_deploy_asset(ModelLoadingSample resources ${CMAKE_CURRENT_LIST_DIR}/resources)
endif()
