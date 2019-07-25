#pragma once

#include <string>

namespace ShaderUtils
{
	std::string parseFile(std::string filename);
}

class ShaderSource
{
public:

	// TODO: Return CompileShaderEntity
	ShaderSource(const std::string vertex_path, const std::string fragment_path);

	std::string vertex;
	std::string fragment;

	inline bool valid() 
	{
		return _valid;
	}

private:

	bool _valid = true;
};
