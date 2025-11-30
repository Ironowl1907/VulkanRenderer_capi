#include "Application.h"
#include "Core/Window.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <assert.h>
#include <iostream>
#include <memory>

namespace Core {

static Application *s_Application = nullptr;

Application::Application(const ApplicationSpec &specification)
    : m_Specification(specification) {
  s_Application = this;

  m_Window = std::make_unique<Window>(specification.Window);
  m_Window->create();
}

Application::~Application() {
  m_Window->destroy();

  s_Application = nullptr;
}

void Application::Run() {
  m_Running = true;

  // TODO :
  // float lastTime = GetTime();

  // Main Application loop
  while (m_Running) {

    m_Window->update();

    if (m_Window->shouldClose()) {
      Stop();
      break;
    }

    for (const std::unique_ptr<Layer> &layer : m_LayerStack)
      layer->OnUpdate(0);

    // NOTE: rendering can be done elsewhere (eg. render thread)
    for (const std::unique_ptr<Layer> &layer : m_LayerStack)
      layer->OnRender();

    m_Window->update();
  }
}

void Application::Stop() { m_Running = false; }

std::shared_ptr<Window> Application::getWindow() { return m_Window; }

glm::vec2 Application::getFramebufferSize() const {
  return m_Window->getFramebufferSize();
}

Application &Application::Get() {
  assert(s_Application);
  return *s_Application;
}

float Application::GetTime() {
  // TODO :
  return 0;
}

} // namespace Core
