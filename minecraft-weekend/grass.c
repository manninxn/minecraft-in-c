#include "block.h"

void grass_init() {
	struct BlockData grass = {
		.block_type = BLOCKTYPE_SOLID,
		.block_id = GRASS
	};

	grass.atlas_coordinates[TOP] = (ivec2s){ .x = 1, .y = 0 };
	grass.atlas_coordinates[BOTTOM] = (ivec2s){ .x = 2, .y = 0 };

	grass.atlas_coordinates[EAST] = (ivec2s){ .x = 0, .y = 0 };
	grass.atlas_coordinates[SOUTH] = (ivec2s){ .x = 0, .y = 0 };
	grass.atlas_coordinates[NORTH] = (ivec2s){ .x = 0, .y = 0 };
	grass.atlas_coordinates[WEST] = (ivec2s){ .x = 0, .y = 0 };

	BLOCKS[GRASS] = grass;

}