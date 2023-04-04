#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"

#define RENDER_DISTANCE 16

struct World {
	ivec3s world_loaded_position;
	struct Chunk** chunks;
};


struct World* world_create();

void world_render(struct World* world);


struct Chunk* world_get_chunk_at_chunk_coordinate(struct World* world, ivec3s pos);


bool get_block_at_world_pos(struct World* world, ivec3s world_pos, BlockId* block, struct Chunk** containing_chunk);

void world_set_loaded_position(struct World* world, ivec3s new_pos);

#define chunk_coord_to_index(x,y,z) (\
((x) * RENDER_DISTANCE * RENDER_DISTANCE) + \
((y) * RENDER_DISTANCE) + \
(z))

static inline size_t world_chunk_index(struct World* self, ivec3s offset) {
	
	ivec3s p;
	glm_ivec3_sub(offset.raw, self->world_loaded_position.raw, &p.raw);
	return (p.x * RENDER_DISTANCE * RENDER_DISTANCE) + p.y * RENDER_DISTANCE + p.z;
}

void world_load_unloaded_chunks(struct World* world, int count);

static inline bool world_chunk_in_bounds(struct World* self, ivec3s offset) {

	ivec3s p;
	glm_ivec3_sub(offset.raw, self->world_loaded_position.raw, &p.raw);
	
	return p.x >= 0 && p.y >= 0 && p.z >= 0 &&
		p.x < RENDER_DISTANCE&& p.y < RENDER_DISTANCE&& p.z < RENDER_DISTANCE;
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



#define NUM_CHUNKS (RENDER_DISTANCE * RENDER_DISTANCE * RENDER_DISTANCE)


#endif