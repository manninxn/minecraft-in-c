#include "vbo.h"


struct VBO vbo_create(GLint type, bool dynamic) {
	struct VBO self = {
		.dynamic = dynamic,
		.type = type
	};

	glGenBuffers(1, &self.handle);

	return self;

}

void vbo_destroy(struct VBO self) {
	glDeleteBuffers(1, &self.handle);
}

void vbo_buffer(struct VBO self, void* data, size_t offset, size_t size) {
	vbo_bind(self);
	
	glBufferData(self.type, size - offset, data, self.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void vbo_bind(struct VBO self) {
	glBindBuffer(self.type, self.handle);
}

void vbo_clear(struct VBO self) {
	
	glBufferData(self.type, 0, NULL, self.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}
