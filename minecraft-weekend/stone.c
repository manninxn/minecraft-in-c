#include "block.h"

void stone_init() {
	struct BlockData block = {
		.block_type = BLOCKTYPE_SOLID,
		.block_id = STONE
	};

	block.atlas_coordinates[TOP] = (ivec2s){ .x = 3, .y = 0 };
	block.atlas_coordinates[BOTTOM] = (ivec2s){ .x = 3, .y = 0 };

	block.atlas_coordinates[EAST] = (ivec2s){ .x = 3, .y = 0 };
	block.atlas_coordinates[SOUTH] = (ivec2s){ .x = 3, .y = 0 };
	block.atlas_coordinates[NORTH] = (ivec2s){ .x = 3, .y = 0 };
	block.atlas_coordinates[WEST] = (ivec2s){ .x = 3, .y = 0 };

	BLOCKS[STONE] = block;

}