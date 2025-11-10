#include "Renderer.h"
#include <GLFW/glfw3.h>

void Renderer::init() {}
void Renderer::shutdown() {}

void Renderer::run() {}

void Renderer::initVulkan() {}

void Renderer::initGLFW() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  m_context.window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGH, "Vulkan Renderer", 0, 0);
}

void Renderer::renderLoop() {}
void Renderer::cleanup() {}
