#include "block.h"

struct BlockData BLOCKS[INT16_MAX];

ivec2s block_get_atlas_coord(BlockId block, Direction direction) {
	if (block == AIR) return (ivec2s) { .x = 0, .y = 0 };

	return BLOCKS[block].atlas_coordinates[direction];

}