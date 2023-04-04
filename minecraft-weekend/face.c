#include "face.h"



ivec3s get_corner(ivec3 vertex, ivec3 normal, ivec3s voxel) {
     ivec3s corner = { voxel.x + normal[0], voxel.y + normal[1], voxel.z + normal[2] };
     if (normal[0] == 0) corner.x += vertex[0] > 0 ? 1 : -1;
     if (normal[1] == 0) corner.y += vertex[1] > 0 ? 1 : -1;
     if (normal[2] == 0) corner.z += vertex[2] > 0 ? 1 : -1;

     return corner;
}

ivec3s get_side1(ivec3 vertex, ivec3 normal, ivec3s voxel) {

    ivec3s ortho = get_ortho_two(normal);
    glm_ivec3_add(ortho.raw, normal, &ortho.raw);
    ivec3s corner = { voxel.x + normal[0], voxel.y + normal[1], voxel.z + normal[2] };
    if (ortho.x == 0) corner.x += vertex[0] > 0 ? 1 : -1;
    if (ortho.y == 0) corner.y += vertex[1] > 0 ? 1 : -1;
    if (ortho.z == 0) corner.z += vertex[2] > 0 ? 1 : -1;

    return corner;
}

ivec3s get_side2(ivec3 vertex, ivec3 normal, ivec3s voxel) {
    ivec3s ortho = get_ortho_one(normal);
    glm_ivec3_add(ortho.raw, normal, &ortho.raw);
    ivec3s corner = { voxel.x + normal[0], voxel.y + normal[1], voxel.z + normal[2] };
    if (ortho.x == 0) corner.x += vertex[0] > 0 ? 1 : -1;
    if (ortho.y == 0) corner.y += vertex[1] > 0 ? 1 : -1;
    if (ortho.z == 0) corner.z += vertex[2] > 0 ? 1 : -1;

    return corner;
}