#pragma once

#include "vulkan/vulkan_core.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGH 600

struct Context {
  GLFWwindow *window;
  VkInstance instance;
};

class Renderer {
public:
  void init();
  void shutdown();

  void run();

private:
  void initVulkan();
  void createInstance();
  void initGLFW();
  void renderLoop();
  void cleanup();
  Context m_context;
};
