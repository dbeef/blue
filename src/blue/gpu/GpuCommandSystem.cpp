#include "blue/gpu/GpuCommandSystem.hpp"
#include "blue/gpu/handlers/CompileShaderHandler.hpp"
#include "blue/gpu/handlers/DisposeShaderHandler.hpp"
#include "blue/gpu/handlers/DisposeMeshHandler.hpp"
#include "blue/gpu/handlers/CreateMeshHandler.hpp"
#include "blue/gpu/handlers/CreateFramebufferHandler.hpp"
#include "blue/gpu/handlers/CreateInstancedMeshHandler.hpp"
#include "blue/gpu/handlers/CreateTextureHandler.hpp"
#include "blue/gpu/handlers/CreateEnvironmentHandler.hpp"
#include "blue/gpu/handlers/UpdateEnvironmentHandler.hpp"
#include "blue/gpu/handlers/SetClearColorHandler.hpp"
#include "blue/gpu/handlers/UpdateUniformVariableHandler.hpp"
#include "blue/Context.hpp"

bool GpuCommandSystem::execute()
{
    bool executed_some_work = false;

	while (!compile_shader_entities.empty())
	{
		handle(compile_shader_entities.front());
		compile_shader_entities.pop();
        executed_some_work = true;
	}

	while (!dispose_shader_entities.empty())
	{
		handle(dispose_shader_entities.front());
		dispose_shader_entities.pop();
        executed_some_work = true;
	}

	while (!create_mesh_entities.empty())
	{
		handle(create_mesh_entities.front());
		create_mesh_entities.pop();
        executed_some_work = true;
	}

	while (!create_instanced_mesh_entities.empty())
	{
		handle(create_instanced_mesh_entities.front());
		create_instanced_mesh_entities.pop();
		executed_some_work = true;
	}

	while (!dispose_mesh_entities.empty())
	{
		handle(dispose_mesh_entities.front());
		dispose_mesh_entities.pop();
        executed_some_work = true;
	}

	while (!update_uniform_variable_entities.empty())
	{
		handle(update_uniform_variable_entities.front());
		update_uniform_variable_entities.pop();
        executed_some_work = true;
	}

	while (!create_framebuffer_entities.empty())
	{
		handle(create_framebuffer_entities.front());
		create_framebuffer_entities.pop();
        executed_some_work = true;
	}

	while (!create_env_entities.empty())
	{
		handle(create_env_entities.front());
		create_env_entities.pop();
        executed_some_work = true;
	}

	while (!update_environment_view_entities.empty())
	{
		handle(update_environment_view_entities.front());
		update_environment_view_entities.pop();
        executed_some_work = true;
	}

	while (!update_environment_light_space_matrix_entities.empty())
	{
		handle(update_environment_light_space_matrix_entities.front());
		update_environment_light_space_matrix_entities.pop();
        executed_some_work = true;
	}

	while (!update_environment_camera_pos_entities.empty())
	{
		handle(update_environment_camera_pos_entities.front());
		update_environment_camera_pos_entities.pop();
        executed_some_work = true;
	}

	while (!update_environment_light_pos_entities.empty())
	{
		handle(update_environment_light_pos_entities.front());
		update_environment_light_pos_entities.pop();
        executed_some_work = true;
	}

	while (!update_environment_projection_entities.empty())
	{
		handle(update_environment_projection_entities.front());
		update_environment_projection_entities.pop();
        executed_some_work = true;
	}

	while (!create_texture_entities.empty())
	{
		handle(create_texture_entities.front());
		create_texture_entities.pop();
        executed_some_work = true;
	}

	while (!set_clear_color_entities.empty())
	{
		handle(set_clear_color_entities.front());
		set_clear_color_entities.pop();
		executed_some_work = true;
	}

	return executed_some_work;
}

 void GpuCommandSystem::lock()
{
	bool expected = false;
	const bool desired = true;

	while (!locked.compare_exchange_weak(expected, desired))
	{
		// busy wait
		expected = false;
	}
}

 void GpuCommandSystem::unlock()
{
	locked.store(false);
}

std::future<ShaderId> GpuCommandSystem::submit(const CompileShaderEntity& entity)
{
	std::promise<ShaderId> promise;
	std::future<ShaderId> future = promise.get_future();
	
	auto pair = std::make_pair(std::move(promise), entity);
	lock();
	compile_shader_entities.push(std::move(pair));
	unlock();

	return future;
}

std::future<VertexArray> GpuCommandSystem::submit(const CreateMeshEntity& entity)
{
	std::promise<VertexArray> promise;
	std::future<VertexArray> future = promise.get_future();

	auto pair = std::make_pair(std::move(promise), entity);
	lock();
	create_mesh_entities.push(std::move(pair));
	unlock();

	return future;
}

std::future<VertexArray> GpuCommandSystem::submit(const CreateInstancedMeshEntity& entity)
{
	std::promise<VertexArray> promise;
	std::future<VertexArray> future = promise.get_future();

	auto pair = std::make_pair(std::move(promise), entity);
	lock();
	create_instanced_mesh_entities.push(std::move(pair));
	unlock();

	return future;
}

std::future<UniformBufferId> GpuCommandSystem::submit(const CreateEnvironmentEntity& entity)
{
	std::promise<UniformBufferId> promise;
	std::future<UniformBufferId> future = promise.get_future();

	auto pair = std::make_pair(std::move(promise), entity);
	lock();
	create_env_entities.push(std::move(pair));
	unlock();

	return future;
}

std::future<Framebuffer> GpuCommandSystem::submit(const CreateFramebufferEntity& entity)
{
	std::promise<Framebuffer> promise;
	std::future<Framebuffer> future = promise.get_future();

	auto pair = std::make_pair(std::move(promise), entity);
	lock();
	create_framebuffer_entities.push(std::move(pair));
	unlock();

	return future;
}

std::future<Texture> GpuCommandSystem::submit(const CreateTextureEntity& entity)
{
	std::promise<Texture> promise;
	std::future<Texture> future = promise.get_future();

	auto pair = std::make_pair(std::move(promise), entity);
	lock();
	create_texture_entities.push(std::move(pair));
	unlock();

	return future;
}

void GpuCommandSystem::submit(const DisposeShaderEntity& entity)
{
	lock();
	dispose_shader_entities.push(entity);
	unlock();
}

void GpuCommandSystem::submit(const UpdateUniformVariableEntity& entity)
{
	lock();
	update_uniform_variable_entities.push(entity);
	unlock();
}

void GpuCommandSystem::submit(const DisposeMeshEntity& entity)
{
	lock();
	dispose_mesh_entities.push(entity);
	unlock();
}

void GpuCommandSystem::submit(const UpdateEnvironmentEntity_Projection& entity)
{
	lock();
	update_environment_projection_entities.push(entity);
	unlock();
}

void GpuCommandSystem::submit(const UpdateEnvironmentEntity_CameraPos& entity)
{
	lock();
	update_environment_camera_pos_entities.push(entity);
	unlock();
}

void GpuCommandSystem::submit(const UpdateEnvironmentEntity_LightPos& entity)
{
	lock();
	update_environment_light_pos_entities.push(entity);
	unlock();
}

void GpuCommandSystem::submit(const UpdateEnvironmentEntity_View& entity)
{
	lock();
	update_environment_view_entities.push(entity);
	unlock();
}

void GpuCommandSystem::submit(const UpdateEnvironmentEntity_LightSpaceMatrix& entity)
{
	lock();
	update_environment_light_space_matrix_entities.push(entity);
	unlock();
}

void GpuCommandSystem::submit(const SetClearColorEntity& entity)
{
	lock();
	set_clear_color_entities.push(entity);
	unlock();
}
