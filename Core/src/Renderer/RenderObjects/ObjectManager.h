#pragma once

#include "BufferManager/BufferManager.h"
#include "RenderObjects/RenderObject.h"
#include "Swapchain/Swapchain.h"
#include "vulkan/vulkan_core.h"
#include <stdexcept>
#include <vector>

struct ObjBufferInfo {
  uint32_t vertexOffset;
  uint32_t vertexCount;
  uint32_t indexOffset;
  uint32_t indexCount;
  int32_t vertexOffsetValue;
};

class ObjManager {
public:
  void init(VulkanContext *p_context, BufferManager *p_bufferManager);
  void shutdown();

  uint32_t addRenderObject(const RenderObject &obj);

  inline const ObjBufferInfo &getObjInfo(uint32_t objId) const {
    if (objId >= m_objInfos.size()) {
      throw std::runtime_error("Invalid mesh ID");
    }
    return m_objInfos[objId];
  }

  inline VkBuffer getVertexBuffer() {
    if (m_needsRebuild)
      rebuildBuffers();
    return m_vertexBuffer;
  }
  inline VkBuffer getIndexBuffer() {
    if (m_needsRebuild)
      rebuildBuffers();
    return m_indexBuffer;
  }

private:
  void createVertexBuffer();
  void createIndexBuffer();
  void destroyBuffers();
  void rebuildBuffers();

private:
  VkBuffer m_vertexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory m_vertexBufferMemory = VK_NULL_HANDLE;
  VkBuffer m_indexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory m_indexBufferMemory = VK_NULL_HANDLE;

  std::vector<RenderObject> m_objects;
  std::vector<ObjBufferInfo> m_objInfos;

  // Aggregated vertex and index data
  std::vector<Vertex> m_allVertices;
  std::vector<uint32_t> m_allIndices;

  bool m_needsRebuild = false;

  VulkanContext *mp_context;
  BufferManager *mp_bufferManager;
};
