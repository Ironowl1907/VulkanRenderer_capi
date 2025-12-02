#include "UniformBufferManager.h"
#include "BufferManager/BufferManager.h"
#include <cstring>

UBOManager::UBOManager(VulkanContext *p_context, BufferManager *p_bufManager,
                       VkDeviceSize size)
    : mp_context(p_context), mp_bufManager(p_bufManager), m_size(size) {
  createBuffer();
  vkMapMemory(mp_context->getDevice(), m_memory, 0, m_size, 0, &m_mapped);
}

void UBOManager::shutdown() {
  vkUnmapMemory(mp_context->getDevice(), m_memory);
  vkDestroyBuffer(mp_context->getDevice(), m_buffer, nullptr);
  vkFreeMemory(mp_context->getDevice(), m_memory, nullptr);
}

void UBOManager::writeData(const void *data) { memcpy(m_mapped, data, m_size); }

void UBOManager::createBuffer() {
  mp_bufManager->createBuffer(m_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              m_buffer, m_memory);
}
