#include "Texture.h"

#include <GL/glew.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../../vendors/stb_image.h"

Texture::Texture() :
	textureID(0), width(0), height(0), numChannels(0)
{}

Texture::~Texture()
{
	glDeleteTextures(1, &textureID);
}

void Texture::loadFromFile(const std::string &filePath)
{
	stbi_set_flip_vertically_on_load(true);
	unsigned char *imageData = stbi_load(filePath.c_str(), &width, &height, &numChannels, 0);

	if (!imageData) {
		std::cerr << "Failed to load texture from file" << std::endl;
		return;
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum format = (numChannels == 3) ? GL_RGB : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(imageData);
}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, textureID);
}


