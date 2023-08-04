#ifndef GLOWUP_SHADER_H
#define GLOWUP_SHADER_H

#include <GL/glew.h>
#include <string>

class Shader
{
public:
	Shader();
	~Shader();

	void createFromFiles(const std::string &vertexPath, const std::string &fragmentPath);
	void use() const;

	int getUniformLocation(const std::string &name);

private:
	unsigned int programID;

	static std::string readShaderFile(const std::string &filePath);
	static unsigned int compileShader(GLenum shaderType, const std::string &shaderCode);
	static unsigned int linkShaders(unsigned int vertexShader, unsigned int fragmentShader);
};

#endif
