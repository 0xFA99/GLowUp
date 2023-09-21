#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

#define WINDOW_WIDTH	1024
#define WINDOW_HEIGHT	768
#define WINDOW_TITLE	"Transformation"

/* Functions */
static void RenderScene();

namespace Shader {
    static GLuint CreateShaderProgram(const std::string &vertexPath, const std::string &fragmentPath);
    static GLuint LinkShaders(GLuint vertexShader, GLuint fragmentShader);
    static GLuint CompileShader(GLenum shaderType, const std::string &shaderCode);
    static std::string ReadShaderFile(const std::string &shaderFilePath);
}

namespace Transformation {
    static glm::mat4 Translation(glm::mat4 const &matrix, glm::vec3 const &position);
    static glm::mat4 Rotation(glm::mat4 const &matrix, float angleDegrees, glm::vec3 const &axis);
    static glm::mat4 Scaling(glm::mat4 const &matrix, glm::vec3 const &scale);
}

/* Variables */
static GLFWwindow *window;
static const char vertexShaderPath[] = "vertexShader.glsl";
static const char fragmentShaderPath[] = "fragmentShader.glsl";
static GLuint shaderProgram;
static GLuint transformationLocation;
static GLuint VAO;
static GLuint VBO;

typedef struct {
    glm::vec3 position;
    uint32_t padding;
    glm::vec3 color;
} Vertex;

int main()
{
    /* Initialize GLFW */
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        std::exit(-1);
    }

    /* Enable 4x MSAA */
    glfwWindowHint(GLFW_SAMPLES, 4);

    /* Setting OpenGL version 3.3 */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    /* Uses the more modern OpenGL(3.0 or above) and doesn't use the old
     * functionality
	 */
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /* Uses only modern OpenGL APIs and does not rely on legacy features which
     * may not be available on all hardware
	 */
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Creating Window */
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE,
							  nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to Create GLFW window" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }

    /* set OpenGL context to window. This means that all subsequent OpenGL
     * operations perform will impact this window
	 */
    glfwMakeContextCurrent(window);

    /* setting GLFW manages keyboard key input */
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }

    Vertex vertices[] = {
            /* Position - pos 0     Padding     Color - pos 1 */
            {{-0.5f, -0.5f, 0.0f},  0,          {1.0f, 0.0f, 0.0f}},
            {{ 0.5f, -0.5f, 0.0f},  0,          {0.0f, 1.0f, 0.0f}},
            {{ 0.0f,  0.5f, 0.0f},  0,          {0.0f, 0.0f, 1.0f}}
    };

    // Create and bind a vertex array object (VAO)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create and bind a vertex buffer object (VBO)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Setting up vertex attribute pointers for Position(index 0) and color (index 1)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, color));

    // Enable Position Attribute
    glEnableVertexAttribArray(0);
    // Enable Color Attribute
    glEnableVertexAttribArray(1);

    // Unbind VAO after configuration(optional)
    glBindVertexArray(0);

    // Create and use shader program
    shaderProgram = Shader::CreateShaderProgram(vertexShaderPath, fragmentShaderPath);
    glUseProgram(shaderProgram);

    transformationLocation = glGetUniformLocation(shaderProgram, "Transformation");

    /* Check if the ESC key was pressed or the window was closed */
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
            !glfwWindowShouldClose(window)) {
		
		RenderScene();

    }

    /* Clean up */
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();

    return 0;
}

void RenderScene()
{
	/* Clear the screen */
	glClear(GL_COLOR_BUFFER_BIT);

    /* Binding VAO */
    glBindVertexArray(VAO);

    glm::mat4 transformationMatrix(1.0f);
    transformationMatrix = Transformation::Translation(transformationMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    transformationMatrix = Transformation::Rotation(transformationMatrix, (float)glfwGetTime() * 60.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    glUniformMatrix4fv(transformationLocation, 1, GL_FALSE, glm::value_ptr(transformationMatrix));

    /* Draw Triangle */
    glDrawArrays(GL_TRIANGLES, 0, 3);

	/* swapping double buffers. This is used when drawing objects or images
	 * and can prevent flickering or an uneven appearance
	 */
	glfwSwapBuffers(window);

	/* examine and process all events that occur in the GLFW window.
	 * This includes user input such as keyboard keys pressed, mouse movement
	 * and other events
	 */
	glfwPollEvents();
}

GLuint Shader::CreateShaderProgram(const std::string &vertexPath, const std::string &fragmentPath)
{
    // Read shader source code from files
    std::string vertexSource    = Shader::ReadShaderFile(vertexPath);
    std::string fragmentSource  = Shader::ReadShaderFile(fragmentPath);

    // Compile vertex and fragment shaders
    GLuint vertexShader     = Shader::CompileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader   = Shader::CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    // Link the shaders to create a program
    GLuint programShader = Shader::LinkShaders(vertexShader, fragmentShader);

    // Cleanup the individual shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return programShader;
}

GLuint Shader::LinkShaders(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    // Check if shader linked well
    int success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
    }
    return programID;
}

GLuint Shader::CompileShader(GLenum shaderType, const std::string &shaderCode)
{
    GLuint shaderID = glCreateShader(shaderType);
    const char *shaderCodeStr = shaderCode.c_str();

    glShaderSource(shaderID, 1, &shaderCodeStr, nullptr);
    glCompileShader(shaderID);

    // Check if the shader compiled well
    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
    }

    return shaderID;
}

std::string Shader::ReadShaderFile(const std::string &shaderFilePath)
{
    std::ifstream shaderFile(shaderFilePath);
    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open shader file: " << shaderFilePath << std::endl;
        return "";
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    return shaderStream.str();
}

glm::mat4 Transformation::Translation(glm::mat4 const &matrix, glm::vec3 const &position)
{
    glm::mat4 translationMatrix(1.0f);

    translationMatrix[3] =  matrix[0] * position[0] +
                            matrix[1] * position[1] +
                            matrix[2] * position[2] +
                            matrix[3];

    return translationMatrix;
}

glm::mat4 Transformation::Rotation(glm::mat4 const &matrix, float angleDegrees, glm::vec3 const &axis)
{
    float angleRadians = glm::radians(angleDegrees);
    float cosA = cosf(angleRadians);
    float sinA = sinf(angleRadians);
    glm::vec3 normalizedAxis = glm::normalize(axis);

    float x = normalizedAxis.x;
    float y = normalizedAxis.y;
    float z = normalizedAxis.z;

    float oneMinusCosA = 1.0f - cosA;

    glm::mat4 rotate(1.0f);
    rotate[0][0] = cosA + x * x * oneMinusCosA;
    rotate[0][1] = x * y * oneMinusCosA - z * sinA;
    rotate[0][2] = x * z * oneMinusCosA + y * sinA;

    rotate[1][0] = y * x * oneMinusCosA + z * sinA;
    rotate[1][1] = cosA + y * y * oneMinusCosA;
    rotate[1][2] = y * z * oneMinusCosA - x * sinA;

    rotate[2][0] = z * x * oneMinusCosA - y * sinA;
    rotate[2][1] = z * y * oneMinusCosA + x * sinA;
    rotate[2][2] = cosA + z * z * oneMinusCosA;

    glm::mat4 rotationMatrix(1.0f);
    rotationMatrix[0] = matrix[0] * rotate[0][0] + matrix[1] * rotate[0][1] + matrix[2] * rotate[0][2];
    rotationMatrix[1] = matrix[0] * rotate[1][0] + matrix[1] * rotate[1][1] + matrix[2] * rotate[1][2];
    rotationMatrix[2] = matrix[0] * rotate[2][0] + matrix[1] * rotate[2][1] + matrix[2] * rotate[2][2];
    rotationMatrix[3] = matrix[3];

    return rotationMatrix;
}

glm::mat4 Transformation::Scaling(glm::mat4 const &matrix, glm::vec3 const &scale)
{
    glm::mat4 scalingMatrix(1.0f);

    scalingMatrix[0] = matrix[0] * scale[0];
    scalingMatrix[1] = matrix[1] * scale[1];
    scalingMatrix[2] = matrix[2] * scale[2];
    scalingMatrix[3] = matrix[3];

    return scalingMatrix;
}
