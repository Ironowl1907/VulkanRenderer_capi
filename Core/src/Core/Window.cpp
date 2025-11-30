#include "Window.h"

#include <GLFW/glfw3.h>
#include <assert.h>
#include <iostream>

namespace Core {

Window::Window(const WindowSpec &specification)
    : m_specification(specification) {}

Window::~Window() { destroy(); }

void Window::create() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  m_Handle = glfwCreateWindow(m_specification.Width, m_specification.Height,
                              m_specification.Title.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(m_Handle, this);
  // TODO:
  // glfwSetFramebufferSizeCallback(m_Handle, resizecallback);
}

void Window::destroy() {
  if (m_Handle)
    glfwDestroyWindow(m_Handle);

  m_Handle = nullptr;
}

void Window::update() { glfwPollEvents(); }

glm::vec2 Window::getFramebufferSize() {
  // int width, height;
  // glfwGetFramebufferSize(m_Handle, &width, &height);
  // return {width, height};
  // TODO:
  return {0, 0};
}

bool Window::shouldClose() const {
  assert(m_Handle);
  return glfwWindowShouldClose(m_Handle);
}

} // namespace Core
