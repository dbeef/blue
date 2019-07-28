#include "jobs/MapIntersectionJob.hpp"
#include "blue/Timestep.hpp"
#include "Application.hpp"
#include "blue/camera/PerspectiveCamera.hpp"
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace
{
	bool TestRayOBBIntersection(
		glm::vec3 ray_origin,        // Ray origin, in world space
		glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
		float& intersection_distance, // Output : distance between ray_origin and the intersection with the OBB
		glm::vec3 tile_position
	) {
		glm::vec3 tile_max(1.0f, 0.01f, 1.0f);       // Maximum X,Y,Z coords. Often tile_max*-1 if your mesh is centered, but it's not always the case.
		glm::vec3 tile_min(0.0f, -0.01f, 0.0f);       // Minimum X,Y,Z coords of the mesh when not transformed at all.

		// Transformation applied to the mesh (which will thus be also applied to its bounding box)
		glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.f), tile_position);

		// Intersection method from Real-Time Rendering and Essential Mathematics for Games

		float tMin = 0.0f;
		float tMax = std::numeric_limits<float>::max();

		glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);
		glm::vec3 delta = OBBposition_worldspace - ray_origin;
		// Test intersection with the 2 planes perpendicular to the OBB's X axis
		{
			glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
			float e = glm::dot(xaxis, delta);
			float f = glm::dot(ray_direction, xaxis);

			if (fabs(f) > 0.001f) { // Standard case

				float t1 = (e + tile_min.x) / f; // Intersection with the "left" plane
				float t2 = (e + tile_max.x) / f; // Intersection with the "right" plane
				// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

				// We want t1 to represent the nearest intersection,
				// so if it's not the case, invert t1 and t2

				if (t1 > t2) {
					float w = t1;
					t1 = t2;
					t2 = w; // swap t1 and t2
				}

				// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
				if (t2 < tMax)
					tMax = t2;
				// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
				if (t1 > tMin) {
					tMin = t1;
				}

				// And here's the trick :
				// If "far" is closer than "near", then there is NO intersection.
				// See the images in the tutorials for the visual explanation.
				if (tMax < tMin)
					return false;

			}
			else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
				if (-e + tile_min.x > 0.0f || -e + tile_max.x < 0.0f)
					return false;
			}
		}


		// Test intersection with the 2 planes perpendicular to the OBB's Y axis
		// Exactly the same thing than above.
		{
			glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
			float e = glm::dot(yaxis, delta);
			float f = glm::dot(ray_direction, yaxis);

			if (fabs(f) > 0.001f) {

				float t1 = (e + tile_min.y) / f;
				float t2 = (e + tile_max.y) / f;

				if (t1 > t2) {
					float w = t1;
					t1 = t2;
					t2 = w;
				}

				if (t2 < tMax)
					tMax = t2;
				if (t1 > tMin) {
					tMin = t1;
				}

				if (tMin > tMax)
					return false;

			}
			else {
				if (-e + tile_min.y > 0.0f || -e + tile_max.y < 0.0f)
					return false;
			}
		}


		// Test intersection with the 2 planes perpendicular to the OBB's Z axis
		// Exactly the same thing than above.
		{
			glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
			float e = glm::dot(zaxis, delta);
			float f = glm::dot(ray_direction, zaxis);

			if (fabs(f) > 0.001f) {

				float t1 = (e + tile_min.z) / f;
				float t2 = (e + tile_max.z) / f;

				if (t1 > t2) {
					float w = t1;
					t1 = t2;
					t2 = w;
				}

				if (t2 < tMax)
					tMax = t2;
				if (t1 > tMin) {
					tMin = t1;
				}
				if (tMin > tMax)
					return false;

			}
			else {
				if (-e + tile_min.z > 0.0f || -e + tile_max.z < 0.0f)
					return false;
			}
		}

		intersection_distance = tMin;
		return true;
	}
}

void MapIntersectionJob::intersection_loop()
{
	Timestep timestep(60);
	while (_running.load())
	{
		timestep.mark_start();
		if (blue::Context::window().is_cursor_attached() || Application::instance().input.intersection.load())
		{
			timestep.mark_end();
			timestep.delay();
		}

		glm::vec3 direction;
		auto& map = Application::instance().get_map();
		const auto& camera = Application::instance().map_environment.camera;

		glm::mat4 projection_matrix = glm::perspective(
			glm::radians(camera.get_fov()),
			(GLfloat)blue::Context::window().get_width() /
			(GLfloat)blue::Context::window().get_height(),
			0.1f, 1000.0f);

		glm::mat4 view_matrix = glm::lookAt(camera.get_position(), camera.get_position() + camera.get_front(), camera.get_up());

		float x = static_cast<float>((2.0f * blue::Context::window().get_last_x()) / blue::Context::window().get_width() - 1.0f);
		float y = static_cast<float>(1.0f - (2.0f * blue::Context::window().get_last_y()) / blue::Context::window().get_height());
		float z = 1.0f;

		glm::vec3 ray_nds(x, y, z);
		glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
		glm::vec4 ray_eye = inverse(projection_matrix) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
		glm::vec3 ray_wor = (inverse(view_matrix) * ray_eye);
		direction = glm::normalize(ray_wor);

		int best_result_tile_x = -1;
		int best_result_tile_y = -1;
		float closest_distance = std::numeric_limits<float>().max();
		{
			std::unique_lock<std::mutex> lock(map.tiles_access);

			for (uint32_t tile_x = 0; tile_x < Map::CHUNK_DIMENSION; tile_x++) {
				for (uint32_t tile_y = 0; tile_y < Map::CHUNK_DIMENSION; tile_y++) {

					float intersec_distance;
					bool result = TestRayOBBIntersection(camera.get_position(), direction, intersec_distance, { tile_x, 0.0f, tile_y });

					if (result) {
						if (intersec_distance < closest_distance) {
							closest_distance = intersec_distance;
							best_result_tile_x = tile_x;
							best_result_tile_y = tile_y;
						}
					}
				}
			}
		}

		if (best_result_tile_x >= 0 && Application::instance().input.clicked.load())
		{
			blue::Context::logger().info("Intersection at: {} {}", best_result_tile_x, best_result_tile_y);
			Application::instance().input.intersection.store(true);
			Application::instance().input.intersection_tile_x.store(best_result_tile_x);
			Application::instance().input.intersection_tile_y.store(best_result_tile_y);
			
			glm::vec3 intersection_point = camera.get_position() + (direction * closest_distance);

			Application::instance().input.intersection_point_x.store(intersection_point.x);
			Application::instance().input.intersection_point_y.store(intersection_point.z);
		}

		Application::instance().input.clicked.store(false);

		timestep.mark_end();
		timestep.delay();
	}
}

void MapIntersectionJob::shutdown()
{
	_running.store(false);
	_intersection_thread.join();
}

void MapIntersectionJob::start()
{
	_running.store(true);
	_intersection_thread = std::thread(&MapIntersectionJob::intersection_loop, this);
}
