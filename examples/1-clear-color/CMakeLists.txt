cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(ClearColorSample)

add_executable(ClearColorSample
        src/Main.cpp
        )

target_link_libraries(ClearColorSample blue -lpthread)
target_include_directories(ClearColorSample PRIVATE include)

install(TARGETS ClearColorSample DESTINATION bin)
