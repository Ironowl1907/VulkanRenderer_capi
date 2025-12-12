#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  Camera();
  void init(float fov, float aspect, float nearPlane, float farPlane);
  void update(float ts);

  void setPosition(const glm::vec3 &pos);
  void setRotation(const glm::vec3 &rot); // pitch/yaw/roll in radians
  void setAspectRatio(const float aspect);

  void move(const glm::vec3 &delta);
  void rotate(const glm::vec3 &delta);

  const glm::mat4 &getViewMatrix();
  const glm::mat4 &getProjectionMatrix();

  const glm::vec3 &getPosition();
  const glm::vec3 &getRotation();

private:
  void updateView();
  void updateProjection();

  glm::vec3 m_position;
  glm::vec3 m_rotation; // pitch, yaw, roll

  glm::mat4 m_view;
  glm::mat4 m_projection;

  float m_fov;
  float m_aspect;
  float m_near;
  float m_far;

  bool m_viewDirty = true;
  bool m_projDirty = true;
};
