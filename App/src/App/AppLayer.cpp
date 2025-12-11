#include "AppLayer.h"

#include "Core/Application.h"
#include "Core/Events/Event.h"
#include "Core/Events/WindowEvents.h"
#include "RenderObjects/Mesh/Mesh.h"
#include "Renderer.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define VERT_SHADER_PATH "../App/Shaders/vert.spv"
#define FRAG_SHADER_PATH "../App/Shaders/frag.spv"

AppLayer::AppLayer() {
  m_renderer.Init(VERT_SHADER_PATH, FRAG_SHADER_PATH);
  Mesh dragonMesh("/home/ironowl/Downloads/dragon/dragon.obj");
  m_dragonMeshId = m_renderer.addObject(dragonMesh);
  m_camera.init(45.0f,
                Core::Application::Get().getFramebufferSize().x /
                    Core::Application::Get().getFramebufferSize().y,
                0.1f, 1000.0f);
}

AppLayer::~AppLayer() { m_renderer.Cleanup(); }

void AppLayer::OnUpdate(float ts) {
  // Movement speed and rotation speed
  const float moveSpeed = 5.0f;
  const float rotateSpeed = 2.0f;

  glm::vec3 position = m_camera.getPosition();
  glm::vec3 rotation = m_camera.getRotation();

  glm::vec3 forward;
  forward.x = cos(rotation.y) * cos(rotation.x);
  forward.y = sin(rotation.x);
  forward.z = sin(rotation.y) * cos(rotation.x);
  forward = glm::normalize(forward);

  glm::vec3 right =
      glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
  glm::vec3 up = glm::normalize(glm::cross(right, forward));

  GLFWwindow *window = Core::Application::Get().getWindow()->getHandle();

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    position += forward * moveSpeed * ts;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    position -= forward * moveSpeed * ts;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    position -= right * moveSpeed * ts;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    position += right * moveSpeed * ts;
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    position -= up * moveSpeed * ts;
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    position += up * moveSpeed * ts;

  // Handle arrow key rotation
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    rotation.x += rotateSpeed * ts;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    rotation.x -= rotateSpeed * ts;
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    rotation.y -= rotateSpeed * ts;
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    rotation.y += rotateSpeed * ts;

  // Optional: Clamp pitch to prevent camera flipping
  rotation.x = glm::clamp(rotation.x, -glm::half_pi<float>() + 0.01f,
                          glm::half_pi<float>() - 0.01f);

  // Update camera
  m_camera.setPosition(position);
  m_camera.setRotation(rotation);
  m_renderer.UpdateUniformBuffer(m_camera);
}

void AppLayer::OnRender() {
  m_renderer.BeginDraw();
  m_renderer.DrawObject(m_dragonMeshId);
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
