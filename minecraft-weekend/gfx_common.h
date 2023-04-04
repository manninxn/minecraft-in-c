#ifndef GFX_COMMON
#define GFX_COMMON
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "utils.h"
#include <cglm/types-struct.h>

struct ViewProjection {
	mat4s view;
	mat4s projection;
};

#endif