#include "chunk.h"
#include "world.h"
#include <stdio.h>
#define min(a,b) (a > b ? b : a)
#define max(a,b) (a > b ? a : b)
#include "state.h"
#include "face.h"

int num_chunks = 0;

const ivec3 face_directions[6] = {
	{ +0, +0, +1 }, //NORTH
	{ +0, +0, -1 }, //SOUTH
	{ +1, +0, +0 }, //EAST
	{ -1, +0, +0 }, //WEST
	{ +0, +1, +0 }, //TOP
	{ +0, -1, +0 }, //BOTTOM
};


struct Chunk* chunk_new(struct World* world, ivec3s pos) {
	struct Chunk* chunk = malloc(sizeof(struct Chunk));

	chunk->chunk_pos = pos;											
	chunk->vertex_buffer = vbo_create(GL_ARRAY_BUFFER, true);
	chunk->index_buffer = vbo_create(GL_ELEMENT_ARRAY_BUFFER, true);
	chunk->vao = vao_create();
	chunk->index_count = ((CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE) / 2) * 6 * 6;
	chunk->vert_count = ((CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE) / 2) * (6 * 8) + 8;
	chunk->block_count = 0;
	chunk->world = world;
	chunk->updating = false;
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
	
		chunk->updating = true;
		ivec3s chunk_pos = chunk->chunk_pos;

		unsigned int* vertex_buffer = malloc(chunk->vert_count* sizeof(unsigned int));

		unsigned int* index_buffer = malloc(chunk->index_count * sizeof(int));

		int index_pos = 0;
		int vertex_pos = 0;
		int index_offset = 0;

		for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {

			int z = i % CHUNK_SIZE;
			int y = (i / CHUNK_SIZE) % CHUNK_SIZE;
			int x = i / (CHUNK_SIZE * CHUNK_SIZE);

			int id = chunk->blocks[x][y][z];
			if (id == AIR) continue;
			int x_world_pos = x + chunk_pos.x * CHUNK_SIZE;
			int y_world_pos = y + chunk_pos.y * CHUNK_SIZE;
			int z_world_pos = z + chunk_pos.z * CHUNK_SIZE;

			ivec3s world_pos = {
				.x = x_world_pos,
				.y = y_world_pos,
				.z = z_world_pos
			};

			for (Direction dir = 0; dir < NUM_DIRECTIONS; dir++) {

				ivec3 face_direction = {
					face_directions[dir][0],
					face_directions[dir][1],
					face_directions[dir][2],
				};

				ivec3s adjacent_block_pos;
				glm_ivec3_add(world_pos.raw, face_direction, &adjacent_block_pos);

				int adjacent_block_exists = get_block_at_world_pos(world, adjacent_block_pos, NULL, NULL);

				if (adjacent_block_exists != 0) {
					continue;
				}
				for (int vert = 0; vert < 4; vert++) {
					const unsigned int* vertex = &CUBE_VERTICES[CUBE_INDICES[(dir * 6) + UNIQUE_INDICES[vert]] * 3];
					int vert_x, vert_y, vert_z;
					vert_x = vertex[0] + x;
					vert_y = vertex[1] + y;	
					vert_z = vertex[2] + z;
					ivec2s atlas_coords = block_get_atlas_coord(id, dir);
					unsigned int vert_data = (vert_x) | ((vert_y) << 6u) | ((vert_z) << 12u) | (vert << 18u) | (atlas_coords.x << 20u) | (atlas_coords.y << 24u) | (dir << 28u);
					vertex_buffer[vertex_pos] = vert_data;
					ivec3s corner = get_corner(vertex, face_direction, world_pos);
					ivec3s side1 = get_side1(vertex, face_direction, world_pos);
					ivec3s side2 = get_side2(vertex, face_direction, world_pos);
					vertex_buffer[vertex_pos + 1] = vertexAO(
						get_block_at_world_pos(world, side1, NULL, NULL),
						get_block_at_world_pos(world, side2, NULL, NULL),
						get_block_at_world_pos(world, corner, NULL, NULL));
					vertex_pos+=2;


				}

				for (int index = 0; index < 6; index++) {
					index_buffer[index_pos] = index_offset + FACE_INDICES[index];
					index_pos++;
				}
				index_offset += 4;
			}
		
			

	}
	
	
		vbo_buffer(chunk->vertex_buffer, vertex_buffer, 0, vertex_pos * sizeof(unsigned int));
		vbo_buffer(chunk->index_buffer, index_buffer, 0, index_pos * sizeof(int));

		free(vertex_buffer);
		free(index_buffer);
		
		chunk->updating = false;


}

void chunk_destroy(struct Chunk* chunk) {
	vbo_destroy(chunk->vertex_buffer);
	vbo_destroy(chunk->index_buffer);
	vao_destroy(chunk->vao);
}

void print_sizes() {
	printf("size of vertex in memory: %d\nsize of index in memory: %d\n", vertex_buffer_size, index_buffer_size);
}


void chunk_set_block(struct Chunk* chunk, ivec3s position, BlockId block) {
	if (block != NULL) {
		(chunk->blocks)[position.x][position.y][position.z] = block;


	}
	else {
		(chunk->blocks)[position.x][position.y][position.z] = 0;
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

	flag_chunk_update(chunk);
}

bool chunk_get_block(struct Chunk* chunk, ivec3s position, BlockId* out_block) {
	if (position.x < 0 | position.x >= CHUNK_SIZE) return false;
	if (position.y < 0 | position.y >= CHUNK_SIZE) return false;
	if (position.z < 0 | position.z >= CHUNK_SIZE) return false;

	if ((chunk->blocks)[position.x][position.y][position.z] == 0) return false;

	if (out_block != NULL) {
		*out_block = (chunk->blocks)[position.x][position.y][position.z];

	}

	return true;
}


void flag_chunk_update(struct Chunk* chunk) {
	push_stack(chunk->world->queued_updates, &chunk);
}

void chunk_render(struct Chunk* chunk) {
	if(chunk->updating) return;
	//if (chunk->empty) { return; }

	vao_attribute(chunk->vao, chunk->vertex_buffer, 0, 1, GL_UNSIGNED_INT, 2 * sizeof(unsigned int), 0);
	vao_attribute(chunk->vao, chunk->vertex_buffer, 1, 1, GL_UNSIGNED_INT, 2 * sizeof(unsigned int), sizeof(unsigned int));
	

	vao_bind(chunk->vao);
	vbo_bind(chunk->index_buffer);

	shader_uniform_ivec3(state.shader, "chunk_pos", chunk->chunk_pos.raw);

 	glDrawElements(GL_TRIANGLES, chunk->index_count, GL_UNSIGNED_INT, 0);
}
