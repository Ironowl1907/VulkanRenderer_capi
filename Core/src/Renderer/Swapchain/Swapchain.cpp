#include "Swapchain.h"
#include "Common/Images/CreateImage.h"
#include "Common/MemoryType/MemoryType.h"
#include "Core/Application.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <stdexcept>

void Swapchain::init(VulkanContext *p_context) {
  mp_context = p_context;

  // createSwapChain();
  // createImageViews();
  // createFramebuffers(renderPass);
  // createDepthResources();
}

void Swapchain::shutdown() { cleanupSwapChain(); }

void Swapchain::createImageViews() {
  m_swapChainImageViews.resize(m_swapChainImages.size());

  for (uint32_t i = 0; i < m_swapChainImages.size(); i++) {
    m_swapChainImageViews[i] =
        createImageView(mp_context, m_swapChainImages[i],
                        m_swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
  }
}

void Swapchain::createSwapChain() {
  SwapChainSupportDetails swapChainSupport = querySwapChainSupport(
      mp_context->getPhysicalDevice(), mp_context->getSurface());

  VkSurfaceFormatKHR surfaceFormat =
      chooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode =
      chooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = mp_context->getSurface();

  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices =
      mp_context->findQueueFamilies(mp_context->getPhysicalDevice());
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                   indices.presentFamily.value()};

  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  if (vkCreateSwapchainKHR(mp_context->getDevice(), &createInfo, nullptr,
                           &m_swapChain) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }

  vkGetSwapchainImagesKHR(mp_context->getDevice(), m_swapChain, &imageCount,
                          nullptr);
  m_swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(mp_context->getDevice(), m_swapChain, &imageCount,
                          m_swapChainImages.data());

  m_swapChainImageFormat = surfaceFormat.format;
  m_swapChainExtent = extent;
}

VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR Swapchain::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
Swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(Core::Application::Get().getWindow()->getHandle(),
                           &width, &height);

    VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                               static_cast<uint32_t>(height)};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

void Swapchain::cleanupSwapChain() {
  vkDestroyImageView(mp_context->getDevice(), m_depthImageView, nullptr);
  vkDestroyImage(mp_context->getDevice(), m_depthImage, nullptr);
  vkFreeMemory(mp_context->getDevice(), m_depthImageMemory, nullptr);

  for (auto framebuffer : m_swapChainFramebuffers) {
    vkDestroyFramebuffer(mp_context->getDevice(), framebuffer, nullptr);
  }

  for (auto imageView : m_swapChainImageViews) {
    vkDestroyImageView(mp_context->getDevice(), imageView, nullptr);
  }

  vkDestroySwapchainKHR(mp_context->getDevice(), m_swapChain, nullptr);
}

void Swapchain::recreateSwapChain(VkRenderPass &renderPass) {
  int width = 0, height = 0;
  glm::vec2 size = Core::Application::Get().getFramebufferSize();
  width = size.x;
  height = size.y;
  while (width == 0 || height == 0) {
    glm::vec2 size = Core::Application::Get().getFramebufferSize();
    width = size.x;
    height = size.y;

    glfwWaitEvents();
  }

  vkDeviceWaitIdle(mp_context->getDevice());

  cleanupSwapChain();
  createSwapChain();
  createImageViews();
  createDepthResources();
  createFramebuffers(renderPass);
}

void Swapchain::createDepthResources() {
  VkFormat depthFormat = findDepthFormat(mp_context);

  createImage(mp_context, m_swapChainExtent.width, m_swapChainExtent.height,
              depthFormat, VK_IMAGE_TILING_OPTIMAL,
              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depthImage,
              m_depthImageMemory);
  m_depthImageView = createImageView(mp_context, m_depthImage, depthFormat,
                                     VK_IMAGE_ASPECT_DEPTH_BIT);
}

VkFormat Swapchain::findDepthFormat(VulkanContext *p_context) {
  return findSupportedFormat(
      p_context,
      {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
       VK_FORMAT_D24_UNORM_S8_UINT},
      VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat Swapchain::findSupportedFormat(VulkanContext *p_context,
                                        const std::vector<VkFormat> &candidates,
                                        VkImageTiling tiling,
                                        VkFormatFeatureFlags features) {
  for (VkFormat format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(p_context->getPhysicalDevice(), format,
                                        &props);

    if (tiling == VK_IMAGE_TILING_LINEAR &&
        (props.linearTilingFeatures & features) == features) {
      return format;
    } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
               (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }

  throw std::runtime_error("failed to find supported format!");
}

void Swapchain::createFramebuffers(VkRenderPass &renderPass) {
  m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

  for (size_t i = 0; i < getSwapChainImageViews().size(); i++) {
    std::array<VkImageView, 2> attachments = {getSwapChainImageViews()[i],
                                              m_depthImageView};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = getSwapChainExtent().width;
    framebufferInfo.height = getSwapChainExtent().height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(mp_context->getDevice(), &framebufferInfo, nullptr,
                            &getSwapChainFramebuffers()[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}
