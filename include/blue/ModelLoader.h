#pragma once

#include <assimp/scene.h>
#include <vector>
#include <blue/Renderer.h>

namespace models 
{
	const aiScene* load_scene(const std::string& path);
	Vertices parse_scene(const aiScene*& scene, const Attributes& attributes, unsigned int& vertex_counter);
};