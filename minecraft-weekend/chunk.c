#include "chunk.h"
#include "world.h"
#include <stdio.h>
#define min(a,b) (a > b ? b : a)
#define max(a,b) (a > b ? a : b)
#include "state.h"
#include "face.h"

int num_chunks = 0;

const ivec3 face_directions[] = {
	{ +0, +0, +1 }, //NORTH
	{ +0, +0, -1 }, //SOUTH
	{ +1, +0, +0 }, //EAST
	{ -1, +0, +0 }, //WEST
	{ +0, +1, +0 }, //TOP
	{ +0, -1, +0 }, //BOTTOM
};


struct Chunk* chunk_new(ivec3s pos) {
	struct Chunk* chunk = malloc(sizeof(struct Chunk));

	chunk->chunk_pos = pos;											
	chunk->vertex_buffer = vbo_create(GL_ARRAY_BUFFER, true);
	chunk->index_buffer = vbo_create(GL_ELEMENT_ARRAY_BUFFER, true);
	chunk->vao = vao_create();
	chunk->index_count = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6 * 6;
	chunk->vert_count = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * (6 * 8) + 8;
	chunk->block_count = 0;
	chunk->update_queued = true;

	num_chunks++;


	return chunk;
}

size_t vertex_buffer_size = 0;
size_t index_buffer_size = 0;

#define STRIDE 8

int vertexAO(bool side1,bool side2,bool corner) {
	if (side1 && side2) {
		return 3;
	};
	return (side1 + side2 + corner);
}

void chunk_update(struct Chunk* chunk, struct World* world) {
	
	if (!chunk->update_queued) { return; }
	chunk->update_queued = false;
	

	ivec3s chunk_pos = chunk->chunk_pos;
	

		unsigned int* vertex_buffer = _malloca(chunk->vert_count* sizeof(unsigned int));

		//
		//
		unsigned int* index_buffer = _malloca(chunk->index_count * sizeof(int));


		int index_pos = 0;
		int vertex_pos = 0;
		int index_offset = 0;

		for (int i = 0; i < CHUNK_SIZE* CHUNK_SIZE* CHUNK_SIZE; i++) {

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


			bool is_north_occluded, is_south_occluded, is_east_occluded, is_west_occluded, is_bottom_occluded, is_top_occluded;
			is_top_occluded = get_block_at_world_pos(world, (ivec3s) { x_world_pos + 0, y_world_pos + 1, z_world_pos + 0 }, NULL, NULL);
			is_bottom_occluded = get_block_at_world_pos(world, (ivec3s) { x_world_pos + 0, y_world_pos - 1, z_world_pos + 0 }, NULL, NULL);
			is_east_occluded = get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos + 0, z_world_pos + 0 }, NULL, NULL);
			is_west_occluded = get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos + 0, z_world_pos + 0 }, NULL, NULL);
			is_north_occluded = get_block_at_world_pos(world, (ivec3s) { x_world_pos + 0, y_world_pos + 0, z_world_pos + 1 }, NULL, NULL);
			is_south_occluded = get_block_at_world_pos(world, (ivec3s) { x_world_pos + 0, y_world_pos + 0, z_world_pos - 1 }, NULL, NULL);


			if (!is_north_occluded) {

				bool ao_blocks[8] = {
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 0, y_world_pos + 1, z_world_pos + 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos + 1, z_world_pos + 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos + 1, z_world_pos + 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 0, y_world_pos - 1, z_world_pos + 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos - 1, z_world_pos + 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos - 1, z_world_pos + 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos + 0, z_world_pos + 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos + 0, z_world_pos + 1 }, NULL, NULL),
				};

				int ao_a = vertexAO(ao_blocks[6], ao_blocks[0], ao_blocks[1]);
				int ao_b = vertexAO(ao_blocks[6], ao_blocks[3], ao_blocks[4]);
				int ao_c = vertexAO(ao_blocks[7], ao_blocks[3], ao_blocks[5]);
				int ao_d = vertexAO(ao_blocks[7], ao_blocks[0], ao_blocks[2]);



				ivec2s atlas_coords = block_get_atlas_coord(id, NORTH);
				unsigned int verts[] = NORTH_VERTS(x, y, z, atlas_coords.x, atlas_coords.y, ao_a, ao_b, ao_c, ao_d);
				int indices[] = INDICES_INWARDS(index_offset);
				index_offset += 4;
				memcpy(vertex_buffer + vertex_pos, verts, sizeof(unsigned int) * STRIDE);
				memcpy(index_buffer + index_pos, indices, sizeof(int) * 6);

				vertex_pos += STRIDE;
				index_pos += 6;

			}

			if (!is_south_occluded) {

				bool ao_blocks[8] = {
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 0, y_world_pos + 1, z_world_pos - 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos + 1, z_world_pos - 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos + 1, z_world_pos - 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 0, y_world_pos - 1, z_world_pos - 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos - 1, z_world_pos - 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos - 1, z_world_pos - 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos + 0, z_world_pos - 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos + 0, z_world_pos - 1 }, NULL, NULL),
				};

				int ao_a = vertexAO(ao_blocks[6], ao_blocks[0], ao_blocks[1]);
				int ao_b = vertexAO(ao_blocks[6], ao_blocks[3], ao_blocks[4]);
				int ao_c = vertexAO(ao_blocks[7], ao_blocks[3], ao_blocks[5]);
				int ao_d = vertexAO(ao_blocks[7], ao_blocks[0], ao_blocks[2]);

				ivec2s atlas_coords = block_get_atlas_coord(id, SOUTH);
				unsigned int verts[] = SOUTH_VERTS(x, y, z, atlas_coords.x, atlas_coords.y, ao_a, ao_b, ao_c, ao_d);
				int indices[] = INDICES_OUTWARDS(index_offset);
				index_offset += 4;
				memcpy(vertex_buffer + vertex_pos, verts, sizeof(unsigned int) * STRIDE);
				memcpy(index_buffer + index_pos, indices, sizeof(int) * 6);
				vertex_pos += STRIDE;
				index_pos += 6;

			}

			if (!is_east_occluded) {

				bool ao_blocks[8] = {
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos + 1, z_world_pos + 0 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos + 1, z_world_pos + 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos + 1, z_world_pos - 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos - 1, z_world_pos + 0 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos - 1, z_world_pos + 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos - 1, z_world_pos - 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos + 0, z_world_pos + 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos + 0, z_world_pos - 1 }, NULL, NULL),
				
				};

				int ao_a = vertexAO(ao_blocks[0], ao_blocks[7], ao_blocks[2]);
				int ao_b = vertexAO(ao_blocks[3], ao_blocks[7], ao_blocks[5]);
				int ao_c = vertexAO(ao_blocks[6], ao_blocks[3], ao_blocks[4]);
				int ao_d = vertexAO(ao_blocks[6], ao_blocks[0], ao_blocks[1]);

				ivec2s atlas_coords = block_get_atlas_coord(id, EAST);
				unsigned int verts[] = EAST_VERTS(x, y, z, atlas_coords.x, atlas_coords.y, ao_a, ao_b, ao_c, ao_d);
				int indices[] = INDICES_INWARDS(index_offset);
				index_offset += 4;
				memcpy(vertex_buffer + vertex_pos, verts, sizeof(unsigned int) * STRIDE);
				memcpy(index_buffer + index_pos, indices, sizeof(int) * 6);
				vertex_pos += STRIDE;
				index_pos += 6;

			}

			if (!is_west_occluded) {

				bool ao_blocks[8] = {
					get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos + 1, z_world_pos + 0 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos + 1, z_world_pos + 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos + 1, z_world_pos - 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos - 1, z_world_pos + 0 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos - 1, z_world_pos + 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos - 1, z_world_pos - 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos + 0, z_world_pos + 1 }, NULL, NULL),
					get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos + 0, z_world_pos - 1 }, NULL, NULL),
				};

				int ao_a = vertexAO(ao_blocks[0], ao_blocks[7], ao_blocks[2]);
				int ao_b = vertexAO(ao_blocks[3], ao_blocks[7], ao_blocks[5]);
				int ao_c = vertexAO(ao_blocks[6], ao_blocks[3], ao_blocks[4]);
				int ao_d = vertexAO(ao_blocks[6], ao_blocks[0], ao_blocks[1]);

				ivec2s atlas_coords = block_get_atlas_coord(id, WEST);
				unsigned int verts[] = WEST_VERTS(x, y, z, atlas_coords.x, atlas_coords.y, ao_a, ao_b, ao_c, ao_d);
				int indices[] = INDICES_OUTWARDS(index_offset);
				index_offset += 4;
				memcpy(vertex_buffer + vertex_pos, verts, sizeof(unsigned int) * STRIDE);
				memcpy(index_buffer + index_pos, indices, sizeof(int) * 6);
				vertex_pos += STRIDE;
				index_pos += 6;

			}

			if (!is_bottom_occluded) {
				bool ao_blocks[8] = {
				get_block_at_world_pos(world, (ivec3s) { x_world_pos + 0, y_world_pos - 1, z_world_pos + 1 }, NULL, NULL),
				get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos - 1, z_world_pos + 1 }, NULL, NULL),
				get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos - 1, z_world_pos + 1 }, NULL, NULL),
				get_block_at_world_pos(world, (ivec3s) { x_world_pos + 0, y_world_pos - 1, z_world_pos - 1 }, NULL, NULL),
				get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos - 1, z_world_pos - 1 }, NULL, NULL),
				get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos - 1, z_world_pos - 1 }, NULL, NULL),
				get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos - 1, z_world_pos + 0 }, NULL, NULL),
				get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos - 1, z_world_pos + 0 }, NULL, NULL),
				};

				int ao_a = vertexAO(ao_blocks[6], ao_blocks[0], ao_blocks[1]);

				int ao_b = vertexAO(ao_blocks[6], ao_blocks[3], ao_blocks[4]);

				int ao_c = vertexAO(ao_blocks[7], ao_blocks[3], ao_blocks[5]);

				int ao_d = vertexAO(ao_blocks[7], ao_blocks[0], ao_blocks[2]);

				ivec2s atlas_coords = block_get_atlas_coord(id, BOTTOM);
				unsigned int verts[] = BOTTOM_VERTS(x, y, z, atlas_coords.x, atlas_coords.y, ao_a, ao_b, ao_c, ao_d);
				int indices[] = INDICES_INWARDS(index_offset);
				index_offset += 4;
				memcpy(vertex_buffer + vertex_pos, verts, sizeof(unsigned int) * STRIDE);
				memcpy(index_buffer + index_pos, indices, sizeof(int) * 6);
				vertex_pos += STRIDE;
				index_pos += 6;

			}

			if (!is_top_occluded) {

				bool ao_blocks[8] = {
				get_block_at_world_pos(world, (ivec3s) { x_world_pos + 0, y_world_pos + 1, z_world_pos + 1 }, NULL, NULL),
				get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos + 1, z_world_pos + 1 }, NULL, NULL),
				get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos + 1, z_world_pos + 1 }, NULL, NULL),
				get_block_at_world_pos(world, (ivec3s) { x_world_pos + 0, y_world_pos + 1, z_world_pos - 1 }, NULL, NULL),
				get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos + 1, z_world_pos - 1 }, NULL, NULL),
				get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos + 1, z_world_pos - 1 }, NULL, NULL),
				get_block_at_world_pos(world, (ivec3s) { x_world_pos + 1, y_world_pos + 1, z_world_pos + 0 }, NULL, NULL),
				get_block_at_world_pos(world, (ivec3s) { x_world_pos - 1, y_world_pos + 1, z_world_pos + 0 }, NULL, NULL),
				};

				int ao_a = vertexAO(ao_blocks[6], ao_blocks[0], ao_blocks[1]);
				int ao_b = vertexAO(ao_blocks[6], ao_blocks[3], ao_blocks[4]);
				int ao_c = vertexAO(ao_blocks[7], ao_blocks[3], ao_blocks[5]);
				int ao_d = vertexAO(ao_blocks[7], ao_blocks[0], ao_blocks[2]);

				ivec2s atlas_coords = block_get_atlas_coord(id, TOP);
				unsigned int verts[] = TOP_VERTS(x, y, z, atlas_coords.x, atlas_coords.y, ao_a, ao_b, ao_c, ao_d);
				int indices[] = INDICES_OUTWARDS(index_offset);
				index_offset += 4;
				memcpy(vertex_buffer + vertex_pos, verts, sizeof(unsigned int) * STRIDE);
				memcpy(index_buffer + index_pos, indices, sizeof(int) * 6);
				vertex_pos += STRIDE;
				index_pos += 6;
			}

	}
	
	

		vbo_buffer(chunk->vertex_buffer, vertex_buffer, 0, vertex_pos * sizeof(unsigned int));
		vbo_buffer(chunk->index_buffer, index_buffer, 0, index_pos * sizeof(int));

		_freea(vertex_buffer);
		_freea(index_buffer);
		



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
		if (adjacent) adjacent->update_queued = true;
	}
	if (position.x == 0) {
		struct Chunk* adjacent = world_get_chunk_at_chunk_coordinate(state.world, (ivec3s) { .x = chunk->chunk_pos.x - 1, .y = chunk->chunk_pos.y, .z = chunk->chunk_pos.z });
		if (adjacent) adjacent->update_queued = true;
	}
	if (position.y == CHUNK_SIZE - 1) {
		struct Chunk* adjacent = world_get_chunk_at_chunk_coordinate(state.world, (ivec3s) { .x = chunk->chunk_pos.x, .y = chunk->chunk_pos.y + 1, .z = chunk->chunk_pos.z });
		if (adjacent) adjacent->update_queued = true;
	}
	if (position.y == 0) {
		struct Chunk* adjacent = world_get_chunk_at_chunk_coordinate(state.world, (ivec3s) { .x = chunk->chunk_pos.x, .y = chunk->chunk_pos.y - 1, .z = chunk->chunk_pos.z });
		if (adjacent) adjacent->update_queued = true;
	}
	if (position.z == CHUNK_SIZE - 1) {
		struct Chunk* adjacent = world_get_chunk_at_chunk_coordinate(state.world, (ivec3s) { .x = chunk->chunk_pos.x, .y = chunk->chunk_pos.y, .z = chunk->chunk_pos.z + 1 });
		if (adjacent) adjacent->update_queued = true;
	}
	if (position.z == 0) {
		struct Chunk* adjacent = world_get_chunk_at_chunk_coordinate(state.world, (ivec3s) { .x = chunk->chunk_pos.x, .y = chunk->chunk_pos.y, .z = chunk->chunk_pos.z - 1 });
		if (adjacent) adjacent->update_queued = true;
	}

	chunk->update_queued = true;
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

void chunk_render(struct Chunk* chunk) {

	//if (chunk->empty) { return; }

	vao_attribute(chunk->vao, chunk->vertex_buffer, 0, 1, GL_UNSIGNED_INT, 2 * sizeof(unsigned int), 0);
	vao_attribute(chunk->vao, chunk->vertex_buffer, 1, 1, GL_UNSIGNED_INT, 2 * sizeof(unsigned int), sizeof(unsigned int));
	

	vao_bind(chunk->vao);
	vbo_bind(chunk->index_buffer);

	shader_uniform_ivec3(state.shader, "chunk_pos", chunk->chunk_pos.raw);

 	glDrawElements(GL_TRIANGLES, chunk->index_count, GL_UNSIGNED_INT, 0);
}
