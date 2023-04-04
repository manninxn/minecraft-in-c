#define STB_IMAGE_IMPLEMENTATION

#include <cglm/cglm.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "utils.h"
#include "vao.h"
#include "vbo.h"
#include "shader.h"
#include "state.h"
#include "camera.h"
#include "gfx_common.h"
#include "texture.h"
#include "chunk.h"
#include "world.h"
#include <time.h>


struct State state;


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	camera_handle_mouse_movement(&state.cam, (float)xposIn, (float)yposIn);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	state.cam.fov += yoffset * 0.02;
}

void window_resize_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	state.cam.viewport_size = (ivec2s){ .x = width, .y = height };
}

#define get_random_range(lower, upper) ((rand() % (upper - lower + 1)) + lower)


#define WORLD_SIZE 16

int main() {

	state.delta_time = 0.2;

	block_init();

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(1920, 1080, "TEST", NULL, NULL);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetWindowSizeCallback(window, window_resize_callback);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gladLoadGL();
	glViewport(0, 0, 1920, 1080);

	glfwShowWindow(window);

	state.window = window;

	//	chunk_update(NULL);

	struct Shader shader = shader_create("vertex.glsl", "fragment.glsl", 2, (struct VertexAttribute[]) {
		{.handle = 0, .name = "vertex_data" },
		{ .handle = 1, .name = "lighting_data" },
	});
	state.shader = shader;


	state.skybox = shader_create("skybox_vert.glsl", "skybox_frag.glsl", 1, (struct VertexAttribute[]) {
		{.handle = 0, .name = "vertex_pos"}
	});

	struct VBO skybox_vertex_buffer = vbo_create(GL_ARRAY_BUFFER, true);
	struct VBO skybox_index_buffer = vbo_create(GL_ELEMENT_ARRAY_BUFFER, true);
	struct VAO skybox_vao = vao_create();

	float skybox_quad[] = {
         1.0f,  1.0f, 0.0f,  // top right
         1.0f, -1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f,  // bottom left
        -1.0f,  1.0f, 0.0f   // top left 
	};
	unsigned int skybox_indices[] = {
		0,1,3,
		1,2,3
	};
	vao_bind(skybox_vao);
	vbo_buffer(skybox_vertex_buffer, skybox_quad, 0, sizeof(skybox_quad));
	vbo_buffer(skybox_index_buffer, skybox_indices, 0, sizeof(skybox_indices));
	

	state.cam = camera_new((vec3s) { 0, 67, -5 }, (vec3s) { 0, 0, 1 }, 64, 0.01, 1000);



	struct World* world = world_create();

	state.world = world;

	struct Texture texture = texture_create_from_path("texture_atlas.png");

	vec3 axis = { 0, 1.0, 0.0 };




	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	srand(100);
	int i = 0;
	clock_t last = clock();
	float dt = 0;
	
	while (!glfwWindowShouldClose(window)) {
		i++;
		
		shader_uniform_vec3(state.shader, "camera_pos", state.cam.position.raw);


		//print_sizes();
		printf("%f,%f,%f\n", state.cam.position.x, state.cam.position.y, state.cam.position.z);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		glActiveTexture(GL_TEXTURE0);
		texture_bind(texture);

		camera_handle_input(&state.cam);

		ivec3s world_center = {
			.x = RENDER_DISTANCE/2,
			.y = RENDER_DISTANCE / 2,
			.z = RENDER_DISTANCE / 2
		};
		ivec3s cam_chunk = world_pos_to_chunk_pos(state.cam.position);

		glm_ivec3_sub(cam_chunk.raw, world_center.raw, &world_center.raw);

		world_set_loaded_position(state.world, world_center);


		world_update(state.world);


		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		shader_bind(state.skybox);
		shader_uniform_vec3(state.skybox, "look_dir", state.cam.look_direction.raw);
		vao_attribute(skybox_vao, skybox_vertex_buffer, 0, 3, GL_FLOAT, 3 * sizeof(float), 0);
		vao_bind(skybox_vao);
		vbo_bind(skybox_index_buffer);
		
		glDrawElements(GL_TRIANGLES, sizeof(skybox_indices) / sizeof(int), GL_UNSIGNED_INT, 0);



		shader_bind(shader);

		
		struct ViewProjection view_proj = camera_gew_view_projection(state.cam);

		shader_uniform_view_proj(shader, view_proj);
		shader_uniform_texture(shader, "texture1", texture, 0);
		shader_uniform_ivec2(shader, "atlas_params", (ivec2) { 16, 256 });

		mat4 transform = GLM_MAT4_IDENTITY_INIT;
		shader_uniform_mat4(shader, "model", transform);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		world_render(world);




		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		clock_t now = clock();
		state.delta_time = (float)(now - last) / (float)CLOCKS_PER_SEC;
		last = now;





		//printf("%f\n", 1.0 / state.delta_time);

	};

	glfwTerminate();

	return 0;
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


}