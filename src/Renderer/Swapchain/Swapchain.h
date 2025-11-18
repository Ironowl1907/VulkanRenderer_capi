#pragma once
#include "Common/SwapchainSupportDetails.h"
#include "Core/VulkanContext.h"
#include "vulkan/vulkan_core.h"
class Swapchain {
public:
  void init(VulkanContext &context);

  void shutdown();

  // Getters
  VkSwapchainKHR getSwapChain() const { return m_swapChain; }

  const std::vector<VkImage> &getSwapChainImages() const {
    return m_swapChainImages;
  }
  VkFormat getSwapChainImageFormat() const { return m_swapChainImageFormat; }

  VkExtent2D getSwapChainExtent() const { return m_swapChainExtent; }

  const std::vector<VkImageView> &getSwapChainImageViews() const {
    return m_swapChainImageViews;
  }

  std::vector<VkFramebuffer> &getSwapChainFramebuffers() {
    return m_swapChainFramebuffers;
  }

  VkImageView createImageView(VkImage image, VkFormat format,
                              VkImageAspectFlags aspectFlags);

  void recreateSwapChain(VkRenderPass &renderPass);

  void createImage(uint32_t width, uint32_t height, VkFormat format,
                   VkImageTiling tiling, VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkImage &image,
                   VkDeviceMemory &imageMemory);

  VkFormat findDepthFormat(VulkanContext &context);

  void createSwapChain();

  void createImageViews();

  void createDepthResources();

  void createFramebuffers(VkRenderPass &renderPass);

private:
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);

  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  void cleanupSwapChain();

  VkFormat findSupportedFormat(VulkanContext &context,
                               const std::vector<VkFormat> &candidates,
                               VkImageTiling tiling,
                               VkFormatFeatureFlags features);

private:
  VulkanContext *mp_context = nullptr;

  VkSwapchainKHR m_swapChain;

  std::vector<VkImage> m_swapChainImages;

  VkFormat m_swapChainImageFormat;

  VkExtent2D m_swapChainExtent;

  std::vector<VkImageView> m_swapChainImageViews;

  std::vector<VkFramebuffer> m_swapChainFramebuffers;

  VkImage m_depthImage;

  VkDeviceMemory m_depthImageMemory;

  VkImageView m_depthImageView;
};
