#include "blue/ModelLoader.h"
#include "blue/Context.hpp"
#include "blue/ResourcesPath.h"
#include "blue/TextureUtils.hpp"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>

// TODO: Rename to ModelUtils

// TODO: Refactor this as it's a frankenstein from assimp's C example

#define aisgl_min(x, y) (x<y?x:y)
#define aisgl_max(x, y) (y>x?y:x)

namespace
{

	void get_bounding_box_for_node(const aiScene* scene,
		const aiNode* nd,
		aiVector3D* min,
		aiVector3D* max,
		aiMatrix4x4* trafo
	) {
		aiMatrix4x4 prev;
		unsigned int n = 0, t;

		prev = *trafo;
		aiMultiplyMatrix4(trafo, &nd->mTransformation);

		for (; n < nd->mNumMeshes; ++n) {
			const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
			for (t = 0; t < mesh->mNumVertices; ++t) {

				aiVector3D tmp = mesh->mVertices[t];
				//aiTransformVecByMatrix4(&tmp, trafo);

				min->x = aisgl_min(min->x, tmp.x);
				min->y = aisgl_min(min->y, tmp.y);
				min->z = aisgl_min(min->z, tmp.z);

				max->x = aisgl_max(max->x, tmp.x);
				max->y = aisgl_max(max->y, tmp.y);
				max->z = aisgl_max(max->z, tmp.z);
			}
		}

		for (n = 0; n < nd->mNumChildren; ++n) {
			get_bounding_box_for_node(scene, nd->mChildren[n], min, max, trafo);
		}
		*trafo = prev;
	}

	void get_bounding_box(const aiScene* scene, aiVector3D* min, aiVector3D* max) {
		aiMatrix4x4 trafo;
		aiIdentityMatrix4(&trafo);

		min->x = min->y = min->z = 1e10f;
		max->x = max->y = max->z = -1e10f;
		get_bounding_box_for_node(scene, scene->mRootNode, min, max, &trafo);
	}

	bool is_normalized_height_requested(const Attributes& attributes)
	{
		for (const auto& attribute : attributes)
		{
			if (attribute._purpose == ShaderAttribute::Purpose::NORMALIZED_HEIGHT) return true;
		}
		return false;
	}

	float find_max_height(const aiScene*& scene)
	{
		// TODO: Real implementation. 
		return 2.0f;
	}

	float find_min_height(const aiScene*& scene)
	{
		return 0;
	}

	std::pair<Vertices, unsigned int> process_mesh(aiMesh* mesh, const Attributes& attributes, float span, aiMatrix4x4 transformation, const aiScene* scene)
	{
		std::pair<Vertices, unsigned int> vertices;

		blue::Context::logger().info("Mesh: {}", mesh->mName.C_Str());

		for (unsigned long t = 0; t < mesh->mNumFaces; ++t) {

			const struct aiFace* face = &mesh->mFaces[t];

			GLenum face_mode;
			switch (face->mNumIndices) {
			case 1:
				face_mode = GL_POINTS;
				continue;
				break;
			case 2:
				face_mode = GL_LINES;
				continue;
				break;
			case 3:
				face_mode = GL_TRIANGLES;
				break;
			}

			aiColor4D current_color;

			for (unsigned long i = 0; i < face->mNumIndices; i++)
			{
				auto index = face->mIndices[i];
				auto vertex = mesh->mVertices[index];

				vertices.second++;

				for (const auto& attribute : attributes)
				{
					switch (attribute._purpose)
					{
					case(ShaderAttribute::Purpose::VERTEX_POSITION):
					{
						aiVector3D transformed = transformation * vertex;

						vertices.first.push_back(transformed.x);
						vertices.first.push_back(transformed.y);
						vertices.first.push_back(transformed.z);
						break;
					}
					case(ShaderAttribute::Purpose::COLOR):
					{
						if (mesh->mColors[0] != nullptr) {
							current_color = mesh->mColors[0][index];

							vertices.first.push_back(current_color.r);
							vertices.first.push_back(current_color.g);
							vertices.first.push_back(current_color.b);
						}
						else {
							vertices.first.push_back(0);
							vertices.first.push_back(0);
							vertices.first.push_back(0);
						}
						break;
					}
					case(ShaderAttribute::Purpose::MATERIAL_AMBIENT):
					{
						auto material_index = mesh->mMaterialIndex;
						const aiMaterial* material = scene->mMaterials[material_index];

						for (std::size_t material_property_index = 0; material_property_index < material->mNumProperties; material_property_index++)
						{
							const aiMaterialProperty* material_property = material->mProperties[material_property_index];
							if (material_property->mKey == aiString("$raw.Ambient"))
							{
								switch (material_property->mType)
								{
								case(aiPropertyTypeInfo::aiPTI_Float):
								{
									float* p = (float*)material_property->mData;
									vertices.first.push_back(*(p + 0));
									vertices.first.push_back(*(p + 1));
									vertices.first.push_back(*(p + 2));
									break;
								}
								case(aiPropertyTypeInfo::aiPTI_Double):
								{
									double* p = (double*)material_property->mData;
									vertices.first.push_back(*(p + 0));
									vertices.first.push_back(*(p + 1));
									vertices.first.push_back(*(p + 2));
									break;
								}
								default:
								{
									BLUE_ASSERT(false);
									break;
								}
								}
							}
						}

						break;
					}
					case(ShaderAttribute::Purpose::MATERIAL_DIFFUSE):
					{
						auto material_index = mesh->mMaterialIndex;
						const aiMaterial* material = scene->mMaterials[material_index];

						for (std::size_t material_property_index = 0; material_property_index < material->mNumProperties; material_property_index++)
						{
							const aiMaterialProperty* material_property = material->mProperties[material_property_index];
							if (material_property->mKey == aiString("$raw.Diffuse"))
							{
								switch (material_property->mType)
								{
								case(aiPropertyTypeInfo::aiPTI_Float):
								{
									float* p = (float*)material_property->mData;
									vertices.first.push_back(*(p + 0));
									vertices.first.push_back(*(p + 1));
									vertices.first.push_back(*(p + 2));
									break;
								}
								case(aiPropertyTypeInfo::aiPTI_Double):
								{
									double* p = (double*)material_property->mData;
									vertices.first.push_back(*(p + 0));
									vertices.first.push_back(*(p + 1));
									vertices.first.push_back(*(p + 2));
									break;
								}
								default:
								{
									BLUE_ASSERT(false);
									break;
								}
								}
							}
						}

						break;
					}
					case(ShaderAttribute::Purpose::MATERIAL_SPECULAR):
					{
						auto material_index = mesh->mMaterialIndex;
						const aiMaterial* material = scene->mMaterials[material_index];

						for (std::size_t material_property_index = 0; material_property_index < material->mNumProperties; material_property_index++)
						{
							const aiMaterialProperty* material_property = material->mProperties[material_property_index];
							if (material_property->mKey == aiString("$raw.Specular"))
							{
								switch (material_property->mType)
								{
								case(aiPropertyTypeInfo::aiPTI_Float):
								{
									float* p = (float*)material_property->mData;
									vertices.first.push_back(*(p + 0));
									vertices.first.push_back(*(p + 1));
									vertices.first.push_back(*(p + 2));
									break;
								}
								case(aiPropertyTypeInfo::aiPTI_Double):
								{
									double* p = (double*)material_property->mData;
									vertices.first.push_back(*(p + 0));
									vertices.first.push_back(*(p + 1));
									vertices.first.push_back(*(p + 2));
									break;
								}
								default:
								{
									BLUE_ASSERT(false);
									break;
								}
								}
							}
						}

						break;
					}
					case(ShaderAttribute::Purpose::MATERIAL_SHININESS):
					{
						auto material_index = mesh->mMaterialIndex;
						const aiMaterial* material = scene->mMaterials[material_index];

						for (std::size_t material_property_index = 0; material_property_index < material->mNumProperties; material_property_index++)
						{
							const aiMaterialProperty* material_property = material->mProperties[material_property_index];
							if (material_property->mKey == aiString("$raw.Shininess"))
							{
								switch (material_property->mType)
								{
								case(aiPropertyTypeInfo::aiPTI_Float):
								{
									float* p = (float*)material_property->mData;
									vertices.first.push_back(*(p));
									break;
								}
								case(aiPropertyTypeInfo::aiPTI_Double):
								{
									double* p = (double*)material_property->mData;
									vertices.first.push_back(*(p));
									break;
								}
								default:
								{
									BLUE_ASSERT(false);
									break;
								}
								}
							}
						}

						break;
					}
					case(ShaderAttribute::Purpose::TEXTURE_COORDINATE):
					{
						if (mesh->mTextureCoords[0] != nullptr) {

							// mesh->mMaterialIndex; !!!
							// Obchodz�  mnie tylko diffuse tekstury 

							auto uv = mesh->mTextureCoords[0][index];
							vertices.first.push_back(uv.x);
							vertices.first.push_back(uv.y);

							// TODO: Push texture index?

							// Ignore Z component, will be 0.
						}
						else {
							vertices.first.push_back(0);
							vertices.first.push_back(0);
						}
						break;
					}
					case(ShaderAttribute::Purpose::NORMAL):
					{
						if (mesh->HasNormals() && mesh->mNormals != nullptr) {

							auto normal = mesh->mNormals[index];

							aiVector3D transformed = transformation * normal;
							transformed = transformed.Normalize();

							vertices.first.push_back(transformed.x);
							vertices.first.push_back(transformed.y);
							vertices.first.push_back(transformed.z);
						}
						else {
							vertices.first.push_back(0);
							vertices.first.push_back(0);
							vertices.first.push_back(0);
						}
						break;
					}
					case(ShaderAttribute::Purpose::NORMALIZED_HEIGHT):
					{
						float current_height = std::abs(vertex.y);
						float normalized = current_height / span;

						vertices.first.push_back(normalized);
						break;
					}
					}
				}
			}
		}
		return vertices;
	}

	std::vector<std::pair<Vertices, unsigned int>> process_node(aiNode* node, aiNode* parent, const aiScene* scene, const Attributes& attributes, float span, std::vector<std::pair<Vertices, unsigned int>>& meshes, aiMatrix4x4& transformation)
	{
		blue::Context::logger().info("Current node: {}", node->mName.C_Str());

		aiMatrix4x4 trafo;
		aiIdentityMatrix4(&trafo);
		aiMultiplyMatrix4(&trafo, &transformation);
		aiMultiplyMatrix4(&trafo, &node->mTransformation);

		for (std::size_t mesh_index = 0; mesh_index < node->mNumMeshes; mesh_index++)
		{
			unsigned int internal_mesh_index = node->mMeshes[mesh_index];
			aiMesh* mesh = scene->mMeshes[internal_mesh_index];
			meshes.push_back(process_mesh(mesh, attributes, span, trafo, scene));
		}

		if (node->mNumChildren)
		{
			for (std::size_t node_index = 0; node_index < node->mNumChildren; node_index++)
			{
				aiIdentityMatrix4(&trafo);
				aiMultiplyMatrix4(&trafo, &transformation);
				aiMultiplyMatrix4(&trafo, &node->mTransformation);

				process_node(node->mChildren[node_index], node, scene, attributes, span, meshes, trafo);
			}
		}

		return meshes;
	}
}

const aiScene* models::load_scene(const std::string& path)
{
	blue::Context::logger().info("*** Loading scene: {} ***", path);

	auto full_path = paths::getResourcesPath() + path;

	std::vector<char> data;
	unsigned int size = 0;
	SDL_RWops* file = SDL_RWFromFile(full_path.c_str(), "rb");

	if (file != nullptr)
	{
		size = file->size(file);
		blue::Context::logger().info("Scene size: {}", size);

		data.reserve(size);
		data.resize(size);

		auto read = file->read(file, &data[0], size, 1);
		SDL_RWclose(file);
	}
	else
	{
		blue::Context::logger().error("Failed to read scene: {}", SDL_GetError());
		return nullptr;
	}

	const aiScene* scene;
	scene = aiImportFileFromMemory(data.data(), size, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FixInfacingNormals | aiProcess_PreTransformVertices, "fbx");

	aiVector3D scene_min, scene_max, scene_center;

	return scene;
}

std::vector<CreateTextureEntity> models::parse_textures(const aiScene*& scene)
{
	std::vector<CreateTextureEntity> entities;

	blue::Context::logger().info("*** Parsing textures ***");
	blue::Context::logger().info("Number of animations: {}", scene->mNumAnimations);
	blue::Context::logger().info("Number of cameras: {}", scene->mNumCameras);
	blue::Context::logger().info("Number of lights: {}", scene->mNumLights);
	blue::Context::logger().info("Number of meshes: {}", scene->mNumMeshes);
	blue::Context::logger().info("Number of textures: {}", scene->mNumTextures);

	for (std::size_t index = 0; index < scene->mNumTextures; index++)
	{
		blue::Context::logger().info("Texture {} name {}", index, scene->mTextures[index]->mFilename.C_Str());
	}

	blue::Context::logger().info("Number of materials: {}", scene->mNumMaterials);

	for (std::size_t index = 0; index < scene->mNumMaterials; index++)
	{
		blue::Context::logger().info("Material {} name {}", index, scene->mMaterials[index]->GetName().C_Str());

		blue::Context::logger().info("--- Properties: ");
		for (std::size_t property_index = 0; property_index < scene->mMaterials[index]->mNumProperties; property_index++)
		{
			auto material_property = scene->mMaterials[index]->mProperties[property_index];
			blue::Context::logger().info("--- Property: {}, type: {}, data length: {}", material_property->mKey.C_Str(), (int)material_property->mType, (int)material_property->mDataLength);
		}

		for (std::size_t textureType = 0; textureType < 12; textureType++) {

			unsigned int textureCount = scene->mMaterials[index]->GetTextureCount(static_cast<aiTextureType>(aiTextureType_NONE + textureType));
			if (textureCount != 0)
			{
				blue::Context::logger().info("--- Texture type: {}, texture count: {}", textureType, textureCount);

				for (int textureIndex = 0; textureIndex < textureCount; textureIndex++)
				{
					aiString outPath;
					aiReturn tex = scene->mMaterials[index]->GetTexture(static_cast<aiTextureType>(aiTextureType_NONE + textureType), textureIndex, &outPath);
					if (tex == aiReturn_SUCCESS)
					{
						blue::Context::logger().info("------- Texture: {}", outPath.C_Str());
						entities.push_back(ImageUtils::read(outPath.C_Str()));
					}
				}
			}
		}
	}

	return entities;
}

std::vector<std::pair<Vertices, unsigned int>> models::parse_scene(const aiScene*& scene, const Attributes& attributes)
{
	blue::Context::logger().info("*** Parsing scene ***");

	float max_height = 0.0f;
	float min_height = 0.0f;
	float span = 0.0f;

	if (is_normalized_height_requested(attributes))
	{
		max_height = find_max_height(scene);
		min_height = find_min_height(scene);
		span = std::abs(max_height) + std::abs(min_height);
	}

	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);

	std::vector<std::pair<Vertices, unsigned int>> meshes;
	process_node(scene->mRootNode, scene->mRootNode, scene, attributes, span, meshes, trafo);

	return meshes;
}
