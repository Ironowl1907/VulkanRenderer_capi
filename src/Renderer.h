#pragma once

#include <GLFW/glfw3.h>

#include <volk/volk.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

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
