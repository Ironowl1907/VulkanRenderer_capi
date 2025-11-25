#pragma once

#include "Swapchain/Swapchain.h"
#include "vulkan/vulkan_core.h"
class BufferManager {
public:
  void init(VulkanContext *p_context, VkCommandPool *p_pool);
  void shutdown();

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &memory);

  void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

  [[nodiscard]] VkCommandBuffer beginOneTimeCommands();
  void endOneTimeCommands(VkCommandBuffer);

private:
  VulkanContext *mp_context;
  VkCommandPool *mp_pool;
};
