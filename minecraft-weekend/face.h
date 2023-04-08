#ifndef FACE_H
#define FACE_H

#include <cglm/types-struct.h>
#include <cglm/cglm.h>
#include "utils.h"
//https://github.com/jdah/minecraft-weekend/blob/master/src/world/blockmesh.c
static const unsigned int FACE_INDICES[] =
{ 1, 0, 3, 1, 3, 2 };

static const unsigned int UNIQUE_INDICES[] = { 1, 0, 5, 2 };
static const unsigned int CUBE_INDICES[] = {
	4, 5, 6, 4, 6, 7, // north (+z)
	1, 0, 3, 1, 3, 2, // south (-z)
	5, 1, 2, 5, 2, 6, // east (+x)
	0, 4, 7, 0, 7, 3, // west (-x)
	2, 3, 7, 2, 7, 6, // top (+y)
	5, 4, 0, 5, 0, 1, // bottom (-y)
};


static const unsigned int CUBE_VERTICES[] = {
	0, 0, 0,
	1, 0, 0,
	1, 1, 0,
	0, 1, 0,

	0, 0, 1,
	1, 0, 1,
	1, 1, 1,
	0, 1, 1
};


ivec3s get_corner(ivec3 vertex, ivec3s normal, ivec3s voxel);


ivec3s get_side1(ivec3 vertex, ivec3s normal, ivec3s voxel);/* {
    ivec3s side1 = { vertex.x, vertex.y, vertex.z };
    if (normal.x != 0) {
        side1.y += 1;
    }
    else if (normal.y != 0) {
        side1.x += 1;
    }
    else if (normal.z != 0) {
        side1.x += 1;
    }
    return side1;
}
*/
ivec3s get_side2(ivec3 vertex, ivec3s normal, ivec3s voxel);/*{
    ivec3s side2 = { vertex.x, vertex.y, vertex.z };
    if (normal.x != 0) {
        side2.z += 1;
    }
    else if (normal.y != 0) {
        side2.z += 1;
    }
    else if (normal.z != 0) {
        side2.y += 1;
    }
    return side2;
}
*/

#endif