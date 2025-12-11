#include "ObjectManager.h"
#include "BufferManager/BufferManager.h"
#include "Swapchain/Swapchain.h"
#include <cstdint>
#include <cstring>
#include <print>

void ObjManager::init(VulkanContext *p_context,
                      BufferManager *p_bufferManager) {
  mp_bufferManager = p_bufferManager;
  mp_context = p_context;
}
void ObjManager::shutdown() {}

uint32_t ObjManager::addRenderObject(const RenderObject &obj) {
  uint32_t objId = m_objects.size();
  m_objects.push_back(obj);

  ObjBufferInfo objInfo{};
  objInfo.indexCount = obj.getIndices().size();
  objInfo.indexOffset = m_allIndices.size();
  objInfo.vertexCount = obj.getVertices().size();
  objInfo.vertexOffset = m_allVertices.size();
  objInfo.vertexOffsetValue = objInfo.vertexOffset;

  m_objInfos.push_back(objInfo);

  // Append to aggregated data
  m_allVertices.insert(m_allVertices.end(), obj.getVertices().begin(),
                       obj.getVertices().end());
  m_allIndices.insert(m_allIndices.end(), obj.getIndices().begin(),
                      obj.getIndices().end());

  m_needsRebuild = true;

  return objId;
}

void ObjManager::createVertexBuffer() {
  std::println("Creating vertex Buffer");
  VkDeviceSize bufferSize = sizeof(m_allVertices[0]) * m_allVertices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  mp_bufferManager->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                 stagingBuffer, stagingBufferMemory);

  void *data;
  vkMapMemory(mp_context->getDevice(), stagingBufferMemory, 0, bufferSize, 0,
              &data);
  memcpy(data, m_allVertices.data(), (size_t)bufferSize);
  vkUnmapMemory(mp_context->getDevice(), stagingBufferMemory);

  mp_bufferManager->createBuffer(bufferSize,
                                 VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                 m_vertexBuffer, m_vertexBufferMemory);

  mp_bufferManager->copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

  vkDestroyBuffer(mp_context->getDevice(), stagingBuffer, nullptr);
  vkFreeMemory(mp_context->getDevice(), stagingBufferMemory, nullptr);
}
void ObjManager::createIndexBuffer() {
  std::println("Creating index Buffer");
  VkDeviceSize bufferSize = sizeof(m_allIndices[0]) * m_allIndices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  mp_bufferManager->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                 stagingBuffer, stagingBufferMemory);

  void *data;
  vkMapMemory(mp_context->getDevice(), stagingBufferMemory, 0, bufferSize, 0,
              &data);
  memcpy(data, m_allIndices.data(), (size_t)bufferSize);
  vkUnmapMemory(mp_context->getDevice(), stagingBufferMemory);

  mp_bufferManager->createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory);

  mp_bufferManager->copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

  vkDestroyBuffer(mp_context->getDevice(), stagingBuffer, nullptr);
  vkFreeMemory(mp_context->getDevice(), stagingBufferMemory, nullptr);
}

void ObjManager::destroyBuffers() {
  if (m_indexBuffer != VK_NULL_HANDLE) {
    vkDestroyBuffer(mp_context->getDevice(), m_indexBuffer, nullptr);
  }
  if (m_indexBufferMemory != VK_NULL_HANDLE) {
    vkFreeMemory(mp_context->getDevice(), m_indexBufferMemory, nullptr);
  }

  if (m_vertexBuffer != VK_NULL_HANDLE) {
    vkDestroyBuffer(mp_context->getDevice(), m_vertexBuffer, nullptr);
  }
  if (m_vertexBufferMemory != VK_NULL_HANDLE) {
    vkFreeMemory(mp_context->getDevice(), m_vertexBufferMemory, nullptr);
  }
}

void ObjManager::rebuildBuffers() {
  if (!m_needsRebuild)
    return;

  destroyBuffers();

  if (!m_allVertices.empty()) {
    createVertexBuffer();
  }
  if (!m_allIndices.empty()) {
    createIndexBuffer();
  }

  m_needsRebuild = false;
}
