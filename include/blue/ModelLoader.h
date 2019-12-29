#pragma once

#include <assimp/scene.h>
#include <vector>
#include <tuple>
#include <blue/Renderer.h>

namespace models 
{
	struct ParsedScene
	{
		Vertices vertices;
		std::vector<CreateTextureEntity> create_texture_entities;
	};

	const aiScene* load_scene(const std::string& path);
	std::vector<std::pair<Vertices, unsigned int>> parse_scene(const aiScene*& scene, const Attributes& attributes);
	std::vector<CreateTextureEntity> parse_textures(const aiScene*& scene);
};