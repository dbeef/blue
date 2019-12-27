#pragma once

#include <assimp/scene.h>
#include <vector>
#include <blue/Renderer.h>

namespace models 
{
	struct ParsedScene
	{
		Vertices vertices;
		std::vector<CreateTextureEntity> create_texture_entities;
	};

	const aiScene* load_scene(const std::string& path);
	std::vector<Vertices> parse_scene(const aiScene*& scene, const Attributes& attributes, unsigned int& vertex_counter);
	std::vector<CreateTextureEntity> parse_textures(const aiScene*& scene);
};