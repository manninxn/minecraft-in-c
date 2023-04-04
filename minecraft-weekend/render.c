#include "render.h"

extern const ivec3s DIRECTION_VECTORS[6] = {
	{{0,0,1}},
	{{0,0,-1}},
	{{1,0,0}},
	{{-1,0,0}},
	{{0,1,0}},
	{{0,-1,0}}
};

enum Direction ivec3s_to_dir(ivec3s vec) {
	for (int i = 0; i < 6; i++) {
		if (!memcmp(&(DIRECTION_VECTORS[i]), &vec, sizeof(ivec3s))) {
			return i;
		}
	}
	return -1;
}
