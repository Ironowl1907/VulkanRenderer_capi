#include "Camera.h"

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
