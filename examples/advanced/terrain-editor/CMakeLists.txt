cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(TerrainEditorSample)

add_executable(TerrainEditorSample
        src/Main.cpp
        src/Application.cpp
        src/Resources.cpp
        src/states/BaseState.cpp
        src/states/Greeting.cpp
        src/states/ModelingTerrain.cpp
        src/states/SelectingClickableBlocks.cpp
        src/terrain/Map.cpp
        src/terrain/Tile.cpp
        src/jobs/MapIntersectionJob.cpp
        include/states/BaseState.hpp
        include/states/Greeting.hpp
        include/states/SelectingClickableBlocks.hpp
        include/states/ModelingTerrain.hpp
        include/Resources.hpp
        include/Application.hpp
        include/terrain/Map.hpp
        include/terrain/Tile.hpp
        include/jobs/MapIntersectionJob.hpp
        )

target_link_libraries(TerrainEditorSample blue -lpthread)
target_include_directories(TerrainEditorSample PRIVATE include)

add_custom_command(TARGET TerrainEditorSample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:TerrainEditorSample>/resources
        )

install(TARGETS TerrainEditorSample DESTINATION bin)



