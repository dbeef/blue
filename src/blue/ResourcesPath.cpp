//
// Created by dbeef on 7/15/19.
//

#include "blue/ResourcesPath.h"
#include <string>
#include <assert.h>
#include <SDL.h>

namespace {
    const char *CURRENT_PLATFORM = BLUE_PLATFORM;
}

const char *paths::getResourcesPath() {
    std::string platformStr = CURRENT_PLATFORM;

    if (platformStr == "Linux") {
        return "";
    } else if (platformStr == "Windows") {
		// FIXME: Should take absolute path for working directory.
		static const char* base_path = SDL_GetBasePath();
		return base_path;
    } else if (platformStr == "Android") {
        return "";
    } else {
        assert(false);
        return "";
    }
}
