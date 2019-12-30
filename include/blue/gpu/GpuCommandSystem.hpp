#pragma once

#include <future>
#include <queue>
#include <utility>
#include <atomic>

#include <blue/gpu/GpuEntities.hpp>
#include <blue/Renderer.h>

class GpuCommandSystem
{
public:

	friend class GpuThread;

	std::future<ShaderId> submit(const CompileShaderEntity&);
	std::future<VertexArray> submit(const CreateMeshEntity&);
	std::future<VertexArray> submit(const CreateInstancedMeshEntity&);
	std::future<Framebuffer> submit(const CreateFramebufferEntity&);
	std::future<UniformBufferId> submit(const CreateEnvironmentEntity&);
	std::future<Texture> submit(const CreateTextureEntity&);
	std::future<std::vector<char>> submit(const ReadFramebufferEntity&);
	std::future<bool> submit(const AddFramebufferTextureAttachmentEntity&);

	void submit(const UpdateUniformVariableEntity&);
	void submit(const DisposeShaderEntity&);
	void submit(const DisposeMeshEntity&);
	void submit(const DisposeTextureEntity&);
	void submit(const UpdateEnvironmentEntity_Projection&);
	void submit(const UpdateEnvironmentEntity_View&);
	void submit(const UpdateEnvironmentEntity_LightSpaceMatrix&);
	void submit(const UpdateEnvironmentEntity_CameraPos&);
	void submit(const UpdateEnvironmentEntity_LightPos&);
	void submit(const UpdateEnvironmentEntity_LightColor&);
	void submit(const UpdateEnvironmentEntity_AmbientStrength&);
	void submit(const SetClearColorEntity&);

private:
	
	bool execute();
	void lock();
	void unlock();

	std::queue<std::pair<std::promise<VertexArray>, CreateMeshEntity>> create_mesh_entities;
	std::queue<std::pair<std::promise<VertexArray>, CreateInstancedMeshEntity>> create_instanced_mesh_entities;
	std::queue<std::pair<std::promise<ShaderId>, CompileShaderEntity>> compile_shader_entities;
	std::queue<std::pair<std::promise<Texture>, CreateTextureEntity>> create_texture_entities;
	std::queue<std::pair<std::promise<Framebuffer>, CreateFramebufferEntity>> create_framebuffer_entities;
	std::queue<std::pair<std::promise<UniformBufferId>, CreateEnvironmentEntity>> create_env_entities;
	std::queue<std::pair<std::promise<std::vector<char>>, ReadFramebufferEntity>> read_framebuffer_entities;

	std::queue<UpdateUniformVariableEntity> update_uniform_variable_entities;
	std::queue<SetClearColorEntity> set_clear_color_entities;
	std::queue<std::pair<std::promise<bool>, AddFramebufferTextureAttachmentEntity>> add_framebuffer_texture_attachment_entities;
	std::queue<DisposeMeshEntity> dispose_mesh_entities;
	std::queue<DisposeTextureEntity> dispose_texture_entities;
	std::queue<DisposeShaderEntity> dispose_shader_entities;
	std::queue<UpdateEnvironmentEntity_CameraPos> update_environment_camera_pos_entities;
	std::queue<UpdateEnvironmentEntity_LightPos> update_environment_light_pos_entities;
	std::queue<UpdateEnvironmentEntity_View> update_environment_view_entities;
	std::queue<UpdateEnvironmentEntity_LightSpaceMatrix> update_environment_light_space_matrix_entities;
	std::queue<UpdateEnvironmentEntity_Projection> update_environment_projection_entities;
	std::queue<UpdateEnvironmentEntity_AmbientStrength> update_ambient_strength_entities;
	std::queue<UpdateEnvironmentEntity_LightColor> update_light_color_entities;

	std::atomic_bool locked {false};
};
