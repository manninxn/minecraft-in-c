#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <cglm/types-struct.h>

/*
load a file's contemts
*/
char* load_file(const char* path);

ivec3s get_ortho_one(ivec3 normal);

ivec3s get_ortho_two(ivec3 normal);


#define remap_range(value, low1, high1, low2, high2) (low2 + (value - low1) * (high2 - low2) / (high1 - low1))

#endif