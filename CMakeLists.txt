cmake_minimum_required(VERSION 3.10)

project(blue)

include(cmake/CurrentPlatform.cmake)
include(cmake/Dependencies.cmake)
include(cmake/Tests.cmake)

blue_detect_platform()
blue_find_dependencies()

option(BLUE_ADD_TESTS "Run tests after build." ON)
if (BLUE_ADD_TESTS)
    blue_add_tests()
endif ()

option(BLUE_EXAMPLES "Additionally build code samples as an artifact." ON)
if (BLUE_EXAMPLES)
    add_subdirectory(examples)
endif ()

add_subdirectory(vendor)

add_library(
        blue STATIC
        src/blue/Window.cpp
        src/blue/Logger.cpp
        src/blue/Renderer.cpp
        src/blue/Context.cpp
        src/blue/ResourcesPath.cpp
        src/blue/InputHandler.cpp
        src/blue/Timestep.cpp
        src/blue/gpu/handlers/CompileShaderHandler.cpp
        src/blue/gpu/handlers/DisposeShaderHandler.cpp
        src/blue/gpu/handlers/CreateMeshHandler.cpp
        src/blue/gpu/handlers/DisposeMeshHandler.cpp
        src/blue/gpu/handlers/CreateEnvironmentHandler.cpp
        src/blue/gpu/GpuCommandSystem.cpp
        src/blue/ShaderUtils.cpp
        src/blue/OrthographicCamera.cpp
        src/blue/PerspectiveCamera.cpp
        src/blue/gpu/handlers/UpdateEnvironmentHandler.cpp
        src/blue/ModelLoader.cpp
        src/blue/gpu/handlers/SetClearColorHandler.cpp
        src/blue/GpuThread.cpp
        include/blue/GpuThread.hpp
        include/blue/gpu/handlers/SetClearColorHandler.hpp
        include/blue/ModelLoader.h
        include/blue/gpu/handlers/UpdateEnvironmentHandler.hpp
        include/blue/ShaderUtils.h
        include/blue/gpu/GpuEntities.hpp
        include/blue/gpu/handlers/CompileShaderHandler.hpp
        include/blue/gpu/handlers/DisposeShaderHandler.hpp
        include/blue/gpu/handlers/CreateMeshHandler.hpp
        include/blue/gpu/handlers/DisposeMeshHandler.hpp
        include/blue/gpu/handlers/CreateEnvironmentHandler.hpp
        include/blue/gpu/GpuCommandSystem.hpp
        include/blue/Assertions.h
        include/blue/GlDebugging.h
        include/blue/Timestep.hpp
        include/blue/Renderer.h
        include/blue/Context.hpp
        include/blue/Logger.hpp
        include/blue/ResourcesPath.h
        include/blue/InputHandler.h
        include/blue/Window.hpp
        include/blue/PerspectiveCamera.hpp
        include/blue/OrthographicCamera.hpp
)

target_include_directories(
        blue
        PUBLIC
        include
        ${OPENGL_INCLUDE_DIRS}
)

target_link_libraries(
        blue
        INTERFACE
        Dependencies
        PRIVATE
        Dependencies
)

install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-export
        LIBRARY DESTINATION shlib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)

install(
        DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp*"
)

export(PACKAGE blue)
