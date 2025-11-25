#pragma once

#include "Swapchain/Swapchain.h"
class RenderPass {

public:
  RenderPass();
  ~RenderPass();

  void init(VulkanContext *p_context, Swapchain &swapchain);
  void shutdown();

  VkRenderPass &getRenderPass() { return m_renderPass; }

private:
  void createRenderPass(VulkanContext *p_context);

private:
  Swapchain *mp_swapchain;
  VulkanContext *mp_context;

  VkRenderPass m_renderPass;
};
