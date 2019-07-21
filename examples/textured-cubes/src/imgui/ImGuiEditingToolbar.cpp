//
// Created by dbeef on 5/26/19.
//

#include <Application.h>
#include "ImGuiEditingToolbar.hpp"

// https://github.com/ocornut/imgui/issues/1096
// https://github.com/ocornut/imgui/issues/1658

void ImGuiEditingToolbar::batchCalls() {

    ImGui::Begin("Level editor; Toolbar");

    const char *edit_mode_items[] = {
            "TERRAIN",
            "MODEL",
    };
    ImGui::ListBox("\nEdit mode", &_current_edit_mode, edit_mode_items, IM_ARRAYSIZE(edit_mode_items), 2);
    auto edit_mode = static_cast<EditMode>(_current_edit_mode);

    if(edit_mode == EditMode::TERRAIN) {

        const char *listbox_items[] = {
                "NAVIGATION",
                "GRASS",
                "DIRT",
                "STONE",
                "SUN"
        };
        ImGui::ListBox("\nCube type", &_current_cube_type, listbox_items, IM_ARRAYSIZE(listbox_items), 4);

    } else {

        const char *listbox_items[] = {
                "TREE",
                "BOULDER",
                "GRASS",
                "PINE TREE",
                "HURDLE",
                "WHEAT",
        };
        ImGui::ListBox("\nModel type", &_current_model_type, listbox_items, IM_ARRAYSIZE(listbox_items), 6);

    }

    ImGui::End();
}

