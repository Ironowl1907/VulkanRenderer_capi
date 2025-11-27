#pragma once

#include "Commands/CommandManager.h"
#include "Swapchain/Swapchain.h"
#include "vulkan/vulkan_core.h"
class BufferManager {
public:
  void init(VulkanContext *p_context, CommandManager *p_cmdManager);
  void shutdown();

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &memory);

  void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                         uint32_t height);

private:
  VulkanContext *mp_context;
  CommandManager *mp_cmdManager;
};
