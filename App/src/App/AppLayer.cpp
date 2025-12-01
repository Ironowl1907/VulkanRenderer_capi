#include "AppLayer.h"

#include "Core/Application.h"
#include "Core/Events/Event.h"
#include "Core/Events/WindowEvents.h"
#include "Renderer.h"

#include <glm/glm.hpp>
#include <iostream>

#define VERT_SHADER_PATH "../App/Shaders/vert.spv"
#define FRAG_SHADER_PATH "../App/Shaders/frag.spv"

AppLayer::AppLayer() { m_renderer.init(VERT_SHADER_PATH, FRAG_SHADER_PATH); }

AppLayer::~AppLayer() { m_renderer.cleanup(); }

void AppLayer::OnUpdate(float ts) {}

void AppLayer::OnRender() { m_renderer.update(); }

void AppLayer::OnEvent(Event &event) {
  std::cout << event.ToString() << '\n';
  EventDispatcher dispatcher(event);
  dispatcher.Dispatch<Core::WindowClosedEvent>(
      [this](Core::WindowClosedEvent &e) { return onWindowClose(e); });

  dispatcher.Dispatch<Core::WindowResizeEvent>(
      [this](Core::WindowResizeEvent &e) { return onWindownResize(e); });
}

bool AppLayer::onWindownResize(Core::WindowResizeEvent &e) {
  std::cout << "WindowResize: " << e.ToString() << '\n';
	m_renderer.framebufferResizeCallback(e.)
  return false;
}
bool AppLayer::onWindowClose(Event &e) {
	
  std::cout << "WindowClose: " << e.ToString() << '\n';
  return false;
}
