#ifndef GLOWUP_TEXTURE_H
#define GLOWUP_TEXTURE_H

#include <string>

class Texture
{
public:
	Texture();
	~Texture();

	void loadFromFile(const std::string &filePath);
	void bind() const;

private:
	unsigned int textureID;
	int width;
	int height;
	int numChannels;
};

#endif
