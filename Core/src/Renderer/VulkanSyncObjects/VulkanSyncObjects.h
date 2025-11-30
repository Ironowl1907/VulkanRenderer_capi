#pragma once

#include "Swapchain/Swapchain.h"
#include <cstdint>
class VulkanSyncManager {
public:
  void init(VulkanContext *p_context, uint32_t framesInFlight,
            uint32_t swapChainImages);
  void cleanup();

  VkFence getFrameFence();
  VkSemaphore getAcquireSemaphore();
  VkSemaphore getSubmitSemaphore(uint32_t imageIndex);

  uint32_t getFlightFrameIndex() { return m_inFlightCurrentFrame; }
  void nextFlightFrame();

private:
  std::vector<VkSemaphore> m_submitSemaphores;
  std::vector<VkSemaphore> m_adquiredSemaphore;
  std::vector<VkFence> m_frameFences;

  uint32_t m_inFlightCurrentFrame = 0;
  uint32_t m_framesInFlight = 0;
  uint32_t m_swapChainImagesSize;

  VulkanContext *mp_context;
};
