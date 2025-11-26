#pragma once

#include "BufferManager/BufferManager.h"
#include "Swapchain/Swapchain.h"
#include "vulkan/vulkan_core.h"
#include <string>
class Texture {
public:
  void loadFromFile(VulkanContext *p_context, BufferManager *p_bufferMan,
                    const std::string &path);

  VkImageView getImageView();
  VkSampler getSampler();

  void shutdown();

private:
private:
  VkImage m_textureImage;
  VkDeviceMemory m_textureImageMemory;
  VkImageView m_textureImageView;
  VkSampler m_textureSampler;
};
