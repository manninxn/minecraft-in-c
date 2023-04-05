#ifndef STATE_H
#define STATE_H

#include <GLFW/glfw3.h>
#include "shader.h"
#include "camera.h"
#include <Windows.h>


struct ThreadData {
	struct World* world;
	int shouldTerminate;
};

struct State {
	GLFWwindow* window;
	struct World* world;
	struct Shader shader;
	struct Shader skybox;
	struct Camera cam;
	struct ThreadData generation_worker_data;
	double delta_time;
};

extern struct State state;



#endif