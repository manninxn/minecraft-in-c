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

