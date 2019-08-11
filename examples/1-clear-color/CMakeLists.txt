cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(ClearColorSample)

set(SOURCES src/Main.cpp)

if(${BLUE_PLATFORM} STREQUAL "Android")
    add_library(ClearColorSample SHARED ${SOURCES})
else()
    add_executable(ClearColorSample ${SOURCES})
endif()

target_link_libraries(ClearColorSample blue SDL2::SDL2main)
target_include_directories(ClearColorSample PRIVATE include)

install(TARGETS ClearColorSample DESTINATION bin)
