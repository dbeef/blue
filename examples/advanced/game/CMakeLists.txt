cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(GameSample)

add_executable(GameSample
        src/Main.cpp
        )

target_link_libraries(GameSample SDL2::SDL2main blue GameCommons -lpthread)
target_include_directories(GameSample PRIVATE include)

add_custom_command(TARGET GameSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:GameSample>/resources
        )

install(TARGETS GameSample DESTINATION bin)



