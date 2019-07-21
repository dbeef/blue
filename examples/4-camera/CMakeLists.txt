cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(CameraSample)

add_executable(CameraSample
        src/Main.cpp
        )

target_link_libraries(CameraSample blue -lpthread)
target_include_directories(CameraSample PRIVATE include)

add_custom_command(TARGET CameraSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:CameraSample>/resources
        )

if(BLUE_PLATFORM STREQUAL "Windows")
# TODO: Copy assimp dll
endif()

install(TARGETS CameraSample DESTINATION bin)
