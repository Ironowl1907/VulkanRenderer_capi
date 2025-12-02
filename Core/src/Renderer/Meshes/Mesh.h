#pragma once

#include "Common/Vertex.h"
#include <string>
#include <vector>
class Mesh {
public:
  Mesh();
  Mesh(const char *path);
  Mesh(const std::vector<Vertex> &vertexes,
       const std::vector<uint32_t> &indexes);

  // Non-copyable
  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;

  // CPU Accesible
  const std::vector<Vertex> &getVertices() { return m_vertices; }
  const std::vector<uint32_t> &getIndices() { return m_indices; }


	void loadFromFile(const char * path);

private:
private:
  std::vector<Vertex> m_vertices;
  std::vector<uint32_t> m_indices;
};
