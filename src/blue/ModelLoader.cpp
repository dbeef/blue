#include "blue/ModelLoader.h"
#include "blue/Context.hpp"
#include "blue/ResourcesPath.h"

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
}

const aiScene* models::load_scene(const std::string& path)
{
	blue::Context::logger().info("Loading scene: {}", path);

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
	scene = aiImportFileFromMemory(data.data(), size, aiProcessPreset_TargetRealtime_MaxQuality, "fbx");

	aiVector3D scene_min, scene_max, scene_center;

	if (!scene)
	{
		return nullptr;
	}
	else
	{
		blue::Context::logger().info("Number of animations: {}", scene->mNumAnimations);
		blue::Context::logger().info("Number of cameras: {}", scene->mNumCameras);
		blue::Context::logger().info("Number of lights: {}", scene->mNumLights);
		blue::Context::logger().info("Number of meshes: {}", scene->mNumMeshes);
		blue::Context::logger().info("Number of textures: {}", scene->mNumTextures);
		blue::Context::logger().info("Number of materials: {}", scene->mNumMaterials);
		return scene;
	}
}

std::vector<VertexType> models::parse_scene(const aiScene*& scene, const std::vector<ShaderAttribute>& attributes, unsigned int& vertex_counter)
{
	std::vector<VertexType> vertices;
	vertex_counter = 0;

	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);
	auto& nd = scene->mRootNode;
	aiMultiplyMatrix4(&trafo, &nd->mTransformation);

	{
		std::vector<GLfloat> vertices;
		std::vector<GLuint> indices;
		for (unsigned long mesh_index = 0; mesh_index < scene->mNumMeshes; mesh_index++) {
			const auto& mesh = scene->mMeshes[mesh_index];

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
					vertex_counter++;

					for (const auto& attribute : attributes)
					{
						switch (attribute._purpose)
						{
						case(ShaderAttribute::Purpose::VERTEX_POSITION):
						{
							vertices.push_back(vertex.x);
							vertices.push_back(vertex.y);
							vertices.push_back(vertex.z);
							break;
						}
						case(ShaderAttribute::Purpose::COLOR):
						{
							if (mesh->mColors[0] != nullptr) {
								current_color = mesh->mColors[0][index];
								vertices.push_back(current_color.r);
								vertices.push_back(current_color.g);
								vertices.push_back(current_color.b);
							}
							else {
								vertices.push_back(0);
								vertices.push_back(0);
								vertices.push_back(0);
							}
							break;
						}
						case(ShaderAttribute::Purpose::NORMAL):
						{
							if (mesh->mNormals != nullptr) {
								auto normal = mesh->mNormals[index];
								vertices.push_back(normal.x);
								vertices.push_back(normal.y);
								vertices.push_back(normal.z);
							}
							else {
								vertices.push_back(0);
								vertices.push_back(0);
								vertices.push_back(0);
							}
						}
						}
					}
				}
			}
		}

		return vertices;
	}
}
