cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(TriangleSample)

set(SOURCES src/Main.cpp)

if(${BLUE_PLATFORM} STREQUAL "Android")
    add_library(TriangleSample SHARED ${SOURCES})
else()
    add_executable(TriangleSample ${SOURCES})
endif()

target_link_libraries(TriangleSample blue SDL2::SDL2main)
target_include_directories(TriangleSample PRIVATE include)

add_custom_command(TARGET TriangleSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:TriangleSample>/resources
        )

install(TARGETS TriangleSample DESTINATION bin)

if(BLUE_PLATFORM_ANDROID)
    blue_android_deploy_asset(TriangleSample resources ${CMAKE_CURRENT_LIST_DIR}/resources)
endif()
