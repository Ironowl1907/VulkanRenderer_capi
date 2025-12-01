#include "Camera.h"

Camera::Camera(float fov, float aspect, float nearPlane, float farPlane)
    : m_fov(fov), m_aspect(aspect), m_near(nearPlane), m_far(farPlane) {
  updateProjection();
}

void Camera::setPosition(const glm::vec3 &pos) {
  m_position = pos;
  m_viewDirty = true;
}

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
  // glm::mat4 rot = glm::yawPitchRoll(m_rotation.y, m_rotation.x, m_rotation.z);
  // glm::mat4 trans = glm::translate(glm::mat4(1.0f), -m_position);
  // m_view = rot * trans;
  // m_viewDirty = false;
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
