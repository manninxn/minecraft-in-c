#include "texture.h"
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


struct Texture texture_create_from_path(const char* path) {
	GLuint id;
	vec2s size;
	// texture 1
	// ---------
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("error lol");
	}
	stbi_image_free(data);

	struct Texture texture = {
		.handle = id,
		.size = (vec2s) {.x = width, .y = height }
	};

	return texture;
}

void texture_bind(struct Texture texture) {
	glBindTexture(GL_TEXTURE_2D, texture.handle);
}

void texture_delete(struct Texture texture) {
	glDeleteTextures(1, &texture.handle);
}

