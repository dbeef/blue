cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(TerrainEditorSample)

add_executable(TerrainEditorSample
        src/Main.cpp
        src/Application.cpp
        src/Resources.cpp
        src/states/BaseState.cpp
        src/states/Greeting.cpp
        src/states/SelectingClickableBlocks.cpp
        include/states/BaseState.hpp
        include/states/Greeting.hpp
        include/states/SelectingClickableBlocks.hpp
        include/Resources.hpp
        include/Application.hpp
        )

target_link_libraries(TerrainEditorSample blue -lpthread)
target_include_directories(TerrainEditorSample PRIVATE include)

install(TARGETS TerrainEditorSample DESTINATION bin)