cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

project(SimpleExample)

add_executable(${PROJECT_NAME}
        src/Main.cpp
        src/input/IntersectionTestJob.h
        src/input/IntersectionTestJob.cpp
        src/input/InputCallbacks.h
        src/input/Input.hpp
        src/input/Input.cpp
        src/entities/Tank.h
        src/input/MouseButtonInputCallbacks.h
        src/input/MouseInputCallbacks.h
        src/input/KeyboardInputCallbacks.h
        src/Application.h
        src/Application.cpp
        src/imgui/ImGuiEditingSummary.cpp
        src/imgui/ImGuiEditingSummary.hpp
        src/imgui/ImGuiEditingToolbar.cpp
        src/imgui/ImGuiEditingToolbar.hpp
        src/imgui/ImGuiGreeting.hpp
        src/imgui/ImGuiNewPerlinLevelParameters.hpp
        src/imgui/ImGuiGreeting.cpp
        src/imgui/ImGuiNewPerlinLevelParameters.cpp
        src/imgui/ImGuiGeneratorOptionsWindow.h
        src/imgui/ImGuiGeneratorOptionsWindow.cpp
        src/imgui/ImGuiNewFlatLevelParameters.hpp
        src/imgui/ImGuiNewFlatLevelParameters.cpp
        src/Resources.h
        src/states/Editing.hpp
        src/states/ApplicationState.hpp
        src/states/StartedApplication.hpp
        src/states/NewPerlinLevel.hpp
        src/states/NewFlatLevel.hpp
        src/Resources.cpp
        src/entities/Tank.h
        src/entities/Sun.hpp
        src/entities/Brush.hpp
        src/entities/Brush.cpp
        )

target_link_libraries(${PROJECT_NAME} blue -lpthread)

target_include_directories(${PROJECT_NAME} PRIVATE src)

add_custom_command(TARGET SimpleExample PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:SimpleExample>/resources
        )

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
