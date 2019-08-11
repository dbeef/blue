cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(ImguiSample)

set(SOURCES src/Main.cpp)

if(${BLUE_PLATFORM} STREQUAL "Android")
    add_library(ImguiSample SHARED ${SOURCES})
else()
    add_executable(ImguiSample ${SOURCES})
endif()

target_link_libraries(ImguiSample blue SDL2::SDL2main)
target_include_directories(ImguiSample PRIVATE include)

install(TARGETS ImguiSample DESTINATION bin)
