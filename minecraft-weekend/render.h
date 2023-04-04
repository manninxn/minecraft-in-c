#ifndef RENDER_H
#define RENDER_H

#include <cglm/types-struct.h>
#include <string.h>
enum Direction {
	NORTH = 0,
	SOUTH,
	EAST,
	WEST,
	UP,
	DOWN
};

extern const ivec3s DIRECTION_VECTORS[6];

enum Direction ivec3s_to_dir(ivec3s vec);


#endif