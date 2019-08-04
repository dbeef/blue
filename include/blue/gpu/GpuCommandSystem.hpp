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
	std::future<UniformBufferId> submit(const CreateEnvironmentEntity&);
	std::future<TextureId> submit(const CreateTextureEntity&);

	void submit(const DisposeShaderEntity&);
	void submit(const DisposeMeshEntity&);
	void submit(const UpdateEnvironmentEntity_Projection&);
	void submit(const UpdateEnvironmentEntity_View&);
	void submit(const UpdateEnvironmentEntity_CameraPos&);
	void submit(const SetClearColorEntity&);

private:
	
	void execute();
	void lock();
	void unlock();

	std::queue<std::pair<std::promise<VertexArray>, CreateMeshEntity>> create_mesh_entities;
	std::queue<std::pair<std::promise<ShaderId>, CompileShaderEntity>> compile_shader_entities;
	std::queue<std::pair<std::promise<TextureId>, CreateTextureEntity>> create_texture_entities;
	std::queue<std::pair<std::promise<UniformBufferId>, CreateEnvironmentEntity>> create_env_entities;

	std::queue<SetClearColorEntity> set_clear_color_entities;
	std::queue<DisposeMeshEntity> dispose_mesh_entities;
	std::queue<DisposeShaderEntity> dispose_shader_entities;
	std::queue<UpdateEnvironmentEntity_CameraPos> update_environment_camera_pos_entities;
	std::queue<UpdateEnvironmentEntity_View> update_environment_view_entities;
	std::queue<UpdateEnvironmentEntity_Projection> update_environment_projection_entities;

	std::atomic_bool locked {false};
};
