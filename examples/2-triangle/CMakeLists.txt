cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(TriangleSample)

add_executable(TriangleSample
        src/Main.cpp
        )

target_link_libraries(TriangleSample blue -lpthread)
target_include_directories(TriangleSample PRIVATE include)

add_custom_command(TARGET TriangleSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:TriangleSample>/resources
        )

install(TARGETS TriangleSample DESTINATION bin)
