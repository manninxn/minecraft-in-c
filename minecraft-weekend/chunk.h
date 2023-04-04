#ifndef CHUNK_H
#define CHUNK_H


#include "block.h"
#include <cglm/types-struct.h>
#include <cglm/cglm.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "vbo.h"
#include "vao.h"
#define CHUNK_SIZE 16

struct World;

struct Chunk {
	BlockId blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	ivec3s chunk_pos;
	struct VAO vao;
	struct VBO vertex_buffer;
	struct VBO index_buffer;
	int vert_count;
	int index_count;
	int block_count;
	struct World* world;

	bool updating;

};

#define chunk_pos_to_index(p) (p.x * CHUNK_SIZE * CHUNK_SIZE + p.z * CHUNK_SIZE.+ p.y)

struct Chunk* chunk_new(struct World* world, ivec3s pos);

void chunk_update(struct Chunk* chunk, struct World* world);

void chunk_set_block(struct Chunk* chunk, ivec3s position, BlockId block);

bool chunk_get_block(struct Chunk* chunk, ivec3s position, BlockId* out);

void chunk_render(struct Chunk* chunk);

void chunk_destroy(struct Chunk* chunk);

void flag_chunk_update(struct Chunk* chunk);

#endif