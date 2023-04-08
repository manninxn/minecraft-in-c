#include "block.h"

void wood_plank_init() {
	struct BlockData block = {
		.block_type = BLOCKTYPE_SOLID,
		.block_id = WOOD_PLANK
	};

	block.atlas_coordinates[TOP] = (ivec2s){ .x = 5, .y = 0 };
	block.atlas_coordinates[BOTTOM] = (ivec2s){ .x = 5, .y = 0 };

	block.atlas_coordinates[EAST] = (ivec2s){ .x = 5, .y = 0 };
	block.atlas_coordinates[SOUTH] = (ivec2s){ .x = 5, .y = 0 };
	block.atlas_coordinates[NORTH] = (ivec2s){ .x = 5, .y = 0 };
	block.atlas_coordinates[WEST] = (ivec2s){ .x = 5, .y = 0 };

	BLOCKS[WOOD_PLANK] = block;

}