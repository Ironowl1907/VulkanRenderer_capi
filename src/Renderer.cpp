#include "Renderer.h"
#include "vulkan/vulkan_core.h"
#include <GLFW/glfw3.h>

void Renderer::init() {
  initGLFW();
  initVulkan();
}
void Renderer::shutdown() {}

void Renderer::run() {}

void Renderer::initVulkan() {}

void Renderer::createInstance() {
VkApplicationInfo appInfo {};
		appInfo.sType
}

void Renderer::initGLFW() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  m_context.window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGH, "Vulkan Renderer", 0, 0);
}

void Renderer::renderLoop() {
  while (glfwWindowShouldClose(m_context.window)) {
    glfwPollEvents();
  }
}
void Renderer::cleanup() {
  glfwDestroyWindow(m_context.window);
  glfwTerminate();
}
