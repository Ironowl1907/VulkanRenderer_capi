#pragma once

#include "Common/Vertex.h"
#pragma once
#include <string>
#include <vector>

struct Material {
  std::string name;
  glm::vec3 ambient{1.0f, 1.0f, 1.0f};  // Ka
  glm::vec3 diffuse{1.0f, 1.0f, 1.0f};  // Kd
  glm::vec3 specular{0.0f, 0.0f, 0.0f}; // Ks
  glm::vec3 emission{0.0f, 0.0f, 0.0f}; // Ke
  float shininess{0.0f};                // Ns
  float dissolve{1.0f};                 // d (alpha/opacity)
  float ior{1.0f};                      // Ni (index of refraction)

  std::string diffuseTexture;  // map_Kd
  std::string specularTexture; // map_Ks
  std::string normalTexture;   // map_Bump or bump
  std::string ambientTexture;  // map_Ka
};

class RenderObject {
public:
  RenderObject(){}

  RenderObject(const std::vector<Vertex> &vertexes,
               const std::vector<uint32_t> &indexes){m_vertices =vertexes; m_indices = indexes;}
  // Non-copyable
  RenderObject(const RenderObject &) = delete;
  RenderObject &operator=(const RenderObject &) = delete;

  // CPU Accessible
  const std::vector<Vertex> &getVertices() { return m_vertices; }
  const std::vector<uint32_t> &getIndices() { return m_indices; }
  const std::vector<Material> &getMaterials() { return m_materials; }
  const std::vector<int> &getMaterialIndices() { return m_materialIndices; }

  void setVertexes(const std::vector<Vertex> &vertexes) {
    m_vertices = vertexes;
  }
  void setIndices(const std::vector<uint32_t> &indices) { m_indices = indices; }

protected:
  std::vector<Vertex> m_vertices;
  std::vector<uint32_t> m_indices;
  std::vector<Material> m_materials;
  std::vector<int> m_materialIndices;
};
