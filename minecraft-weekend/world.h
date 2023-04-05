#ifndef WORLD_H
#define WORLD_H

#include "ivec3s.h"
#include "chunk.h"
#include "stack.h"
#define RENDER_DISTANCE 16
#define MAX_CHUNKS (RENDER_DISTANCE * RENDER_DISTANCE * RENDER_DISTANCE)


struct World {
	ivec3s world_loaded_position;
	struct Chunk** chunks;
	Stack* queued_updates;
};

struct BlockQueryParams {
	struct Chunk* calling_chunk;
	ivec3s block_position;
	ivec3s world_position;
};

struct World* world_create();

void world_render(struct World* world);


struct Chunk* world_get_chunk_at_chunk_coordinate(struct World* world, ivec3s pos);


/*
returns block status
-1: block is not loaded
0: block does not exist
1: block exists
*/
int get_block_at_world_pos(struct World* world, ivec3s world_pos, BlockId* block, struct Chunk** containing_chunk);

void world_set_loaded_position(struct World* world, ivec3s new_pos);

#define chunk_coord_to_index(_x,_y,_z) (\
((_x) * RENDER_DISTANCE * RENDER_DISTANCE) + \
((_y) * RENDER_DISTANCE) + \
(_z))


static inline size_t world_chunk_index(struct World* self, ivec3s offset) {
	
	ivec3s p = glms_ivec3_sub(offset, self->world_loaded_position);
	return (p.x * RENDER_DISTANCE * RENDER_DISTANCE) + p.y * RENDER_DISTANCE + p.z;
}

static inline ivec3s world_chunk_offset(struct World* self, size_t index) {

	// Calculate the z index of the chunk within the grid
	size_t z = index % RENDER_DISTANCE;

	// Calculate the y index of the chunk within the grid
	size_t y = (index / RENDER_DISTANCE) % RENDER_DISTANCE;

	// Calculate the x index of the chunk within the grid
	size_t x = index / (RENDER_DISTANCE * RENDER_DISTANCE);

	// Calculate the offset of the chunk from the loaded position
	ivec3s offset = {
		(int)x + self->world_loaded_position.x,
		(int)y + self->world_loaded_position.y,
		(int)z + self->world_loaded_position.z
	};

	return offset;
}

void world_load_unloaded_chunks(struct World* world, int count);

static inline bool world_chunk_in_bounds(struct World* self, ivec3s offset) {

	ivec3s p;
	glm_ivec3_sub(offset.raw, self->world_loaded_position.raw, &p.raw);
	
	return p.x >= 0 && p.y >= 0 && p.z >= 0 &&
		p.x < RENDER_DISTANCE && p.y < RENDER_DISTANCE && p.z < RENDER_DISTANCE;
}

void world_update(struct World* world);



#define world_pos_to_block_pos(world_pos) (ivec3s) {\
			.x = (world_pos.x < 0 ? CHUNK_SIZE : 0) + (world_pos.x % CHUNK_SIZE == 0 && world_pos.x < 0 ? -CHUNK_SIZE : world_pos.x % CHUNK_SIZE),		\
			.y = (world_pos.y < 0 ? CHUNK_SIZE : 0) + (world_pos.y % CHUNK_SIZE == 0  && world_pos.y < 0? -CHUNK_SIZE : world_pos.y % CHUNK_SIZE),		\
			.z = (world_pos.z < 0 ? CHUNK_SIZE : 0) + (world_pos.z % CHUNK_SIZE == 0  && world_pos.z < 0 ? -CHUNK_SIZE : world_pos.z % CHUNK_SIZE),		\
}

#define world_pos_to_chunk_pos(world_pos) (ivec3s) {\
	.x = (float)world_pos.x / (float)CHUNK_SIZE,	\
	.y = (float)world_pos.y / (float)CHUNK_SIZE,	\
	.z = (float)world_pos.z / (float)CHUNK_SIZE,	\
}



static inline bool world_pos_in_chunk_bounds(ivec3s world_pos, struct Chunk* chunk) {
	ivec3s chunk_pos = world_pos_to_chunk_pos(world_pos);
return  chunk_pos.x == chunk->chunk_pos.x &&
		chunk_pos.y == chunk->chunk_pos.y &&
		chunk_pos.z == chunk->chunk_pos.z;
}

#endif