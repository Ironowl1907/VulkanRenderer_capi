#include "Texture.h"
#include "Common/CommandUtils/CommandUtils.h"
#include "Common/Images/CreateImage.h"
#include "Swapchain/Swapchain.h"
#include <cassert>
#include <cstring>
#include <stb_image.h>
#include <stdexcept>

void Texture::init(VulkanContext *p_context, CommandManager *p_cmdManager) {
  mp_context = p_context;
  mp_cmdManager = p_cmdManager;
}

void Texture::loadFromFile(VulkanContext *p_context, BufferManager *p_bufferMan,
                           const std::string &path) {
  createTextureImage(p_context, p_bufferMan, path);
  createTextureImageView(p_context);
  createTextureSampler(p_context);
}

void Texture::createDefaultWhite(BufferManager *p_bufferMan) {
  // Create a 1x1 white texture
  const int width = 1;
  const int height = 1;
  unsigned char whitePixel[4] = {255, 255, 255, 255}; // RGBA white

  createTextureFromData(mp_context, p_bufferMan, whitePixel, width, height);
  createTextureImageView(mp_context);
  createTextureSampler(mp_context);
}

void Texture::createTextureFromData(VulkanContext *p_context,
                                    BufferManager *p_bufferMan,
                                    const unsigned char *data, int width,
                                    int height) {
  VkDeviceSize imageSize = width * height * 4;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  p_bufferMan->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            stagingBuffer, stagingBufferMemory);

  void *mappedData;
  vkMapMemory(p_context->getDevice(), stagingBufferMemory, 0, imageSize, 0,
              &mappedData);
  memcpy(mappedData, data, static_cast<size_t>(imageSize));
  vkUnmapMemory(p_context->getDevice(), stagingBufferMemory);

  createImage(p_context, width, height, VK_FORMAT_R8G8B8A8_SRGB,
              VK_IMAGE_TILING_OPTIMAL,
              VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage,
              m_textureImageMemory);

  transitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  p_bufferMan->copyBufferToImage(stagingBuffer, m_textureImage,
                                 static_cast<uint32_t>(width),
                                 static_cast<uint32_t>(height));
  transitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(p_context->getDevice(), stagingBuffer, nullptr);
  vkFreeMemory(p_context->getDevice(), stagingBufferMemory, nullptr);
}

void Texture::createTextureImage(VulkanContext *p_context,
                                 BufferManager *p_bufferManger,
                                 const std::string &path) {
  int texWidth, texHeight, texChannels;
  stbi_uc *pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels,
                              STBI_rgb_alpha);
  VkDeviceSize imageSize = texWidth * texHeight * 4;

  if (!pixels) {
    throw std::runtime_error("failed to load texture image!");
  }

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  p_bufferManger->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                               stagingBuffer, stagingBufferMemory);

  void *data;
  vkMapMemory(p_context->getDevice(), stagingBufferMemory, 0, imageSize, 0,
              &data);
  memcpy(data, pixels, static_cast<size_t>(imageSize));
  vkUnmapMemory(p_context->getDevice(), stagingBufferMemory);

  stbi_image_free(pixels);

  createImage(p_context, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB,
              VK_IMAGE_TILING_OPTIMAL,
              VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage,
              m_textureImageMemory);

  transitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  p_bufferManger->copyBufferToImage(stagingBuffer, m_textureImage,
                                    static_cast<uint32_t>(texWidth),
                                    static_cast<uint32_t>(texHeight));
  transitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(p_context->getDevice(), stagingBuffer, nullptr);
  vkFreeMemory(p_context->getDevice(), stagingBufferMemory, nullptr);
}

void Texture::transitionImageLayout(VkImage image, VkFormat format,
                                    VkImageLayout oldLayout,
                                    VkImageLayout newLayout) {
  OneTimeSubmit submit(mp_cmdManager);

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
      newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    throw std::invalid_argument("unsupported layout transition!");
  }

  vkCmdPipelineBarrier(submit.get(), sourceStage, destinationStage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);
}

void Texture::createTextureImageView(VulkanContext *p_context) {
  m_textureImageView =
      createImageView(p_context, m_textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                      VK_IMAGE_ASPECT_COLOR_BIT);
}

void Texture::createTextureSampler(VulkanContext *p_context) {
  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(p_context->getPhysicalDevice(), &properties);

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  if (vkCreateSampler(p_context->getDevice(), &samplerInfo, nullptr,
                      &m_textureSampler) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler!");
  }
}

void Texture::cleanup() {
  vkDestroySampler(mp_context->getDevice(), m_textureSampler, nullptr);
  vkDestroyImageView(mp_context->getDevice(), m_textureImageView, nullptr);
  vkDestroyImage(mp_context->getDevice(), m_textureImage, nullptr);
  vkFreeMemory(mp_context->getDevice(), m_textureImageMemory, nullptr);
}
