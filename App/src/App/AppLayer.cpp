#include "AppLayer.h"

#include "Core/Application.h"
#include "Core/Events/Event.h"
#include "Core/Events/WindowEvents.h"
#include "RenderObjects/Mesh/Mesh.h"
#include "Renderer.h"

#include <GLFW/glfw3.h>
#include <cstdint>
#include <glm/glm.hpp>

#define VERT_SHADER_PATH "../App/Shaders/vert.spv"
#define FRAG_SHADER_PATH "../App/Shaders/frag.spv"

AppLayer::AppLayer() {
  m_renderer.Init(VERT_SHADER_PATH, FRAG_SHADER_PATH);
  Mesh dragonMesh("/home/ironowl/Downloads/dragon/dragon.obj");
  Mesh spooza("/home/ironowl/Downloads/sponza/sponza.obj");
  m_dragonMeshId = m_renderer.addObject(dragonMesh);
  m_spoozaMeshId = m_renderer.addObject(spooza);
  m_camera.init(45.0f,
                Core::Application::Get().getFramebufferSize().x /
                    Core::Application::Get().getFramebufferSize().y,
                0.001f, 1000.0f);
}

AppLayer::~AppLayer() { m_renderer.Cleanup(); }

void AppLayer::OnUpdate(float ts) {
  m_camera.update(ts);
  m_renderer.UpdateUniformBuffer(m_camera);
}

void AppLayer::OnRender() {
  m_renderer.BeginDraw();
  m_renderer.DrawObject(m_dragonMeshId);
  m_renderer.DrawObject(m_spoozaMeshId);
  m_renderer.EndDraw();
}

void AppLayer::OnEvent(Event &event) {
  EventDispatcher dispatcher(event);
  dispatcher.Dispatch<Core::WindowClosedEvent>(
      [this](Core::WindowClosedEvent &e) { return onWindowClose(e); });

  dispatcher.Dispatch<Core::WindowResizeEvent>(
      [this](Core::WindowResizeEvent &e) { return onWindownResize(e); });
}

bool AppLayer::onWindownResize(Core::WindowResizeEvent &e) {
  m_renderer.OnFrameBufferResize();
  m_camera.setAspectRatio((float)e.GetWidth() / e.GetHeight());
  return false;
}
bool AppLayer::onWindowClose(Event &e) {
  Core::Application::Get().Stop();

  return true;
}
