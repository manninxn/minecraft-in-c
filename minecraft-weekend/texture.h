#ifndef TEXTURE_H
#define TEXTURE_H


#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <cglm/types-struct.h>
#include <stdbool.h>


struct Texture {
	GLuint handle;
	vec2s size;
};

struct Texture texture_create_from_path(const char* path);

void texture_bind(struct Texture texture);

void texture_delete(struct Texture texture);





#endif