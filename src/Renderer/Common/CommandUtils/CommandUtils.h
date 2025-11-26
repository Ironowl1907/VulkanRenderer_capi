#pragma once

#include "Swapchain/Swapchain.h"
#include "vulkan/vulkan_core.h"

[[nodiscard]] VkCommandBuffer beginOneTimeCommands(VulkanContext *p_context,
                                                   VkCommandPool *p_pool);

void endOneTimeCommands(VulkanContext *p_context, VkCommandPool *p_pool,
                        VkCommandBuffer *p_commandBuffer);

class OneTimeSubmit {
public:
  OneTimeSubmit(VulkanContext *p_ctx, VkCommandPool *p_pool)
      : mp_ctx(p_ctx), mp_pool(p_pool) {
    m_cmd = beginOneTimeCommands(mp_ctx, mp_pool);
  }

  ~OneTimeSubmit() { endOneTimeCommands(mp_ctx, mp_pool, &m_cmd); }

  VkCommandBuffer get() const { return m_cmd; }

private:
  VulkanContext *mp_ctx;
  VkCommandPool *mp_pool;
  VkCommandBuffer m_cmd;
};
