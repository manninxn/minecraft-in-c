#include "chunk.h"
#include "world.h"
#include <stdio.h>
#define min(a,b) (a > b ? b : a)
#define max(a,b) (a > b ? a : b)
#include "state.h"
#include "face.h"

int num_chunks = 0;

const ivec3 face_directions[6] = {
	+0, +0, +1, //NORTH
	+0, +0, -1, //SOUTH
	+1, +0, +0, //EAST
	-1, +0, +0, //WEST
	+0, +1, +0, //TOP
	+0, -1, +0, //BOTTOM
};


struct Chunk* chunk_new(struct World* world, ivec3s pos) {
	struct Chunk* chunk = malloc(sizeof(struct Chunk));

	chunk->chunk_pos = pos;											
	chunk->vbo = vbo_create(GL_ARRAY_BUFFER, true);
	chunk->ebo = vbo_create(GL_ELEMENT_ARRAY_BUFFER, true);
	chunk->vao = vao_create();
	chunk->index_count = ((CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE) / 2) * 6 * 6;
	chunk->vert_count = ((CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE) / 2) * (6 * 8) + 8;
	//chunk->blocks = calloc(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, sizeof(BlockId));
	chunk->block_count = 0;
	chunk->world = world;
	chunk->updating = false;
	chunk->empty = true;
	num_chunks++;



	return chunk;						   
}

size_t vertex_buffer_size = 0;
size_t index_buffer_size = 0;

#define STRIDE 8

int vertexAO(int side1,int side2,int corner) {
	if (side1 == 1 && side2 == 1) {
		return 3;
	};
	return ((side1 == 1) + (side2 == 1) + (corner == 1));
}

void chunk_update(struct Chunk* chunk, struct World* world) {

		ivec3s chunk_pos = chunk->chunk_pos;

		unsigned int* vertex_buffer = malloc(chunk->vert_count* sizeof(unsigned int));

		unsigned int* index_buffer = malloc(chunk->index_count * sizeof(int));

		int index_pos = 0;
		int vertex_pos = 0;
		int index_offset = 0;
		chunk->empty = true;
		chunk_foreach(block_pos) {
			int index = chunk_pos_to_index(block_pos);
			int id = chunk->blocks[index];
			if (id != AIR) {
				chunk->empty = false;
				ivec3s world_pos = block_pos_to_world_pos(block_pos, chunk);

				for (Direction dir = 0; dir < NUM_DIRECTIONS; dir++) {

					int* face_direction = &face_directions[dir];


					ivec3s adjacent_block_pos;
					glm_ivec3_add(block_pos.raw, face_direction, &adjacent_block_pos.raw);

					int adjacent_block_exists = CHUNK_WORLD_BLOCK_EXISTS(chunk, adjacent_block_pos);

					if (adjacent_block_exists == 0) {
						
					
						for (int vert = 0; vert < 4; vert++) {
							const unsigned int* vertex = &CUBE_VERTICES[CUBE_INDICES[(dir * 6) + UNIQUE_INDICES[vert]] * 3];
							ivec2s atlas_coords = block_get_atlas_coord(id, dir);
							vertex_buffer[vertex_pos] = (vertex[0] + x) | ((vertex[1] + y) << 6u) | ((vertex[2] + z) << 12u) | (vert << 18u) | (atlas_coords.x << 20u) | (atlas_coords.y << 24u) | (dir << 28u);
							ivec3s corner = get_corner(vertex, face_direction, block_pos);
							ivec3s side1 = get_side1(vertex, face_direction, block_pos);
							ivec3s side2 = get_side2(vertex, face_direction, block_pos);
							vertex_buffer[vertex_pos + 1] = vertexAO(
								CHUNK_WORLD_BLOCK_EXISTS(chunk, side1),
								CHUNK_WORLD_BLOCK_EXISTS(chunk, side2),
								CHUNK_WORLD_BLOCK_EXISTS(chunk, corner));
							vertex_pos += 2;


						}

						for (int index = 0; index < 6; index++) {
							index_buffer[index_pos] = index_offset + FACE_INDICES[index];
							index_pos++;
						}
						index_offset += 4;
					}
				}
			}
			

	}
	
	
		vbo_buffer(chunk->vbo, vertex_buffer, 0, vertex_pos * sizeof(unsigned int));
		vbo_buffer(chunk->ebo, index_buffer, 0, index_pos * sizeof(int));

		free(vertex_buffer);
		free(index_buffer);
		
		chunk->updating = false;


}

void chunk_destroy(struct Chunk* chunk) {
	vbo_destroy(chunk->vbo);
	vbo_destroy(chunk->ebo);
	vao_destroy(chunk->vao);
}

void print_sizes() {
	printf("size of vertex in memory: %d\nsize of index in memory: %d\n", vertex_buffer_size, index_buffer_size);
}


void chunk_set_block(struct Chunk* chunk, ivec3s position, BlockId block) {
	BlockId last = (chunk->blocks)[chunk_pos_to_index(position)];
	if (block != NULL) {
		(chunk->blocks)[chunk_pos_to_index(position)] = block;


	}
	else {
		(chunk->blocks)[chunk_pos_to_index(position)] = 0;
	}

	if (position.x == CHUNK_SIZE - 1) {
		struct Chunk* adjacent = world_get_chunk_at_chunk_coordinate(state.world, (ivec3s) { .x = chunk->chunk_pos.x + 1, .y = chunk->chunk_pos.y, .z = chunk->chunk_pos.z });
		if (adjacent) flag_chunk_update(adjacent);
	}
	if (position.x == 0) {
		struct Chunk* adjacent = world_get_chunk_at_chunk_coordinate(state.world, (ivec3s) { .x = chunk->chunk_pos.x - 1, .y = chunk->chunk_pos.y, .z = chunk->chunk_pos.z });
		if (adjacent) flag_chunk_update(adjacent);
	}
	if (position.y == CHUNK_SIZE - 1) {
		struct Chunk* adjacent = world_get_chunk_at_chunk_coordinate(state.world, (ivec3s) { .x = chunk->chunk_pos.x, .y = chunk->chunk_pos.y + 1, .z = chunk->chunk_pos.z });
		if (adjacent) flag_chunk_update(adjacent);
	}
	if (position.y == 0) {
		struct Chunk* adjacent = world_get_chunk_at_chunk_coordinate(state.world, (ivec3s) { .x = chunk->chunk_pos.x, .y = chunk->chunk_pos.y - 1, .z = chunk->chunk_pos.z });
		if (adjacent) flag_chunk_update(adjacent);
	}
	if (position.z == CHUNK_SIZE - 1) {
		struct Chunk* adjacent = world_get_chunk_at_chunk_coordinate(state.world, (ivec3s) { .x = chunk->chunk_pos.x, .y = chunk->chunk_pos.y, .z = chunk->chunk_pos.z + 1 });
		if (adjacent) flag_chunk_update(adjacent);
	}
	if (position.z == 0) {
		struct Chunk* adjacent = world_get_chunk_at_chunk_coordinate(state.world, (ivec3s) { .x = chunk->chunk_pos.x, .y = chunk->chunk_pos.y, .z = chunk->chunk_pos.z - 1 });
		if (adjacent) flag_chunk_update(adjacent);
	}
	if(last != block) flag_chunk_update(chunk);
	
}

bool chunk_get_block(struct Chunk* chunk, ivec3s position, BlockId* out_block) {
	if (position.x < 0 | position.x >= CHUNK_SIZE) return false;
	if (position.y < 0 | position.y >= CHUNK_SIZE) return false;
	if (position.z < 0 | position.z >= CHUNK_SIZE) return false;

	if ((chunk->blocks)[chunk_pos_to_index(position)] == 0) return false;

	if (out_block != NULL) {
		*out_block = (chunk->blocks)[chunk_pos_to_index(position)];

	}

	return true;
}


void flag_chunk_update(struct Chunk* chunk) {
	if (!chunk->updating) { //if multiple sources flag this chunk, do not add it multiple times
		chunk->updating = true;
		push_stack(chunk->world->queued_updates, &chunk);
	}
}

void chunk_render(struct Chunk* chunk) {
	if(chunk->updating) return;
	if (chunk->empty) { return; }

	vao_attribute(chunk->vao, chunk->vbo, 0, 1, GL_UNSIGNED_INT, 2 * sizeof(unsigned int), 0);
	vao_attribute(chunk->vao, chunk->ebo, 1, 1, GL_UNSIGNED_INT, 2 * sizeof(unsigned int), sizeof(unsigned int));
	

	vao_bind(chunk->vao);
	vbo_bind(chunk->ebo);

	shader_uniform_ivec3(state.shader, "chunk_pos", chunk->chunk_pos.raw);

 	glDrawElements(GL_TRIANGLES, chunk->index_count, GL_UNSIGNED_INT, 0);
}
