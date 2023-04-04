#include "block.h"

void dirt_init() {
	struct BlockData block = {
		.block_type = BLOCKTYPE_SOLID,
		.block_id = DIRT
	};

	block.atlas_coordinates[TOP] = (ivec2s){ .x = 2, .y = 0 };
	block.atlas_coordinates[BOTTOM] = (ivec2s){ .x = 2, .y = 0 };

	block.atlas_coordinates[EAST] = (ivec2s){ .x = 2, .y = 0 };
	block.atlas_coordinates[SOUTH] =(ivec2s){ .x = 2, .y = 0 };
	block.atlas_coordinates[NORTH] =(ivec2s){ .x = 2, .y = 0 };
	block.atlas_coordinates[WEST] = (ivec2s){ .x = 2, .y = 0 };

	BLOCKS[DIRT] = block;

}