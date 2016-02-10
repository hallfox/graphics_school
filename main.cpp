#define GLFW_INCLUDE_GL3
#define GLFW_NO_GLU

#ifdef __APPLE__
#include <OpenGL/gl3ext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <iostream>

static void glfw_error_callback(int error, const char *desc) {
  std::cerr << desc << std::endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

int main(void) {
  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failure to initialize GLFW.\n";
    exit(EXIT_FAILURE);
  }

  // Set some configs for the window
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  glfwSetErrorCallback(glfw_error_callback);

  // Create the main window and set callbacks
  GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr);
  glfwSetKeyCallback(window, key_callback);

  #ifndef __APPLE__
  GLenum err = glewInit();

  if (GLEW_OK != err) {
    std::cerr << "GLEW could not start\n";
  }
  #endif

  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  std::cout << vertexBuffer << std::endl;

  if (!window) {
    glfwTerminate();
    std::cerr << "Failure to initialize window.\n";
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);

  while(!glfwWindowShouldClose(window)) {
    // Render

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for events
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
