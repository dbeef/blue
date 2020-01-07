//
// Created by dbeef on 7/15/19.
//

#include "blue/ResourcesPath.h"
#include <string>
#include <assert.h>

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

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
