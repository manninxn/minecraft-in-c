#ifndef SHADER_H
#define SHADER_H

#include "gfx_common.h"
#include <cglm/cglm.h>
#include <cglm/struct.h>
#include "texture.h"

struct Shader {
	GLuint program_handle, vertex_handle, fragment_handle;
};

struct VertexAttribute {
	GLuint handle;
	GLchar* name;
};

/*
loads a shader from a path.
path: the file path string
*/
struct Shader shader_create(const char* vertex_path, const char* fragment_path, size_t num_attributes, struct VertexAttribute attributes[]);


/*
deletes a shader
*/
void shader_delete(struct Shader self);

/*
use a shader
*/
void shader_bind(struct Shader self);


void shader_uniform_float(struct Shader self, char* name, float value);

void shader_uniform_vec2(struct Shader self, char* name, vec2 value);

void shader_uniform_vec3(struct Shader self, char* name, vec3 value);

void shader_uniform_vec4(struct Shader self, char* name, vec4 value);

void shader_uniform_int(struct Shader self, char* name, int value);

void shader_uniform_ivec2(struct Shader self, char* name, ivec2 value);

void shader_uniform_ivec3(struct Shader self, char* name, ivec3 value);

void shader_uniform_ivec4(struct Shader self, char* name, ivec4 value);

void shader_uniform_mat2(struct Shader self, char* name, mat2 value);

void shader_uniform_mat3(struct Shader self, char* name, mat3 value);

void shader_uniform_mat4(struct Shader self, char* name, mat4 value);

void shader_uniform_view_proj(struct Shader self, struct ViewProjection vp);

void shader_uniform_texture(struct Shader self, char* name, struct Texture texutre, GLuint n);

#endif