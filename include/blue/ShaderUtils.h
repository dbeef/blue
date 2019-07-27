#pragma once

#include "blue/gpu/GpuEntities.hpp"
#include <string>

namespace ShaderUtils
{
	std::string read(const std::string& filename);
	CompileShaderEntity make_entity(const std::string vertex_path, const std::string fragment_path);
}
