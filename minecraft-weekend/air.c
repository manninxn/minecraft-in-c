#include "block.h"

void air_init() {
	struct BlockData block = {
		.block_type = BLOCKTYPE_SOLID,
		.block_id = AIR
	};

	BLOCKS[AIR] = block;

}