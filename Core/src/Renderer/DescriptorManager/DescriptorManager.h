#pragma once

#include "Swapchain/Swapchain.h"
#include "Texture/Texture.h"
#include "vulkan/vulkan_core.h"

class DescriptorManager {
public:
  void init(VulkanContext *p_context);

  void createPool(uint32_t framesInFlight);

  std::vector<VkDescriptorSet>
  allocateSets(std::vector<VkDescriptorSetLayout> layouts,
               std::vector<VkBuffer> &uniformBuffers, Texture &texutre,
               uint32_t framesInFlight);

  void update(std::vector<VkDescriptorSet> &sets,
              const std::vector<VkBuffer> &uniformBuffers,
              VkImageView textureView, VkSampler textureSampler);

  void shutdown();

private:
  VulkanContext *mp_context;
  VkDescriptorPool m_pool;
};
