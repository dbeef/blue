cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(GameCommons)

add_library(GameCommons STATIC
        src/terrain/Map.cpp
        src/terrain/Tile.cpp
        src/Resources.cpp
        src/terrain/Flora.cpp
        src/terrain/Water.cpp
        include/terrain/Flora.hpp
        include/terrain/Map.hpp
        include/terrain/Tile.hpp
        include/terrain/Water.hpp
        )

target_link_libraries(GameCommons blue)
target_include_directories(GameCommons PUBLIC include)

install(TARGETS GameCommons DESTINATION bin)



