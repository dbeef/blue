#pragma once

#include "blue/gpu/GpuCommandSystem.hpp"

void handle(std::pair<std::promise<std::vector<char>>, ReadFramebufferEntity>& pair);
