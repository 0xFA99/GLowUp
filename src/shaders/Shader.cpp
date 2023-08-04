#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader() : programID(0) {}

Shader::~Shader() { glDeleteProgram(programID); }

void Shader::createFromFiles(const std::string &vertexPath, const std::string &fragmentPath)
{
	std::string vertexCode		= readShaderFile(vertexPath);
	std::string fragmentCode	= readShaderFile(fragmentPath);

	unsigned int vertexShader	= compileShader(GL_VERTEX_SHADER, vertexCode);
	unsigned int fragmentShader	= compileShader(GL_FRAGMENT_SHADER, fragmentCode);

	programID = linkShaders(vertexShader, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::use() const { glUseProgram(programID); }

int Shader::getUniformLocation(const std::string &name)
{
	int uniformLocation = glGetUniformLocation(programID, name.c_str());
	return uniformLocation;
}

std::string Shader::readShaderFile(const std::string &filePath)
{
	std::ifstream shaderFile(filePath);
	if (!shaderFile.is_open()) {
		std::cerr << "Failed to open shader file: " << filePath << std::endl;
		return "";
	}

	std::stringstream shaderStream;
	shaderStream << shaderFile.rdbuf();
	shaderFile.close();

	return shaderStream.str();
}

unsigned int Shader::compileShader(GLenum shaderType, const std::string &shaderCode)
{
	unsigned int shaderID = glCreateShader(shaderType);
	const char *shaderCodeStr = shaderCode.c_str();

	glShaderSource(shaderID, 1, &shaderCodeStr, nullptr);
	glCompileShader(shaderID);

	int success;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
		std::cerr << "Shader compilation error: " << infoLog << std::endl;
	}

	return shaderID;
}

unsigned int Shader::linkShaders(unsigned int vertexShader, unsigned int fragmentShader)
{
	unsigned int program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(program, 512, nullptr, infoLog);
		std::cerr << "Shader program linking error: " << infoLog << std::endl;
	}

	return program;
}
