#include "Mesh.h"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

Mesh::Mesh() {}
Mesh::Mesh(const char *path) { loadFromFile(path); }

Mesh::Mesh(const std::vector<Vertex> &vertexes,
           const std::vector<uint32_t> &indexes) {}

void Mesh::loadFromFile(const char *path) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn;
  std::string err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path)) {
    throw std::runtime_error(err);
  }

  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Vertex vertex{};

      // Load position
      vertex.pos = {attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]};

      // Load texture coordinates (check if they exist)
      if (index.texcoord_index >= 0) {
        vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                           attrib.texcoords[2 * index.texcoord_index + 1]};
        std::cout << "tex coords" << '\n';
      } else {
        vertex.texCoord = {0.5f, 0.5f}; // Default texture coordinates
      }

      vertex.color = {1.0f, 1.0f, 1.0f};

      m_vertices.push_back(vertex);
      m_indices.push_back(m_indices.size());
    }
  }
}
