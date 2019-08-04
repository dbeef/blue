cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(InstancingSample)

add_executable(InstancingSample
        src/Main.cpp
        )

target_link_libraries(InstancingSample blue SDL2::SDL2main -lpthread)
target_include_directories(InstancingSample PRIVATE include)

add_custom_command(TARGET InstancingSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:InstancingSample>/resources
        )

install(TARGETS InstancingSample DESTINATION bin)
