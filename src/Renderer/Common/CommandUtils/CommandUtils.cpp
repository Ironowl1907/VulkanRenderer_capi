#include "CommandUtils.h"
#include "vulkan/vulkan_core.h"

VkCommandBuffer beginOneTimeCommands(VulkanContext *p_context,
                                     VkCommandPool *p_pool) {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = *p_pool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(p_context->getDevice(), &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}

void endOneTimeCommands(VulkanContext *p_context, VkCommandPool *p_pool,
                        VkCommandBuffer *p_commandBuffer) {
  vkEndCommandBuffer(*p_commandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = p_commandBuffer;

  vkQueueSubmit(p_context->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(p_context->getGraphicsQueue());

  vkFreeCommandBuffers(p_context->getDevice(), *p_pool, 1, p_commandBuffer);
}
