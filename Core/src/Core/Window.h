#pragma once

#include "vulkan/vulkan_core.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <string>

namespace Core {

struct WindowSpec {
  std::string Title;
  uint32_t Width = 1280;
  uint32_t Height = 720;
  bool IsResizeable = true;
  bool VSync = true;
};

class Window {
public:
  Window(const WindowSpec &specification = WindowSpec());
  ~Window();

  void create();
  void destroy();

  void update();

  glm::vec2 getFramebufferSize();

  bool shouldClose() const;

  GLFWwindow *getHandle() const { return m_Handle; }

private:
  WindowSpec m_specification;

  GLFWwindow *m_Handle = nullptr;
};

} // namespace Core
