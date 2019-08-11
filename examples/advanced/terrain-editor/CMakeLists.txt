cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(TerrainEditorSample)

set(SOURCES
        src/Main.cpp
        src/Application.cpp
        src/states/BaseState.cpp
        src/states/Greeting.cpp
        src/states/ModelingTerrain.cpp
        src/states/SelectingClickableBlocks.cpp
        src/jobs/MapIntersectionJob.cpp
        include/states/BaseState.hpp
        include/states/Greeting.hpp
        include/states/SelectingClickableBlocks.hpp
        include/states/ModelingTerrain.hpp
        include/Application.hpp
        include/jobs/MapIntersectionJob.hpp
        )

if(${BLUE_PLATFORM} STREQUAL "Android")
    add_library(TerrainEditorSample SHARED ${SOURCES})
else()
    add_executable(TerrainEditorSample ${SOURCES})
endif()

target_link_libraries(TerrainEditorSample blue SDL2::SDL2main GameCommons)
target_include_directories(TerrainEditorSample PRIVATE include)

add_custom_command(TARGET TerrainEditorSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:TerrainEditorSample>/resources
        )

install(TARGETS TerrainEditorSample DESTINATION bin)
