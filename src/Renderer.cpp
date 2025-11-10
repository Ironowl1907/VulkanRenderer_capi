#include "Renderer.h"
#include <GLFW/glfw3.h>
#include <stdexcept>

void Renderer::init() {
  initGLFW();
  initVulkan();
}
void Renderer::shutdown() {}

void Renderer::run() {}

void Renderer::initVulkan() {
  if (volkInitialize()) {
    throw std::runtime_error("Failed to initialize volk.");
  }

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Renderer";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
}

void Renderer::createInstance() { VkApplicationInfo appInfo{}; }

void Renderer::initGLFW() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  m_context.window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan Renderer", 0, 0);
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
