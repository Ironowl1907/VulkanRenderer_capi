#pragma once
#include "BufferManager/BufferManager.h"
#include "Swapchain/Swapchain.h"
#include <vulkan/vulkan.h>

class UBOManager {
public:
  UBOManager() {}
  UBOManager(VulkanContext *p_context, BufferManager *p_bufManager,
             VkDeviceSize size);
  void shutdown();

  void writeData(const void *data);

  VkBuffer getBuffer() const { return m_buffer; }

private:
  void createBuffer();
  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);

private:
  VulkanContext *mp_context;
  VkBuffer m_buffer;
  VkDeviceMemory m_memory;
  VkDeviceSize m_size;
  void *m_mapped = nullptr;

  BufferManager *mp_bufManager;
};
