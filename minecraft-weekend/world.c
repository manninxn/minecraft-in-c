#include "world.h"
#define FNL_IMPL
#include "FastNoiseLite.h"
#include <windows.h>
#include <time.h>

fnl_state noise, stone_noise, iron_noise, cave_noise;


struct World* world_create() {
	struct World* world = malloc(sizeof(struct World));

world->world_loaded_position = (ivec3s) {0, 0, 0},
	
	world->chunks = malloc(RENDER_DISTANCE * RENDER_DISTANCE * RENDER_DISTANCE * sizeof(struct Chunk*));
	memset(world->chunks, 0, NUM_CHUNKS * sizeof(struct Chunk*));

	world->queued_updates = create_stack(sizeof(struct Chunk*));


	 noise = fnlCreateState();
	noise.noise_type = FNL_NOISE_PERLIN;


	 stone_noise = fnlCreateState();
	stone_noise.noise_type = FNL_NOISE_PERLIN;

	 iron_noise = fnlCreateState();
	iron_noise.noise_type = FNL_NOISE_PERLIN;
	iron_noise.frequency = 0.25;

	 cave_noise = fnlCreateState();
	cave_noise.noise_type = FNL_NOISE_PERLIN;
	cave_noise.frequency = 0.05;

	return world;
}

void world_update(struct World* world) {

	world_load_unloaded_chunks(world, 1);
	clock_t start, end;
	double cpu_time_used;

	start = clock();


	while (world->queued_updates->size > 0) {
		struct Chunk* chunk;
		pop_stack(world->queued_updates, &chunk);
		if (chunk != NULL) {
			
			chunk_update(chunk, world);

			
		}
	}
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("world took %f seconds to update\n", cpu_time_used);


}


void world_render(struct World* world) {
	

	for (int x = 0; x < RENDER_DISTANCE; x++)
		for (int y = 0; y < RENDER_DISTANCE; y++)
			for (int z = 0; z < RENDER_DISTANCE; z++)
			{
				struct Chunk* chunk = world->chunks[chunk_coord_to_index(x, y, z)];
				if (chunk != NULL) {
					chunk_render(world->chunks[chunk_coord_to_index(x, y, z)]);
				}
				
			}



}

void world_set_loaded_position(struct World* world, ivec3s new_pos) {
	int x_offset = world->world_loaded_position.x - new_pos.x;
	int y_offset = world->world_loaded_position.y - new_pos.y;
	int z_offset = world->world_loaded_position.z - new_pos.z;
	world->world_loaded_position.x = new_pos.x;
	world->world_loaded_position.z = new_pos.z;
	bool x_positive, y_positive, z_positive;
	x_positive = x_offset > 0 ? true : false;
	y_positive = y_offset > 0 ? true : false;
	z_positive = z_offset > 0 ? true : false;

	x_offset = abs(x_offset);
	y_offset = abs(y_offset);
	z_offset = abs(z_offset);

	struct Chunk* old[NUM_CHUNKS];
	memcpy(old, world->chunks, NUM_CHUNKS * sizeof(struct Chunk*));

	// Set world to all unloaded chunks initially
	memset(world->chunks, 0, NUM_CHUNKS * sizeof(struct Chunk*));

	for (int i = 0; i < NUM_CHUNKS; i++) {
		struct Chunk* c = old[i];
		if (c == NULL) {
			continue;
		}
		else if (world_chunk_in_bounds(world, c->chunk_pos)) {
			world->chunks[world_chunk_index(world, c->chunk_pos)] = c;
		}
		else {
			chunk_destroy(c);
			free(c);
		}
	}

}

void world_load_unloaded_chunks(struct World* world, int count) {
	int num_loaded = 0;
	int spread = 1;

	for (int chunk_x = 0; chunk_x < RENDER_DISTANCE; chunk_x++)
		for (int chunk_z = 0; chunk_z < RENDER_DISTANCE; chunk_z++) 
			for (int chunk_y = 0; chunk_y < RENDER_DISTANCE; chunk_y++) {

					struct Chunk* chunk = world->chunks[chunk_coord_to_index(chunk_x, chunk_y, chunk_z)];

					if (chunk != NULL) continue;
					num_loaded++;
					if (num_loaded > count) break;
					chunk = chunk_new(world, (ivec3s) {
						.x = chunk_x + world->world_loaded_position.x,
							.y = chunk_y + world->world_loaded_position.y,
							.z = chunk_z + world->world_loaded_position.z
					});





					world->chunks[chunk_coord_to_index(chunk_x, chunk_y, chunk_z)] = chunk;

					int num_blocks = 0;
					int x, y, z;
					x = chunk->chunk_pos.x;
					y = chunk->chunk_pos.y;
					z = chunk->chunk_pos.z;


					for (int xb = 0; xb < CHUNK_SIZE; xb++)
						for (int zb = 0; zb < CHUNK_SIZE; zb++) {

							const int N_OCTAVES = 5;
							float amplitude = 15.0;
							float frequency = 0.5;
							float out1 = 0; // or double value = 0; depending
							for (int i = 0; i < N_OCTAVES; i++) {
								out1 += fnlGetNoise2D(&noise, frequency * (float)(x * CHUNK_SIZE + xb), frequency * (float)(z * CHUNK_SIZE + zb)) * amplitude;
								amplitude *= 0.5;
								frequency *= 2.0;
							}

							int terrain_height = floor(out1) + 64;


							float stone_height = fnlGetNoise2D(&stone_noise, xb + CHUNK_SIZE * x, zb + CHUNK_SIZE * z);
							stone_height = round(remap_range(stone_height, -1.0, 1.0, 2.0, 5.0));

							for (int i = 0; i < CHUNK_SIZE; i++) {
								int world_y = i + CHUNK_SIZE * y;
								if (world_y == terrain_height) {
									chunk->blocks[xb][i][zb] = GRASS;
									num_blocks++;
								}
								else if (world_y < terrain_height) {
									if (world_y < terrain_height - stone_height) {
										chunk->blocks[xb][i][zb] = STONE;
										num_blocks++;

										if (world_y < 55) {
											float iron_ore_noise = fnlGetNoise3D(&iron_noise, xb + CHUNK_SIZE * x, i + CHUNK_SIZE * y, zb + CHUNK_SIZE * z) + 1.0;
											iron_ore_noise *= iron_ore_noise * iron_ore_noise;
											int distance_from_best_level = abs(world_y - 40);
											if (iron_ore_noise > remap_range((float)distance_from_best_level, 0.0, 9.0, 3.15, 4.0)) {
												chunk->blocks[xb][i][zb] = IRON_ORE;
											}
										}

										if (world_y < 60) {
											float cave = fnlGetNoise3D(&cave_noise, xb + CHUNK_SIZE * x, i + CHUNK_SIZE * y, zb + CHUNK_SIZE * z);
											if (cave > 0.5) {
												chunk->blocks[xb][i][zb] = AIR;
												num_blocks--;
											}
										}


									}
									else {
										chunk->blocks[xb][i][zb] = DIRT;
										num_blocks++;
									}




								}
								else {
									chunk->blocks[xb][i][zb] = AIR;
								}




							}

						}
					if (num_blocks > 0) {


						struct Chunk** adjacent_chunks = malloc(sizeof(struct Chunk*) * 6);
						memset(adjacent_chunks, 0, sizeof(struct Chunk*) * 6);
						adjacent_chunks[0] = chunk_x < RENDER_DISTANCE - 1 ? world->chunks[chunk_coord_to_index(chunk_x + 1, chunk_y, chunk_z)] : NULL;
						adjacent_chunks[1] = chunk_x > 0 ? world->chunks[chunk_coord_to_index(chunk_x - 1, chunk_y, chunk_z)] : NULL;
						adjacent_chunks[2] = chunk_y < RENDER_DISTANCE - 1 ? world->chunks[chunk_coord_to_index(chunk_x, chunk_y + 1, chunk_z)] : NULL;
						adjacent_chunks[3] = chunk_y > 0 ? world->chunks[chunk_coord_to_index(chunk_x, chunk_y - 1, chunk_z)] : NULL;
						adjacent_chunks[4] = chunk_z < RENDER_DISTANCE - 1 ? world->chunks[chunk_coord_to_index(chunk_x, chunk_y, chunk_z + 1)] : NULL;
						adjacent_chunks[5] = chunk_z > 0 ? world->chunks[chunk_coord_to_index(chunk_x, chunk_y, chunk_z - 1)] : NULL;

						for (int i = 0; i < 6; i++) {
							struct Chunk* adjacent = adjacent_chunks[i];
							if (adjacent != NULL) {
								flag_chunk_update(adjacent);
							}
						}
						flag_chunk_update(chunk);
					}

				}


	
}

struct Chunk* world_get_chunk_at_chunk_coordinate(struct World* world, ivec3s pos) {
	

	if (world_chunk_in_bounds(world, pos))
 		return world->chunks[world_chunk_index(world, pos)];
	else
		return NULL;
}



int get_block_at_world_pos(struct World* world, ivec3s world_pos, BlockId* block, struct Chunk** containing_chunk) {
	bool does_block_exist;
	ivec3s chunk_pos = (ivec3s){
		.x = floor((float)world_pos.x / (float)CHUNK_SIZE),
		.y = floor((float)world_pos.y / (float)CHUNK_SIZE),
		.z = floor((float)world_pos.z / (float)CHUNK_SIZE),
	};
	
	ivec3s block_pos_in_chunk = world_pos_to_block_pos(world_pos);

	struct Chunk* chunk = world_get_chunk_at_chunk_coordinate(world, chunk_pos);
	if (chunk == NULL) {
		return -1;
	}
	if(containing_chunk != NULL) *containing_chunk = chunk;
	return chunk_get_block(chunk, block_pos_in_chunk, block) ? 1 : 0;
}

