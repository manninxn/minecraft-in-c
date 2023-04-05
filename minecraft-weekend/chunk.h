#ifndef CHUNK_H
#define CHUNK_H


#include "block.h"
#include <cglm/types-struct.h>
#include <cglm/cglm.h>
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "vbo.h"
#include "vao.h"
#define CHUNK_SIZE 16

struct World;

struct Chunk {
	BlockId blocks[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];
	ivec3s chunk_pos;
	bool empty;
	struct VAO vao;
	struct VBO vbo;
	struct VBO ebo;
	int vert_count;
	int index_count;
	int block_count;
	struct World* world;
	HANDLE generation_mutex;
	unsigned int* vertex_buffer;
	unsigned int* index_buffer;
	bool updating;
	bool loaded;
	bool changed;

};

#define chunk_pos_to_index(p) (p.x * CHUNK_SIZE * CHUNK_SIZE + p.z * CHUNK_SIZE.+ p.y)

struct Chunk* chunk_new(struct World* world, ivec3s pos);

void chunk_update(struct Chunk* chunk, struct World* world);

void chunk_set_block(struct Chunk* chunk, ivec3s position, BlockId block);

bool chunk_get_block(struct Chunk* chunk, ivec3s position, BlockId* out);

void chunk_render(struct Chunk* chunk);

void chunk_destroy(struct Chunk* chunk);

void flag_chunk_update(struct Chunk* chunk);

// https://github.com/jdah/minecraft-weekend/blob/master/src/world/chunk.h#L40
static inline bool chunk_in_bounds(ivec3s pos) {
	return pos.x >= 0 && pos.y >= 0 && pos.z >= 0 &&
		pos.x < CHUNK_SIZE && pos.y < CHUNK_SIZE && pos.z < CHUNK_SIZE;
}

#define block_pos_to_world_pos(pos, chunk) (ivec3s) {\
	.x = pos.x + (chunk->chunk_pos.x * CHUNK_SIZE),				\
	.y = pos.y + (chunk->chunk_pos.y * CHUNK_SIZE),			\
	.z = pos.z + (chunk->chunk_pos.z * CHUNK_SIZE),			\
}

#define chunk_pos_to_index(p) (p.x * CHUNK_SIZE * CHUNK_SIZE + p.y * CHUNK_SIZE + p.z)

#define chunk_xyz_to_index(x,y,z) (x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z)

static inline int CHUNK_WORLD_BLOCK_EXISTS(struct Chunk* chunk, ivec3s pos) {																										
    return chunk_in_bounds(pos) ?																
        chunk_get_block(chunk, pos, NULL) :
        get_block_at_world_pos(chunk->world, block_pos_to_world_pos(pos, chunk), NULL, NULL);
}

//https://github.com/jdah/minecraft-weekend/blob/master/src/world/chunk.h
#define chunk_foreach(_pname)\
    ivec3s _pname = (ivec3s) {.x = 0, .y = 0, .z = 0};\
    for (int x = 0; x < CHUNK_SIZE; x++)\
        for (int z = 0; z < CHUNK_SIZE; z++)\
            for (int y = 0;\
                y < CHUNK_SIZE &&\
                ((_pname.x = x) != INT32_MAX) &&\
                ((_pname.y = y) != INT32_MAX) &&\
                ((_pname.z = z) != INT32_MAX);\
                y++)

#endif