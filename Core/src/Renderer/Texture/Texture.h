#pragma once

#include "BufferManager/BufferManager.h"
#include "Commands/CommandManager.h"
#include "Swapchain/Swapchain.h"
#include "vulkan/vulkan_core.h"
#include <string>
class Texture {
public:
  void init(VulkanContext *p_context, CommandManager * p_cmdManager);
  void loadFromFile(VulkanContext *p_context, BufferManager *p_bufferMan,
                    const std::string &path);

  VkImageView getImageView() { return m_textureImageView; }
  VkSampler getSampler() { return m_textureSampler; }

  void cleanup(VulkanContext *p_context);

private:
  void createTextureImage(VulkanContext *p_context,
                          BufferManager *p_bufferManger,
                          const std::string &path);

  void createTextureImageView(VulkanContext *p_context);

  void transitionImageLayout(VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout);

  void createTextureSampler(VulkanContext *p_context);

private:
  VkImage m_textureImage;
  VkDeviceMemory m_textureImageMemory;
  VkImageView m_textureImageView;
  VkSampler m_textureSampler;

  VulkanContext *mp_context;
  CommandManager *mp_cmdManager;
};
