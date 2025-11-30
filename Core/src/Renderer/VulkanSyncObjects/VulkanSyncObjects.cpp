#include "VulkanSyncObjects/VulkanSyncObjects.h"
#include <cstdint>
#include <stdexcept>

void VulkanSyncManager::init(VulkanContext *p_context, uint32_t framesInFlight,
                             uint32_t swapChainImages) {
  mp_context = p_context;
  m_framesInFlight = framesInFlight;
  m_swapChainImagesSize = swapChainImages;
  m_submitSemaphores.resize(swapChainImages);
  m_adquiredSemaphore.resize(swapChainImages);
  m_frameFences.resize(framesInFlight);

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < framesInFlight; i++) {
    if (vkCreateFence(p_context->getDevice(), &fenceInfo, nullptr,
                      &m_frameFences[i]) != VK_SUCCESS) {
      throw std::runtime_error(
          "failed to create synchronization objects for a frame!");
    }
  }

  for (int i = 0; i < swapChainImages; i++) {
    if (vkCreateSemaphore(p_context->getDevice(), &semaphoreInfo, nullptr,
                          &m_submitSemaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(p_context->getDevice(), &semaphoreInfo, nullptr,
                          &m_adquiredSemaphore[i]) != VK_SUCCESS) {
      throw std::runtime_error(
          "failed to create synchronization objects for a frame!");
    }
  }
}

void VulkanSyncManager::nextFlightFrame() {
  m_inFlightCurrentFrame = (m_inFlightCurrentFrame + 1) % m_framesInFlight;
}

VkFence VulkanSyncManager::getFrameFence() {
  return m_frameFences[m_inFlightCurrentFrame];
}
VkSemaphore VulkanSyncManager::getAcquireSemaphore() {
  return m_adquiredSemaphore[m_inFlightCurrentFrame];
}
VkSemaphore VulkanSyncManager::getSubmitSemaphore(uint32_t imageIndex) {
  return m_submitSemaphores[imageIndex];
}

void VulkanSyncManager::cleanup() {
  for (size_t i = 0; i < m_swapChainImagesSize; i++) {
    vkDestroySemaphore(mp_context->getDevice(), m_submitSemaphores[i], nullptr);
    vkDestroySemaphore(mp_context->getDevice(), m_adquiredSemaphore[i],
                       nullptr);
  }

  for (size_t i = 0; i < m_framesInFlight; i++) {
    vkDestroyFence(mp_context->getDevice(), m_frameFences[i], nullptr);
  }
}
