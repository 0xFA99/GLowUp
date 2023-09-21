#include <GLFW/glfw3.h>
#include <iostream>

#define WINDOW_WIDTH	1024
#define WINDOW_HEIGHT	768
#define WINDOW_TITLE	"Create Window"

/* Functions */
static void RenderScene();

/* Variables */
static GLFWwindow *window;

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

    /* Check if the ESC key was pressed or the window was closed */
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
            glfwWindowShouldClose(window)) {
		
		RenderScene();

    }

    glfwTerminate();

    return 0;
}

void RenderScene()
{
	/* Clear the screen */
	glClear(GL_COLOR_BUFFER_BIT);

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
