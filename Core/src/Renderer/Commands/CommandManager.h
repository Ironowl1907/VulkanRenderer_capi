#pragma once

#include "Swapchain/Swapchain.h"
#include "vulkan/vulkan_core.h"
#include <vector>
#include <vulkan/vulkan.h>

class CommandManager {
public:
  void init(VulkanContext *p_context);
	void shutdown() ;
  ~CommandManager() = default;

  void createCommandPools();
  void allocateFrameCommandBuffers(uint32_t framesInFlight);

  // One-time commands
  VkCommandBuffer beginOneTimeCommands();
  void endOneTimeCommands(VkCommandBuffer cmd);

  // Frame-by-frame commands
  VkCommandBuffer getFrameCommandBuffer(uint32_t frameIndex) {
    return m_frameCmdBuffers[frameIndex];
  }

  VkCommandPool *getCommandPool_ptr() { return &m_graphicsPool; }

private:
  VkDevice m_device;

  uint32_t m_graphicsQueueFamily;
  VkQueue m_graphicsQueue;

  VkCommandPool m_graphicsPool;
  std::vector<VkCommandBuffer> m_frameCmdBuffers;

  VulkanContext *mp_context;
};
