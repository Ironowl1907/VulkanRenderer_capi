#include "Mesh.h"
#include "RenderObjects/RenderObject.h"
#include <filesystem>
#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

Mesh::Mesh() {}
Mesh::Mesh(const char *path) { loadFromFile(path); }
Mesh::Mesh(const std::vector<Vertex> &vertexes,
           const std::vector<uint32_t> &indexes) {
  RenderObject(vertexes, indexes);
}

void Mesh::loadFromFile(const char *path) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;
  std::string warn;

  // Extract the directory path for material file loading
  std::filesystem::path objPath(path);
  std::string mtlBaseDir = objPath.parent_path().string();
  if (!mtlBaseDir.empty()) {
    mtlBaseDir += "/";
  }

  // Load OBJ with materials
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path,
                        mtlBaseDir.c_str())) {
    throw std::runtime_error(err);
  }

  if (!err.empty()) {
    std::cout << "Error loading model: " << err << '\n';
  }
  if (!warn.empty()) {
    std::cout << "Warning loading model: " << warn << '\n';
  }

  // Load materials
  std::cout << "Loaded " << materials.size() << " materials\n";
  for (const auto &mat : materials) {
    Material material;
    material.name = mat.name;

    // Load material properties
    material.ambient =
        glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
    material.diffuse =
        glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
    material.specular =
        glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]);
    material.emission =
        glm::vec3(mat.emission[0], mat.emission[1], mat.emission[2]);

    material.shininess = mat.shininess;
    material.dissolve = mat.dissolve;
    material.ior = mat.ior;

    // Load texture paths
    material.diffuseTexture = mat.diffuse_texname;
    material.specularTexture = mat.specular_texname;
    material.normalTexture =
        mat.bump_texname.empty() ? mat.normal_texname : mat.bump_texname;
    material.ambientTexture = mat.ambient_texname;

    m_materials.push_back(material);

    // Debug output
    std::cout << "Material: " << material.name << "\n";
    std::cout << "  Diffuse: (" << material.diffuse.x << ", "
              << material.diffuse.y << ", " << material.diffuse.z << ")\n";
    if (!material.diffuseTexture.empty()) {
      std::cout << "  Diffuse Texture: " << material.diffuseTexture << "\n";
    }
  }

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};

  for (const auto &shape : shapes) {
    size_t index_offset = 0;

    // Iterate over faces
    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
      size_t fv = shape.mesh.num_face_vertices[f];

      // Get material ID for this face
      int material_id = shape.mesh.material_ids[f];

      // Process each vertex in the face
      for (size_t v = 0; v < fv; v++) {
        tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

        Vertex vertex{};
        vertex.pos = {attrib.vertices[3 * idx.vertex_index + 0],
                      attrib.vertices[3 * idx.vertex_index + 1],
                      attrib.vertices[3 * idx.vertex_index + 2]};

        // Load texture coordinates
        if (idx.texcoord_index >= 0) {
          vertex.texCoord = {
              attrib.texcoords[2 * idx.texcoord_index + 0],
              1.0f - attrib.texcoords[2 * idx.texcoord_index + 1] // Flip V
          };
        } else {
          vertex.texCoord = {0.0f, 0.0f};
        }

        // Load normals if available
        if (idx.normal_index >= 0) {
          vertex.normal = {attrib.normals[3 * idx.normal_index + 0],
                           attrib.normals[3 * idx.normal_index + 1],
                           attrib.normals[3 * idx.normal_index + 2]};
        }

        // Apply material color if no texture
        if (material_id >= 0 && material_id < m_materials.size()) {
          vertex.color = m_materials[material_id].diffuse;
        } else {
          vertex.color = {1.0f, 1.0f, 1.0f};
        }

        if (uniqueVertices.count(vertex) == 0) {
          uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
          m_vertices.push_back(vertex);
        }

        m_indices.push_back(uniqueVertices[vertex]);
      }

      // Store material index for this triangle
      m_materialIndices.push_back(material_id);

      index_offset += fv;
    }
  }

  std::cout << "Loaded " << m_vertices.size() << " vertices, "
            << m_indices.size() / 3 << " triangles\n";
}
