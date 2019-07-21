//
// Created by dbeef on 5/18/19.
//

#ifndef OPENGL_PLAYGROUND_TANK_H
#define OPENGL_PLAYGROUND_TANK_H

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <memory>

#include "blue/RenderStep.h"
#include "blue/primitives/Mesh.h"

#undef far
#undef FAR
#undef near
#undef NEAR

class Tank {

public:

    Tank() = default;

    RenderStep mesh_step{};
    //RenderStep wireframe_step{};
	glm::vec3 wireframe_dimensions;

	//Dimensions: 12.625 16.2915 9.52601
	//Min : -6.68419 - 7.59735 - 16.2808
	//Max : 5.94079 1.92867 0.0106936

    void load_wireframe(glm::vec3 bb_min, glm::vec3 bb_max) {

        wireframe_dimensions.x = std::abs(bb_min.x) + std::abs(bb_max.x);
		wireframe_dimensions.y = std::abs(bb_min.y) + std::abs(bb_max.y);
		wireframe_dimensions.z = std::abs(bb_min.z) + std::abs(bb_max.z);

		std::cout << "Dimensions: " << wireframe_dimensions.x << " " << wireframe_dimensions.y << " " << wireframe_dimensions.z << std::endl;

		std::cout << "Min: " << bb_min.x << " " << bb_min.y << " " << bb_min.z << std::endl;
		std::cout << "Max: " << bb_max.x << " " << bb_max.y << " " << bb_max.z << std::endl;

        _wireframe = std::make_shared<Mesh>();

        std::vector<GLfloat> vertices;

        {
            for (const auto &v : Quad::vertices(Orientation::LEFT, wireframe_dimensions))vertices.push_back(v);
            for (const auto &v : Quad::vertices(Orientation::RIGHT, wireframe_dimensions))vertices.push_back(v);
            for (const auto &v : Quad::vertices(Orientation::CLOSE, wireframe_dimensions))vertices.push_back(v);
            for (const auto &v : Quad::vertices(Orientation::FAR, wireframe_dimensions))vertices.push_back(v);
            for (const auto &v : Quad::vertices(Orientation::UP, wireframe_dimensions))vertices.push_back(v);
            for (const auto &v : Quad::vertices(Orientation::DOWN, wireframe_dimensions))vertices.push_back(v);
        }

        _wireframe->upload_vertices(vertices);
    }

    // TODO: Calculate abs dimensions from bb_min / bb_max
    void load(const aiScene *&scene) {

		aiMatrix4x4 trafo;
		aiIdentityMatrix4(&trafo);
		auto &nd = scene->mRootNode;
		aiMultiplyMatrix4(&trafo, &nd->mTransformation);

        _mesh = std::make_shared<Mesh>();
        {
            std::vector<GLfloat> vertices;
            std::vector<GLuint> indices;
            for (unsigned long mesh_index = 0; mesh_index < scene->mNumMeshes; mesh_index++) {
                const auto &mesh = scene->mMeshes[mesh_index];

                for (unsigned long t = 0; t < mesh->mNumFaces; ++t) {
                    const struct aiFace *face = &mesh->mFaces[t];

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
//                    default: face_mode = GL_POLYGON; continue; break; <- compatibility mode
                    }


                    for (unsigned long v = 0; v < mesh->mNumVertices; v++) {
                        if (mesh->mColors[0] != nullptr) {
                            auto color = mesh->mColors[0][v];
                        }
                    }

                    aiColor4D current_color;

                    for (unsigned long i = 0; i < face->mNumIndices; i++) {

                        auto index = face->mIndices[i];
                        //std::cout << "Index: " << index << std::endl;

                        auto vertex = mesh->mVertices[index];
						//aiTransformVecByMatrix4(&vertex, &trafo);

                        vertices.push_back(vertex.x);
                        vertices.push_back(vertex.y);
                        vertices.push_back(vertex.z);

                        if (mesh->mColors[0] != nullptr) {
//                        if (i % 3 == 0)
                            current_color = mesh->mColors[0][index];
                            vertices.push_back(current_color.r);
                            vertices.push_back(current_color.g);
                            vertices.push_back(current_color.b);
                            //std::cout << current_color.r << " " << current_color.g << " " << current_color.b
                                      //<< std::endl;
                        } else {
                            vertices.push_back(0);
                            vertices.push_back(0);
                            vertices.push_back(0);
                        }

                        if (mesh->mNormals != nullptr) {
                            auto normal = mesh->mNormals[index];
                            vertices.push_back(normal.x);
                            vertices.push_back(normal.y);
                            vertices.push_back(normal.z);
                        } else {
                            vertices.push_back(0);
                            vertices.push_back(0);
                            vertices.push_back(0);
                        }
                    }
                }

            }
            _mesh->upload_vertices(vertices);
        }
    }

    std::shared_ptr<Mesh> _mesh = nullptr;
    std::shared_ptr<Mesh> _wireframe = nullptr;

};

#endif //OPENGL_PLAYGROUND_TANK_H
