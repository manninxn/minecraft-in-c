#include "camera.h"
#include "state.h"
#include "world.h"


vec3 view_up = { 0, 1, 0 };

struct Camera camera_new(vec3s position, vec3s direction, float fov, float near_clip, float far_clip) {
	struct Camera camera = {
		.position = position,
		.look_direction = direction,
		.fov = fov,
		.near_clip = near_clip,
		.far_clip = far_clip,
		.last_delta_x = 0,
		.last_delta_y = 0,
		.pitch = 0,
		.yaw = 0,
		.speed = 7.5,
		.viewport_size = (ivec2s) {.x = 1920, .y = 1080}
	};

	return camera;

}

struct ViewProjection camera_gew_view_projection(struct Camera cam) {
	mat4s projection;
	glm_perspective(cam.fov, (float)cam.viewport_size.x / (float)cam.viewport_size.y, cam.near_clip, cam.far_clip, &projection.raw);
	mat4s view;
	glm_look(cam.position.raw, cam.look_direction.raw, view_up, &view.raw);
	
	struct ViewProjection vp = {
		.projection = projection,
		.view = view
	};

	return vp;
}

void camera_handle_input(struct Camera* cam) {
	vec3 delta = GLM_VEC3_ZERO_INIT;

	float cam_speed = cam->speed * 3 * state.delta_time;

	if (glfwGetKey(state.window, GLFW_KEY_W) == GLFW_PRESS) {
		vec3 out;
		glm_vec3_scale(cam->look_direction.raw, cam_speed, &out);
		glm_vec3_add(delta, out, &delta);
	}
	if (glfwGetKey(state.window, GLFW_KEY_S) == GLFW_PRESS) {
		vec3 out;
		glm_vec3_scale(cam->look_direction.raw, -cam_speed, &out);
		glm_vec3_add(delta, out, &delta);
	}

	if (glfwGetKey(state.window, GLFW_KEY_A) == GLFW_PRESS) {
		vec3 out;
		glm_cross(cam->look_direction.raw, view_up, &out);
		glm_normalize(&out);
		glm_vec3_scale(out, -cam_speed, &out);
		glm_vec3_add(delta, out, &delta);
	}

	if (glfwGetKey(state.window, GLFW_KEY_D) == GLFW_PRESS) {
		vec3 out;
		glm_cross(cam->look_direction.raw, view_up, &out);
		glm_normalize(&out);
		glm_vec3_scale(out, cam_speed, &out);
		glm_vec3_add(delta, out, &delta);
	}
	glm_vec3_add(cam->position.raw, delta, &cam->position.raw);


	if (glfwGetMouseButton(state.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

		ivec3s hit_pos;
		struct Chunk* containing_chunk;
		for (int i = 1; i < 5; i++) {
			vec3s pos;
			vec3s cam_pos = state.cam.position;

			vec3 ray;
			glm_vec3_scale(state.cam.look_direction.raw, i, &ray);
			glm_vec3_add(cam_pos.raw, ray, &pos);


			hit_pos.x = (int)pos.x;
			hit_pos.y = (int)pos.y;
			hit_pos.z = (int)pos.z;



			bool hit = get_block_at_world_pos(state.world, hit_pos, NULL, &containing_chunk);

			if (hit) {
				
				ivec3s block_pos = world_pos_to_block_pos(hit_pos);

				chunk_set_block(containing_chunk, block_pos, AIR);
				break;
			}

		}
	}

}

void camera_handle_mouse_movement(struct Camera* cam, float xpos, float ypos) {

	float xoffset = xpos - cam->last_delta_x;
	float yoffset = cam->last_delta_y - ypos; // reversed since y-coordinates go from bottom to top
	cam->last_delta_x = xpos;
	cam->last_delta_y = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	cam->yaw += xoffset;
	cam->pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (cam->pitch > 89.0f)
		cam->pitch = 89.0f;
	if (cam->pitch < -89.0f)
		cam->pitch = -89.0f;

	vec3 front;
	front[0] = cos(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch));
	front[1] = sin(glm_rad(cam->pitch));
	front[2] = sin(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch));
	glm_vec3_normalize_to(front, &cam->look_direction.raw);
	
	glm_vec3_normalize(&cam->look_direction.raw);
}
