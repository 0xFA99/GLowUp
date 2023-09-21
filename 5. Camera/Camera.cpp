#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define WINDOW_WIDTH    1024
#define WINDOW_HEIGHT   768

static void renderScene();
static void processInput(GLFWwindow *window);
static void mouseCallback(GLFWwindow *window, double xPos, double yPos);

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

namespace CoordinateSystem {
    static glm::mat4 ModelMatrix(glm::vec3 position, float angle, glm::vec3 rotationAxis, glm::vec3 const &scaling);
    static glm::mat4 ViewMatrix(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
    static glm::mat4 PerspectiveProjectionMatrix(float FoVY, float aspectRatio, float nearZ, float farZ);
    static glm::mat4 OrthographicProjectionMatrix(float left, float right, float bottom, float top, float near, float far);
}

glm::mat4 CameraMatrix(glm::vec3 const &position, glm::vec3 const &target, glm::vec3 const &up);

GLFWwindow *window;

const char *vertexShaderPath = "vertexShader.glsl";
const char *fragmentShaderPath = "fragmentShader.glsl";

GLuint shaderProgram;
GLuint shaderModelMatrixLocation;
GLuint shaderViewMatrixLocation;
GLuint shaderProjectionMatrixLocation;

GLuint VAO;
GLuint VBO;
GLuint IBO;

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;

struct Vertex {
    glm::vec3 position;
    uint32_t padding;
    glm::vec3 color;
};

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        std::exit(-1);
    }

    // Setup GLFW context OpenGL
    // Enable 4x MSAA
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Setting OpenGL version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Uses the more modern OpenGL(3.0 or above) and doesn't use the old
    // functionality
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Uses only modern OpenGL APIs and does not rely on legacy features which
    // may not be available on all hardware
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creating Window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Triangle", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to Create GLFW window" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }

    // set OpenGL context to window. This means that all subsequent OpenGL
    // operations perform will impact this window
    glfwMakeContextCurrent(window);

    // setting GLFW manages keyboard key input
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetCursorPosCallback(window, mouseCallback);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }

    glEnable(GL_DEPTH_TEST);

	// Define an array of vertices for a simple triangle
    Vertex vertices[] = {
            // Position - pos 0       Padding      Color - pos 1
            // Front 
		{ { -0.5f, -0.5f,  0.5f },	0,		{ 1.0f, 0.0f, 0.0f } },  // Bottom-left
   		{ {  0.5f, -0.5f,  0.5f },	0,		{ 0.0f, 1.0f, 0.0f } },  // Bottom-right
   		{ {  0.5f,  0.5f,  0.5f },	0,		{ 0.0f, 0.0f, 1.0f } },  // Top-right
   		{ { -0.5f,  0.5f,  0.5f },	0,		{ 0.0f, 1.0f, 1.0f } },  // Top-left

            // Back face
   		{ { -0.5f, -0.5f, -0.5f },	0,		{ 1.0f, 0.0f, 0.0f } },  // Bottom-left
   		{ {  0.5f, -0.5f, -0.5f },	0,		{ 0.0f, 1.0f, 0.0f } },  // Bottom-right
   		{ {  0.5f,  0.5f, -0.5f },	0,		{ 0.0f, 0.0f, 1.0f } },  // Top-right
   		{ { -0.5f,  0.5f, -0.5f }, 0,		{ 0.0f, 1.0f, 1.0f } }  // Top-left
    };

	GLuint indices[] = {
    // Front face
    0, 1, 2,
    2, 3, 0,

    // Back face
    4, 5, 6,
    6, 7, 4,

    // Left face
    7, 3, 0,
    0, 4, 7,

    // Right face
    1, 5, 6,
    6, 2, 1,

    // Top face
    3, 2, 6,
    6, 7, 3,

    // Bottom face
    0, 1, 5,
    5, 4, 0
};

	// Create and bind a Vertex Array Object (VAO)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

	// Create and bind a Vertex Buffer Object (VBO)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create and bind a Index Buffer Object (IBO)
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Define how vertex attributes are stored in the VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, color));

	// Enable vertex attribute arrays
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

	// Unbind the VAO
    glBindVertexArray(0);

	// Create and compile a shader program vertex shader and fragment shader.
    shaderProgram = Shader::CreateShaderProgram(vertexShaderPath, fragmentShaderPath);
    glUseProgram(shaderProgram);

	// Get the location of the "Translation" uniform variable in the shader.
    shaderModelMatrixLocation = glGetUniformLocation(shaderProgram, "ModelMatrix");
    shaderViewMatrixLocation = glGetUniformLocation(shaderProgram, "ViewMatrix");
    shaderProjectionMatrixLocation = glGetUniformLocation(shaderProgram, "ProjectionMatrix");

    // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
        renderScene();
    }

    // Clean up
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();

    return 0;
}

void renderScene()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	processInput(window);

    glBindVertexArray(VAO);

    glm::mat4 modelMatrix(1.0f);
    modelMatrix = CoordinateSystem::ModelMatrix(glm::vec3(0.0f, 0.f, 2.0f), (float)glfwGetTime() * 60.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f));

    // glm::mat4 viewMatrix(1.0f);
    // viewMatrix = Transformation::Translation(viewMatrix, glm::vec3(0.0f, 0.0f, -2.0f));

    glm::mat4 viewMatrix(1.0f);
    // viewMatrix = CameraMatrix(cameraPos, cameraPos + cameraFront, cameraFront);
	viewMatrix = glm::lookAt(cameraPos, cameraFront, cameraUp);

    glm::mat4 projectionMatrix(1.0f);
    projectionMatrix = CoordinateSystem::PerspectiveProjectionMatrix(90.0f, (float)WINDOW_WIDTH / WINDOW_HEIGHT, 1.0f, 10.f);

    glUniformMatrix4fv(shaderModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(shaderViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(shaderProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // swapping double buffers. This is used when drawing objects or images
    // and can prevent flickering or an uneven appearance
    glfwSwapBuffers(window);

    // examine and process all events that occur in the GLFW window.
    // This includes user input such as keyboard keys pressed, mouse movement
    // and other events
    glfwPollEvents();
}

GLuint Shader::CreateShaderProgram(const std::string &vertexPath, const std::string &fragmentPath)
{
    std::string vertexSource    = Shader::ReadShaderFile(vertexPath);
    std::string fragmentSource  = Shader::ReadShaderFile(fragmentPath);

    GLuint vertexShader     = Shader::CompileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader   = Shader::CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint programShader = Shader::LinkShaders(vertexShader, fragmentShader);

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

glm::mat4 Transformation::Translation(const glm::mat4 &matrix, const glm::vec3 &position)
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
    scalingMatrix[3];

    return scalingMatrix;
}

glm::mat4 CoordinateSystem::ModelMatrix(glm::vec3 position, float angle, glm::vec3 rotationAxis, glm::vec3 const &scaling)
{
    glm::mat4 modelMatrix(1.0f);

    modelMatrix = Transformation::Translation(modelMatrix, position);
    modelMatrix = Transformation::Rotation(modelMatrix, angle, rotationAxis);
    modelMatrix = Transformation::Scaling(modelMatrix, scaling);

    return modelMatrix;
}

glm::mat4 CoordinateSystem::ViewMatrix(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp)
{
    glm::vec3 forward = glm::normalize(cameraTarget - cameraPosition);
    glm::vec3 right = glm::normalize(glm::cross(cameraUp, forward));
    glm::vec3 up = glm::cross(forward, right);

    return {
            { right.x, up.x, -forward.x, 0.0f },
            { right.y, up.y, -forward.y, 0.0f },
            { right.z, up.z, -forward.z, 0.0f },
            { -glm::dot(right, cameraPosition), -glm::dot(up, cameraPosition), glm::dot(forward, cameraPosition), 1.0f }
    };
}

glm::mat4 CoordinateSystem::PerspectiveProjectionMatrix(float FoVY, float aspectRatio, float nearZ, float farZ)
{
    float tanHalfFoVy = tanf(glm::radians(FoVY / 2.0f));
    float rangeZ = nearZ - farZ;

    float d = 1 / tanHalfFoVy;
    float A = (-farZ - nearZ) / rangeZ;
    float B = 2.0f * farZ * nearZ / rangeZ;

    glm::mat4 projectionMatrix = {
            { d / aspectRatio, 0.0f, 0.0f, 0.0f },
            { 0.0f, d, 0.0f, 0.0f },
            { 0.0f, 0.0f, A, B },
            { 0.0f, 0.0f, 1.0f, 0.0f }
    };

    return projectionMatrix;
}

glm::mat4 CoordinateSystem::OrthographicProjectionMatrix(float left, float right, float bottom, float top, float near, float far)
{
    const float A = 2.0f / (right - left);
    const float B = 2.0f / (top - bottom);
    const float C = -2.0f / (far - near);
    const float tx = -(right + left) / (right - left);
    const float ty = -(top + bottom) / (top - bottom);
    const float tz = -(far + near) / (far - near);

    return {
            {A, 0.0f, 0.0f, tx},
            {0.0f, B, 0.0f, ty},
            {0.0f, 0.0f, C, tz},
            {0.0f, 0.0f, 0.0f, 1.0f}
    };
}

glm::mat4 CameraMatrix(glm::vec3 const &position, glm::vec3 const &target, glm::vec3 const &up)
{
    glm::vec3 cameraFront = glm::normalize(target - position);
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, up));
    glm::vec3 cameraUpNew = glm::cross(cameraRight, cameraFront);

    glm::mat4 viewMatrix(1.0f);
    viewMatrix[0][0] = cameraRight.x;
    viewMatrix[1][0] = cameraRight.y;
    viewMatrix[2][0] = cameraRight.z;

    viewMatrix[0][1] = cameraUpNew.x;
    viewMatrix[1][1] = cameraUpNew.y;
    viewMatrix[2][1] = cameraUpNew.z;

    viewMatrix[0][2] = -cameraFront.x;
    viewMatrix[1][2] = -cameraFront.y;
    viewMatrix[2][2] = -cameraFront.z;

    viewMatrix[3][0] = -glm::dot(cameraRight, position);
    viewMatrix[3][1] = -glm::dot(cameraUpNew, position);
    viewMatrix[3][2] = glm::dot(cameraFront, position);

    return viewMatrix;
}

void processInput(GLFWwindow *window)
{
    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

static void mouseCallback(GLFWwindow *window, double xPos, double yPos)
{
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;

    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
    direction.y = sinf(glm::radians(pitch));
    direction.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}
