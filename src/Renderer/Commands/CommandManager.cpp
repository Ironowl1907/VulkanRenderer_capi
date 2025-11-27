#include "CommandManager.h"
#include "Swapchain/Swapchain.h"
#include <stdexcept>

void CommandManager::init(VulkanContext *p_context) { mp_context = p_context; }

void CommandManager::shutdown() {
    vkDestroyCommandPool(mp_context->getDevice(), m_graphicsPool, nullptr);
}

void CommandManager::createCommandPools() {
  QueueFamilyIndices queueFamilyIndices =
      mp_context->findQueueFamilies(mp_context->getPhysicalDevice());

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

  if (vkCreateCommandPool(mp_context->getDevice(), &poolInfo, nullptr,
                          &m_graphicsPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics command pool!");
  }
}

void CommandManager::allocateFrameCommandBuffers(uint32_t framesInFlight) {
  m_frameCmdBuffers.resize(framesInFlight);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = m_graphicsPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t)m_frameCmdBuffers.size();

  if (vkAllocateCommandBuffers(mp_context->getDevice(), &allocInfo,
                               m_frameCmdBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

VkCommandBuffer CommandManager::beginOneTimeCommands() {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = m_graphicsPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(mp_context->getDevice(), &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}

void CommandManager::endOneTimeCommands(VkCommandBuffer cmd) {
  vkEndCommandBuffer(cmd);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &cmd;

  vkQueueSubmit(mp_context->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(mp_context->getGraphicsQueue());

  vkFreeCommandBuffers(mp_context->getDevice(), m_graphicsPool, 1, &cmd);
}
