#pragma once

#include "blue/Assertions.h"

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <memory>
#include <vector>
#include <string>

struct ShaderAttribute
{
	enum class Type
	{
		FLOAT,
		VEC2,
		VEC3,
		VEC4,
		INT
	};

	enum class Purpose
	{
		VERTEX_POSITION,
		TEXTURE_COORDINATE,
		COLOR,
		NORMAL,
		NORMALIZED_HEIGHT,
		OTHER,
		TRANSLATION,
		QUATERNION,
		MODEL,
	};

	enum class Buffer
	{
		VERTEX,
		INSTANCED
	};

	ShaderAttribute(Type type, const Purpose& purpose, const Buffer& buffer) : _type(type), _purpose(purpose), _buffer(buffer) {}

	std::size_t getNumOfComponents() const
	{
		switch (_type)
		{
		case Type::FLOAT: return 1;
		case Type::VEC2: return 2;
		case Type::VEC3: return 3;
		case Type::VEC4: return 4;
		default: BLUE_ASSERT(false); return 0;
		}
	}

	std::size_t getSize() const
	{
		return sizeof(float) * getNumOfComponents();
	}

	const Type _type;
	const Purpose _purpose;
	const Buffer _buffer;
};

using VertexType = float;
using IndexType = unsigned int;
using InstanceType = float;

using Vertices = std::vector<VertexType>;
using Indices = std::vector<IndexType>;
using Instances = std::vector<InstanceType>;
using Attributes = std::vector<ShaderAttribute>;

using ShaderId = GLuint;
using TextureId = GLuint;
using TextureSlot = GLuint;
using VertexArrayId = GLuint;
using IndexBufferId = GLuint;
using VertexBufferId = GLuint;
using InstanceBufferId = GLuint;
using UniformBufferId = GLuint;
using FramebufferId = GLuint;

enum class FramebufferAttachmentType
{
	// For rendering only depth component of scene:
	DEPTH_ATTACHMENT,
	// For discarding fragments based on some condition:
	STENCIL_ATTACHMENT,
	// For both stencil/depth attachment.
	DEPTH_STENCIL_ATTACHMENT,
	// For rendering color component of scene:
	COLOR_ATTACHMENT,
};

struct Texture
{
    TextureSlot slot = 0;
    TextureId id = 0;
};

struct Environment
{
	glm::mat4 view;
	glm::mat4 projection;
};

struct VertexArray
{
	VertexArrayId vao; 
	VertexBufferId vbo;
	IndexBufferId ibo; 
	std::uint32_t vertices_count{}; 
	// Zeroed if instanced rendering is not used:
	std::uint32_t number_of_instances{};
};

struct Framebuffer
{
	FramebufferId framebuffer = 0;
	TextureId texture = 0;
	std::uint16_t texture_width = 0;
	std::uint16_t texture_height = 0;
};

struct CreateFramebufferEntity
{
	bool with_texture;
	// If with_texture passed with true, following will be used:
	FramebufferAttachmentType attachmentType;
	std::uint16_t texture_width;
	std::uint16_t texture_height;
};

struct UpdateUniformVariableEntity
{
	ShaderAttribute::Type type;
	const void* value;
	ShaderId program;
	// If explicit uniform location is used, one can specify location:
	GLint location = -1;
	// Used to query GPU for uniform location, if `location` is zero:
	std::string name;
};

struct CompileShaderEntity
{
	std::string vertex;
	std::string fragment;
};

struct CreateMeshEntity
{
	const Vertices& vertices;
	const Indices& indices;
	const Attributes& attributes;
	const std::uint32_t indices_count{};
};

struct CreateInstancedMeshEntity
{
	const VertexArray& vao;
	const Attributes& attributes;
	const Instances& instances;
	const std::uint32_t number_of_instances;
};

struct CreateTextureEntity
{
	// Will not attempt to fill with data if empty vector passed.
	std::shared_ptr<std::vector<char>> data;
	// Optional, used when data empty.
	std::uint16_t width;
	std::uint16_t height;
	TextureSlot slot = 0;
	// TODO: Texture format
	// TODO: Filtering
	// TODO: Wrapping
};

struct DisposeShaderEntity
{
	const ShaderId& shader;
};

struct SetClearColorEntity
{
	const glm::vec3 color;
};

struct DisposeMeshEntity
{
};

struct CreateEnvironmentEntity
{
};

struct UpdateEnvironmentEntity_View
{
	const UniformBufferId id;
	const glm::mat4 view;
};

struct UpdateEnvironmentEntity_LightSpaceMatrix
{
	const UniformBufferId id;
	const glm::mat4 light_space_matrix;
};

struct UpdateEnvironmentEntity_LightPos
{
	const UniformBufferId id;
	const glm::vec3 light_position;
};

struct UpdateEnvironmentEntity_CameraPos
{
	const UniformBufferId id;
	const glm::vec3 camera_position;
};

struct UpdateEnvironmentEntity_Projection
{
	const UniformBufferId id;
	const glm::mat4 projection;
};
