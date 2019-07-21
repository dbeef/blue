cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(ImguiSample)

add_executable(ImguiSample
        src/Main.cpp
        )

target_link_libraries(ImguiSample blue -lpthread)
target_include_directories(ImguiSample PRIVATE include)

install(TARGETS ImguiSample DESTINATION bin)
