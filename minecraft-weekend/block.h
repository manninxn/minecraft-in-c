#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>
#include <cglm/types-struct.h>

typedef char BlockType;

#define BLOCKTYPE_SOLID 0

typedef enum {
	AIR = 0,
	GRASS,
	DIRT,
	STONE,
	IRON_ORE,
	WOOD_PLANK
} BlockId;





struct BlockData {
	BlockId block_id;
	BlockType block_type;
	ivec2s atlas_coordinates[6];
};

extern struct BlockData BLOCKS[INT16_MAX];

typedef enum {
	NORTH = 0,
	SOUTH,
	EAST,
	WEST,
	TOP,
	BOTTOM,
	NUM_DIRECTIONS
} Direction;


ivec2s block_get_atlas_coord(BlockId block, Direction direction);


//https://github.com/jdah/minecraft-weekend/blob/master/src/block/block.h
//basically, it calls a function by the name passed appended to _init
// that ## thing appends them at compile time
// like it is essentially _name + _init();
// to the compiler


#define _BLOCK_DECL(_name)\
    extern void _name##_init();\
    _name##_init();



static inline void block_init() {
	_BLOCK_DECL(air);
	_BLOCK_DECL(grass);
	_BLOCK_DECL(dirt);
	_BLOCK_DECL(stone);
	_BLOCK_DECL(iron_ore);
	_BLOCK_DECL(wood_plank);
}


#endif
