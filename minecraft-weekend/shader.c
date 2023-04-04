#include "shader.h"

void log_shader_info(GLuint handle) {
	int success;
	char infoLog[512];
	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(handle, 512, NULL, infoLog);
		printf("%s\n", infoLog);
	}
}

GLuint load_shader(const char* path, GLenum type) {

	GLuint out = glCreateShader(type);

	char* source = load_file(path);
	glShaderSource(out, 1, &source, NULL);
	glCompileShader(out);

	log_shader_info(out);

	return out;

}

struct Shader shader_create(const char* vertex_path, const char* fragment_path, size_t num_attributes, struct VertexAttribute attributes[]) {
	struct Shader self;
	self.vertex_handle = load_shader(vertex_path, GL_VERTEX_SHADER);
	self.fragment_handle = load_shader(fragment_path, GL_FRAGMENT_SHADER);
	self.program_handle = glCreateProgram();
	glAttachShader(self.program_handle, self.vertex_handle);
	glAttachShader(self.program_handle, self.fragment_handle);

	for (size_t i = 0; i < num_attributes; i++) {
		glBindAttribLocation(self.program_handle, attributes[i].handle, attributes[i].name);
	}


	glLinkProgram(self.program_handle);

	GLint success;
	char infoLog[512];
	glGetProgramiv(self.program_handle, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(self.program_handle, 512, NULL, infoLog);
		printf("%s\n", infoLog);
	}

	return self;
}

void shader_delete(struct Shader self) {
	glDeleteShader(self.vertex_handle);
	glDeleteShader(self.fragment_handle);
	glDeleteProgram(self.program_handle);
}

void shader_bind(struct Shader self) {
	glUseProgram(self.program_handle);
}

void shader_uniform_float(struct Shader self, char* name, float value) { glUniform1f(glGetUniformLocation(self.program_handle, name), value); }

void shader_uniform_vec2(struct Shader self, char* name, vec2 value) { glUniform2f(glGetUniformLocation(self.program_handle, name), value[0], value[1]); }

void shader_uniform_vec3(struct Shader self, char* name, vec3 value) { glUniform3f(glGetUniformLocation(self.program_handle, name), value[0], value[1], value[2]); }

void shader_uniform_vec4(struct Shader self, char* name, vec4 value) { glUniform4f(glGetUniformLocation(self.program_handle, name), value[0], value[1], value[2], value[3]); }

void shader_uniform_int(struct Shader self, char* name, int value) { glUniform1i(glGetUniformLocation(self.program_handle, name), value); }

void shader_uniform_ivec2(struct Shader self, char* name, ivec2 value) { glUniform2i(glGetUniformLocation(self.program_handle, name), value[0], value[1]);}

void shader_uniform_ivec3(struct Shader self, char* name, ivec3 value) { glUniform3i(glGetUniformLocation(self.program_handle, name), value[0], value[1], value[2]);}

void shader_uniform_ivec4(struct Shader self, char* name, ivec4 value) { glUniform4i(glGetUniformLocation(self.program_handle, name), value[0], value[1], value[2], value[3]);}

void shader_uniform_mat2(struct Shader self, char* name, mat2 value) { glUniformMatrix2fv(glGetUniformLocation(self.program_handle, name), 1, GL_FALSE, value); }

void shader_uniform_mat3(struct Shader self, char* name, mat3 value) { glUniformMatrix3fv(glGetUniformLocation(self.program_handle, name), 1, GL_FALSE, value);}

void shader_uniform_mat4(struct Shader self, char* name, mat4 value) { 
	glUniformMatrix4fv(glGetUniformLocation(self.program_handle, name), 1, GL_FALSE, (float*)value);
}

void shader_uniform_view_proj(struct Shader self, struct ViewProjection vp) {
	shader_uniform_mat4(self, "view", vp.view.raw);
	shader_uniform_mat4(self, "projection", vp.projection.raw);
}

void shader_uniform_texture(struct Shader self, char* name, struct Texture texture, GLuint n) {
	glActiveTexture(GL_TEXTURE0 + n);
	texture_bind(texture);
	glUniform1i(glGetUniformLocation(self.program_handle, (const GLchar*)name), n);
}