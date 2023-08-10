#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>

#include "shaders/Shader.h"
#include "textures/Texture.h"

void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
void renderScene(Shader &shader, Texture &texture, unsigned int VAO);
void processInput(GLFWwindow *window);

unsigned int SCREEN_WIDTH  = 800;
unsigned int SCREEN_HEIGHT = 600;

int main() {
	if (!glfwInit()) {
		std::cerr << "Failed to Initialize GLFW" << std::endl;
		std::exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello OpenGL", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to Initialize GLFW window" << std::endl;
		glfwTerminate();
		std::exit(-1);
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialized GLEW" << std::endl;
		glfwTerminate();
		std::exit(-1);
	}

	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

	Shader shader = Shader();
	shader.createFromFiles("shaders/basicVertexShader.vert", "shaders/basicFragmentShader.frag");

	Texture texture = Texture();
	texture.loadFromFile("textures/Wood.jpg");

	float vertices[] = {
		// Position				// Color			// Texture
		 0.5f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f, 	1.0f, 1.0f,	// Top Right
		 0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,	// Bottom Right
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,	// Bottom Left
		-0.5f,  0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	0.0f, 1.0f	// Top Left
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position Attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	// Color Attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// glBindBuffer(GL_ARRAY_BUFFER, 0);
	// glBindVertexArray(0);

	shader.use();

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		renderScene(shader, texture, VAO);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void renderScene(Shader &shader, Texture &texture, unsigned int VAO)
{
	glClearColor(0.12, 0.13, 0.14, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	texture.bind();
	shader.use();

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
