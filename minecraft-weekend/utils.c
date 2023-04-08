#include "utils.h"

char* load_file(const char* fileName)
{
	FILE* fp;
	long size = 0;
	char* shaderContent;

	/* Read File to get size */
	fopen_s(&fp, fileName, "rb");
	if (fp == NULL) {
		return "";
	}
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp) + 1;
	fclose(fp);

	/* Read File for Content */
	fopen_s(&fp, fileName, "r");
	shaderContent = memset(malloc(size), '\0', size);
	fread(shaderContent, 1, size - 1, fp);
	fclose(fp);

	return shaderContent;
}


ivec3s get_ortho_one(ivec3s normal) {
	if (normal.x != 0) {
		return (ivec3s) { .x = 0, .y = 1, .z = 0};
	}
	else if(normal.y != 0) {
		return (ivec3s) { .x = 0, .y = 0, .z = 1 };
	}
	else {
		return (ivec3s) { .x = 1, .y = 0, .z = 0 };
	}
}

ivec3s get_ortho_two(ivec3s normal) {
	if (normal.x != 0) {
		return (ivec3s) { .x = 0, .y = 0, .z = 1 };
	}
	else if (normal.y != 0) {
		return (ivec3s) {
		.x = 1, .y = 0, .z = 0
	};
	}
	else {
		return 	(ivec3s) {
			.x = 0, .y = 1, .z = 0
		};
	}
}
