#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGH 600

struct Context {
	GLFWwindow * window;
};

class Renderer {
public:
  void init();
  void shutdown();

  void run();

private:
  void initVulkan();
	void initGLFW();
  void renderLoop();
  void cleanup();
  Context m_context;
};
