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

#define INDEX_MAX_COUNT (((CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE) / 2) * 6 * 6)

#define VERTEX_MAX_COUNT (((CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE) / 2) * (6 * 8) + 8)


struct Chunk* chunk_new(struct World* world, ivec3s pos) {
	struct Chunk* chunk = malloc(sizeof(struct Chunk));

	chunk->chunk_pos = pos;											

	chunk->index_count = INDEX_MAX_COUNT;
	chunk->vert_count = VERTEX_MAX_COUNT;
	//chunk->blocks = calloc(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, sizeof(BlockId));
	chunk->block_count = 0;
	chunk->world = world;
	chunk->updating = false;
	chunk->empty = true;
	chunk->loaded = false;
	num_chunks++;

	chunk->generation_mutex = CreateMutex(NULL, FALSE, NULL);

	chunk->vertex_buffer = NULL;
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
		WaitForSingleObject(chunk->generation_mutex, INFINITY);


		if (chunk->vertex_buffer != NULL) return;

		ivec3s chunk_pos = chunk->chunk_pos;

		chunk->vertex_buffer = malloc(VERTEX_MAX_COUNT * sizeof(unsigned int));

		chunk->index_buffer = malloc(INDEX_MAX_COUNT * sizeof(int));

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
							chunk->vertex_buffer[vertex_pos] = (vertex[0] + x) | ((vertex[1] + y) << 6u) | ((vertex[2] + z) << 12u) | (vert << 18u) | (atlas_coords.x << 20u) | (atlas_coords.y << 24u) | (dir << 28u);
							ivec3s corner = get_corner(vertex, face_direction, block_pos);
							ivec3s side1 = get_side1(vertex, face_direction, block_pos);
							ivec3s side2 = get_side2(vertex, face_direction, block_pos);
							chunk->	vertex_buffer[vertex_pos + 1] = vertexAO(
								CHUNK_WORLD_BLOCK_EXISTS(chunk, side1),
								CHUNK_WORLD_BLOCK_EXISTS(chunk, side2),
								CHUNK_WORLD_BLOCK_EXISTS(chunk, corner));
							vertex_pos += 2;


						}

						for (int index = 0; index < 6; index++) {
							chunk->index_buffer[index_pos] = index_offset + FACE_INDICES[index];
							index_pos++;
						}
						index_offset += 4;
					}
				}
			}
			

	}
	
	
		chunk->vert_count = vertex_pos;
		chunk->index_count = index_pos;
		chunk->updating = false;

		ReleaseMutex(chunk->generation_mutex);

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
		WaitForSingleObject(chunk->world->stack_mutex, INFINITY);
		push_stack(chunk->world->queued_updates, &chunk);
		ReleaseMutex(chunk->world->stack_mutex);
	}
}

void chunk_render(struct Chunk* chunk) {

	WaitForSingleObject(chunk->generation_mutex, INFINITY);


	if (!chunk->loaded) {
		chunk->vbo = vbo_create(GL_ARRAY_BUFFER, true);
		chunk->ebo = vbo_create(GL_ELEMENT_ARRAY_BUFFER, true);
		chunk->vao = vao_create();
		chunk->loaded = true;
	}

	if (chunk->empty) { return; }

	if (chunk->changed) {

		vbo_buffer(chunk->vbo, chunk->vertex_buffer, 0, chunk->vert_count * sizeof(unsigned int));
		vbo_buffer(chunk->ebo, chunk->index_buffer, 0, chunk->index_count * sizeof(int));

		free(chunk->vertex_buffer);
		free(chunk->index_buffer);

		chunk->vertex_buffer = NULL;

		chunk->changed = false;

	}

	vao_attribute(chunk->vao, chunk->vbo, 0, 1, GL_UNSIGNED_INT, 2 * sizeof(unsigned int), 0);
	vao_attribute(chunk->vao, chunk->ebo, 1, 1, GL_UNSIGNED_INT, 2 * sizeof(unsigned int), sizeof(unsigned int));


	vao_bind(chunk->vao);
	vbo_bind(chunk->ebo);

	shader_uniform_ivec3(state.shader, "chunk_pos", chunk->chunk_pos.raw);

	glDrawElements(GL_TRIANGLES, chunk->index_count, GL_UNSIGNED_INT, 0);

	ReleaseMutex(chunk->generation_mutex);

}
