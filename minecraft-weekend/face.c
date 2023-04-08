#include "face.h"



ivec3s get_corner(ivec3 vertex, ivec3s normal, ivec3s voxel) {
     ivec3s corner = glms_ivec3_add(voxel, normal);
     if (normal.x == 0) corner.x += vertex[0] > 0 ? 1 : -1;
     if (normal.y == 0) corner.y += vertex[1] > 0 ? 1 : -1;
     if (normal.z == 0) corner.z += vertex[2] > 0 ? 1 : -1;

     return corner;
}

ivec3s get_side1(ivec3 vertex, ivec3s normal, ivec3s voxel) {

    ivec3s ortho = glms_ivec3_add(normal, get_ortho_two(normal));
    ivec3s corner = glms_ivec3_add(voxel, normal);
    if (ortho.x == 0) corner.x += vertex[0] > 0 ? 1 : -1;
    if (ortho.y == 0) corner.y += vertex[1] > 0 ? 1 : -1;
    if (ortho.z == 0) corner.z += vertex[2] > 0 ? 1 : -1;

    return corner;
}

ivec3s get_side2(ivec3 vertex, ivec3s normal, ivec3s voxel) {
    ivec3s ortho = glms_ivec3_add(normal, get_ortho_one(normal));
    ivec3s corner = glms_ivec3_add(voxel, normal);
    if (ortho.x == 0) corner.x += vertex[0] > 0 ? 1 : -1;
    if (ortho.y == 0) corner.y += vertex[1] > 0 ? 1 : -1;
    if (ortho.z == 0) corner.z += vertex[2] > 0 ? 1 : -1;

    return corner;
}