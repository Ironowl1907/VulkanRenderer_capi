#pragma once
#include "Common/Vertex.h"
#include "RenderObjects/RenderObject.h"
#include <string>
#include <vector>

class Mesh : public RenderObject {
public:
  Mesh();
  Mesh(const char *path);
  Mesh(const std::vector<Vertex> &vertexes,
       const std::vector<uint32_t> &indexes);
  void loadFromFile(const char *path);
};
