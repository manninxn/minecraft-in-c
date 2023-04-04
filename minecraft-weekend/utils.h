#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <malloc.h>
#include <string.h>
/*
load a file's contemts
*/
char* load_file(const char* path);

#define remap_range(value, low1, high1, low2, high2) (low2 + (value - low1) * (high2 - low2) / (high1 - low1))

#endif