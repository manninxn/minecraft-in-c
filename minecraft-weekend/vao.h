#ifndef VAO_H
#define VAO_H

#include "gfx_common.h"
#include "vbo.h"


struct VAO {
	GLuint handle;
};

/*
creates a new vertex array object
*/
struct VAO vao_create();

/*
destroy a vertex array object
*/
void vao_destroy(struct VAO self);


/*
bind a vertex array object
*/
void vao_bind(struct VAO self);


/*
set a vertex attribute pointer

*/
void vao_attribute(
    struct VAO self, struct VBO vbo, GLuint index, GLint size, GLenum type,
    GLsizei stride, size_t offset);




#endif VAO_H