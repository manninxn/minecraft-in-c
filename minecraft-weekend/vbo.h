#ifndef VBO_H
#define VBO_H

#include "gfx_common.h"
#include <stdbool.h>

struct VBO {
	GLint handle;
	GLint type;
	bool dynamic;
};

/*
Creates a new vbo
type: type of buffer (is it an array buffer or an element array buffer?
dynamic: whether to use static draw or dynamic draw when buffering data
*/
struct VBO vbo_create(GLint type, bool dynamic);

/*
destroys the buffer
*/
void vbo_destroy(struct VBO self);


/*
buffers the vbo with new data
data: the data to buffer the vbo with
offset: the offset of the vbo (can usually be left as zero)
size: the size of the data

this will bind the vbo!
*/
void vbo_buffer(struct VBO self, void* data, size_t offset, size_t size);

/*
binds the vbo
*/
void vbo_bind(struct VBO self);


/*
lol i forgot i was trying to document everything
*/
void vbo_clear(struct VBO self);

void* vbo_map(struct VBO self);

void vbo_unmap(struct VBO self);

#endif