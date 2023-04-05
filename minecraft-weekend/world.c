#include "world.h"
#define FNL_IMPL
#include "FastNoiseLite.h"
#include <windows.h>
#include <time.h>
#include <io.h>

fnl_state noise, stone_noise, iron_noise, cave_noise;



struct World* world_create() {
	struct World* world = malloc(sizeof(struct World));

world->world_loaded_position = (ivec3s) {0, 0, 0},
	
	world->chunks = malloc(MAX_CHUNKS * sizeof(struct Chunk*));
	memset(world->chunks, 0, MAX_CHUNKS * sizeof(struct Chunk*));

	world->queued_updates = create_stack(sizeof(struct Chunk*), MAX_CHUNKS);


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

	WaitForSingleObject(world->load_unloaded_mutex, INFINITY);



	clock_t start, end;
	double cpu_time_used, load_unloaded_chunks_time;
	start = clock();
	world_load_unloaded_chunks(world, 2);
	end = clock();
	load_unloaded_chunks_time = ((double)(end - start)) / CLOCKS_PER_SEC;

	start = clock();
	int updates = 0;
	while (world->queued_updates->size > 0) {
		struct Chunk* chunk;
		pop_stack(world->queued_updates, &chunk);
		if (chunk != NULL) {
			
			chunk_update(chunk, world);
			updates += 1;
			
		}
	}
	end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	if (updates > 0) printf("world took %f seconds to load unloaded chunks, %f seconds to update %d chunks (%fs/update)\n", load_unloaded_chunks_time,  cpu_time_used, updates, cpu_time_used / (float)updates);

	ReleaseMutex(world->load_unloaded_mutex);
}


void world_render(struct World* world) {
	WaitForSingleObject(world->load_unloaded_mutex, INFINITY);
	for (int x = 0; x < RENDER_DISTANCE; x++)
		for (int y = 0; y < RENDER_DISTANCE; y++)
			for (int z = 0; z < RENDER_DISTANCE; z++)
			{
				int chunk_index = chunk_coord_to_index(x, y, z);
				struct Chunk* chunk = world->chunks[chunk_index];
				if (chunk != NULL) {
					chunk_render(chunk);
				}

				
			}
	ReleaseMutex(world->load_unloaded_mutex);

}

void world_set_loaded_position(struct World* world, ivec3s new_pos) {
	WaitForSingleObject(world->load_unloaded_mutex, INFINITY);
	int x_offset = world->world_loaded_position.x - new_pos.x;
	int y_offset = world->world_loaded_position.y - new_pos.y;
	int z_offset = world->world_loaded_position.z - new_pos.z;
	world->world_loaded_position.x = new_pos.x;
	world->world_loaded_position.z = new_pos.z;
	world->world_loaded_position.y = new_pos.y;
	bool x_positive, y_positive, z_positive;
	x_positive = x_offset > 0 ? true : false;
	y_positive = y_offset > 0 ? true : false;
	z_positive = z_offset > 0 ? true : false;

	x_offset = abs(x_offset);
	y_offset = abs(y_offset);
	z_offset = abs(z_offset);

	struct Chunk** old = malloc(sizeof(struct Chunk*) * MAX_CHUNKS);
	memcpy(old, world->chunks, MAX_CHUNKS * sizeof(struct Chunk*));

	// Set world to all unloaded chunks initially
	memset(world->chunks, 0, MAX_CHUNKS * sizeof(struct Chunk*));

	for (int i = 0; i < MAX_CHUNKS; i++) {
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

	free(old);

	ReleaseMutex(world->load_unloaded_mutex);

}


DWORD WINAPI generation_worker_thread(LPVOID lpParam) {
	struct ThreadData* data = (struct ThreadData*)lpParam;

	while (!data->shouldTerminate) {
		world_update(data->world);
	}

	return 0;
}

// front-to-back ordering comparator
static int _ftb_cmp(const ivec3s* center, const ivec3s* a, ivec3s* b) {
	return -(glms_ivec3_norm2(glms_ivec3_sub(*center, *b)) - glms_ivec3_norm2(glms_ivec3_sub(*center, *a)));
}

void world_load_unloaded_chunks(struct World* world, int count) {
	int num_loaded = 0;
	int spread = 1;
	ivec3s vectors_sorted[MAX_CHUNKS];
	int num_unloaded = 0;
	for (int chunk_index = 0; chunk_index < MAX_CHUNKS; chunk_index++) {
			struct Chunk* chunk = world->chunks[chunk_index];
			ivec3s pos = world_chunk_offset(world, chunk_index);
			if (chunk != NULL) {
				continue;
			}
			
			vectors_sorted[num_unloaded] = pos;
			num_unloaded++;
	}
	int amt = min(num_unloaded, count);
	if (amt == 0) return;
	ivec3s world_center = glms_ivec3_add(world->world_loaded_position, (ivec3s) {RENDER_DISTANCE / 2, RENDER_DISTANCE / 2, RENDER_DISTANCE / 2});

	qsort_s(vectors_sorted, num_unloaded, sizeof(ivec3s), (int (*)(const void*, const void*, void*)) _ftb_cmp, &world_center);

	//WaitForSingleObject(world->load_unloaded_mutex, INFINITY);

	for(int vec = 0; vec < amt; vec++) {
					ivec3s chunk_pos = vectors_sorted[vec];
					int chunk_index = world_chunk_index(world, chunk_pos);

					struct Chunk* chunk = world->chunks[chunk_index];

					if (chunk != NULL) continue;
				
					chunk = chunk_new(world, chunk_pos);
					

					world->chunks[chunk_index] = chunk;
					int num_blocks = 0;
					int x, y, z;
					x = chunk_pos.x;
					y = chunk_pos.y;
					z = chunk_pos.z;


					for (int xb = 0; xb < CHUNK_SIZE; xb++)
						for (int zb = 0; zb < CHUNK_SIZE; zb++) {
							BlockId block = AIR;
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
									block = GRASS;
									num_blocks++;
								}
								else if (world_y < terrain_height) {
									if (world_y < terrain_height - stone_height) {
										block = STONE;
										num_blocks++;

										if (world_y < 55) {
											float iron_ore_noise = fnlGetNoise3D(&iron_noise, xb + CHUNK_SIZE * x, i + CHUNK_SIZE * y, zb + CHUNK_SIZE * z) + 1.0;
											iron_ore_noise *= iron_ore_noise * iron_ore_noise;
											int distance_from_best_level = abs(world_y - 40);
											if (iron_ore_noise > remap_range((float)distance_from_best_level, 0.0, 9.0, 3.15, 4.0)) {
												block = IRON_ORE;
											}
										}

										if (world_y < 60) {
											float cave = fnlGetNoise3D(&cave_noise, xb + CHUNK_SIZE * x, i + CHUNK_SIZE * y, zb + CHUNK_SIZE * z);
											if (cave > 0.5) {
												block = AIR;
												num_blocks--;
											}
										}


									}
									else {
										block = DIRT;
										num_blocks++;
									}




								}
								else {
									block = AIR;
								}

								chunk_set_block(chunk, (ivec3s) { xb, i, zb }, block);

		
							}

						}

					}

	//ReleaseMutex(world->load_unloaded_mutex);

	
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

