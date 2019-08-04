cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(GameCommons)

add_library(GameCommons STATIC
        src/terrain/Map.cpp
        src/terrain/Tile.cpp
        src/Resources.cpp
        )

target_link_libraries(GameCommons blue -lpthread)
target_include_directories(GameCommons PUBLIC include)

install(TARGETS GameCommons DESTINATION bin)



