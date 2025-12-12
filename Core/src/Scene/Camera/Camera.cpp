#include "Camera.h"
#include "Core/Application.h"
#include <GLFW/glfw3.h>

Camera::Camera() {}

void Camera::init(float fov, float aspect, float nearPlane, float farPlane) {
  m_fov = fov;
  m_aspect = aspect;
  m_near = nearPlane;
  m_far = farPlane;
  m_position = glm::vec3(0.0f, 0.0f, 0.0f);
  m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
  m_viewDirty = true;
  m_projDirty = true;
}

void Camera::setPosition(const glm::vec3 &pos) {
  m_position = pos;
  m_viewDirty = true;
}

void Camera::setAspectRatio(const float aspect) { m_aspect = aspect; }

void Camera::setRotation(const glm::vec3 &rot) {
  m_rotation = rot;
  m_viewDirty = true;
}

void Camera::move(const glm::vec3 &delta) {
  m_position += delta;
  m_viewDirty = true;
}

void Camera::rotate(const glm::vec3 &delta) {
  m_rotation += delta;
  m_viewDirty = true;
}

void Camera::updateView() {
  glm::mat4 rot = glm::mat4(1.0f);
  rot = glm::rotate(rot, m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  rot = glm::rotate(rot, m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
  rot = glm::rotate(rot, m_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

  glm::mat4 trans = glm::translate(glm::mat4(1.0f), -m_position);
  m_view = rot * trans;
  m_viewDirty = false;
}
void Camera::updateProjection() {
  m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
  m_projection[1][1] *= -1; // Vulkan Y correction
  m_projDirty = false;
}

const glm::mat4 &Camera::getViewMatrix() {
  if (m_viewDirty)
    updateView();
  return m_view;
}

const glm::mat4 &Camera::getProjectionMatrix() {
  if (m_projDirty)
    updateProjection();
  return m_projection;
}

const glm::vec3 &Camera::getPosition() { return m_position; }

const glm::vec3 &Camera::getRotation() { return m_rotation; }

void Camera::update(float ts) {
  const float moveSpeed = 5.0f;
  const float rotateSpeed = 2.0f;

  glm::vec3 position = getPosition();
  glm::vec3 rotation = getRotation();

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

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    rotation.x += rotateSpeed * ts;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    rotation.x -= rotateSpeed * ts;
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    rotation.y -= rotateSpeed * ts;
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    rotation.y += rotateSpeed * ts;

  rotation.x = glm::clamp(rotation.x, -glm::half_pi<float>() + 0.01f,
                          glm::half_pi<float>() - 0.01f);

  setPosition(position);
  setRotation(rotation);
}
