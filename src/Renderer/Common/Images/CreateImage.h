#pragma once

#include "Swapchain/Swapchain.h"
#include "vulkan/vulkan_core.h"

void createImage(VulkanContext *p_context, uint32_t width, uint32_t height,
                 VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                 VkMemoryPropertyFlags properties, VkImage &image,
                 VkDeviceMemory &imageMemory);

VkImageView createImageView(VulkanContext *p_context, VkImage image,
                            VkFormat format, VkImageAspectFlags aspectFlags);
