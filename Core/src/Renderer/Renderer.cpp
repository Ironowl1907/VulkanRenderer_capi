#include "Renderer.h"
#include "BufferManager/UniformBufferManager.h"
#include "Common/UniformBufferObject.h"
#include "Core/Application.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <glm/fwd.hpp>
#include <vector>

#include "Descriptors/Descriptors.h"
#include "RenderObjects/Mesh/Mesh.h"
#include "Scene/Camera/Camera.h"
#include "vulkan/vulkan_core.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

const int MAX_FRAMES_IN_FLIGHT = 2;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

// Define static member
RendererData Renderer::s_Data = {};

void Renderer::OnFrameBufferResize() { s_Data.framebufferResized = true; }

void Renderer::Init(const std::string &vertShaderPath,
                    const std::string &fragShaderPath) {
  s_Data.vertShaderPath = vertShaderPath;
  s_Data.fragShaderPath = fragShaderPath;

  InitVulkan();
}

void Renderer::InitVulkan() {
  ApplicationInfo appInfo;
  appInfo.width = Core::Application::Get().getWindow()->getFramebufferSize().x;
  appInfo.height = Core::Application::Get().getWindow()->getFramebufferSize().y;
  appInfo.validationLayersEnabled = enableValidationLayers;
  appInfo.validationLayers = validationLayers;

  s_Data.context.init(appInfo);

  s_Data.swapchain.init(&s_Data.context);
  s_Data.swapchain.createSwapChain();
  s_Data.swapchain.createImageViews();
  s_Data.swapchain.createDepthResources();

  s_Data.renderPass.init(&s_Data.context, s_Data.swapchain);

  s_Data.swapchain.createFramebuffers(s_Data.renderPass.getRenderPass());

  s_Data.globalPool =
      DescriptorPoolBuilder(&s_Data.context)
          .setMaxSets(MAX_FRAMES_IN_FLIGHT)
          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT)
          .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                       MAX_FRAMES_IN_FLIGHT)
          .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
          .build();

  s_Data.globalSetLayout =
      DescriptorSetLayoutBuilder(&s_Data.context)
          .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                      VK_SHADER_STAGE_VERTEX_BIT)
          .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                      VK_SHADER_STAGE_FRAGMENT_BIT)
          .build();

  s_Data.pipeline.init(&s_Data.context, s_Data.renderPass,
                       *s_Data.globalSetLayout, s_Data.vertShaderPath,
                       s_Data.fragShaderPath);

  s_Data.commandManager.init(&s_Data.context);
  s_Data.commandManager.createCommandPools();

  s_Data.bufferManager.init(&s_Data.context, &s_Data.commandManager);

  s_Data.objectManager.init(&s_Data.context, &s_Data.bufferManager);

  s_Data.whiteTexture.init(&s_Data.context, &s_Data.commandManager);
  s_Data.whiteTexture.createDefaultWhite(&s_Data.bufferManager);

  s_Data.uniformBufferManager.resize(MAX_FRAMES_IN_FLIGHT);
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    s_Data.uniformBufferManager[i] = UBOManager(
        &s_Data.context, &s_Data.bufferManager, sizeof(UniformBufferObject));
  }

  s_Data.globalDescriptorSets =
      std::vector<VkDescriptorSet>(MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < s_Data.globalDescriptorSets.size(); i++) {
    auto bufferInfo = s_Data.uniformBufferManager[i].getBufferInfo();
    auto imageInfo = s_Data.whiteTexture.getImageDescriptor();

    DescriptorWriter(*s_Data.globalSetLayout, *s_Data.globalPool)
        .writeBuffer(0, &bufferInfo)
        .writeImage(1, &imageInfo)
        .build(s_Data.globalDescriptorSets[i]);
  }

  s_Data.commandManager.allocateFrameCommandBuffers(MAX_FRAMES_IN_FLIGHT);

  s_Data.syncManager.init(&s_Data.context, MAX_FRAMES_IN_FLIGHT,
                          s_Data.swapchain.getSwapChainImages().size());
}

void Renderer::Cleanup() {
  vkDeviceWaitIdle(s_Data.context.getDevice());

  s_Data.whiteTexture.cleanup();

  s_Data.objectManager.shutdown();

  s_Data.pipeline.shutdown();
  s_Data.renderPass.shutdown();

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    s_Data.uniformBufferManager[i].shutdown();
  }

  s_Data.globalPool->freeDescriptors(s_Data.globalDescriptorSets);
  s_Data.globalPool->shutdown();
	s_Data.globalSetLayout->cleanup();

  s_Data.syncManager.cleanup();

  s_Data.commandManager.shutdown();

  s_Data.swapchain.shutdown();

  s_Data.context.shutdown();

  glfwTerminate();
}

void Renderer::RecordCommandBuffer(VkCommandBuffer commandBuffer,
                                   uint32_t imageIndex) {

  vkResetCommandBuffer(commandBuffer, 0);
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = s_Data.renderPass.getRenderPass();
  renderPassInfo.framebuffer =
      s_Data.swapchain.getSwapChainFramebuffers()[imageIndex];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = s_Data.swapchain.getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
  clearValues[1].depthStencil = {1.0f, 0};

  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    s_Data.pipeline.getPipeline());

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)s_Data.swapchain.getSwapChainExtent().width;
  viewport.height = (float)s_Data.swapchain.getSwapChainExtent().height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = s_Data.swapchain.getSwapChainExtent();
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  // Bind shared vertex and index buffers
  VkBuffer vertexBuffers[] = {s_Data.objectManager.getVertexBuffer()};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
  vkCmdBindIndexBuffer(commandBuffer, s_Data.objectManager.getIndexBuffer(), 0,
                       VK_INDEX_TYPE_UINT32);

  // Bind descriptor sets once
  vkCmdBindDescriptorSets(
      commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
      s_Data.pipeline.getPipelineLayout(), 0, 1,
      &s_Data.globalDescriptorSets[s_Data.syncManager.getFlightFrameIndex()], 0,
      nullptr);

  // Draw all queued objects
  for (uint32_t objId : s_Data.drawQueue) {
    const ObjBufferInfo &objInfo = s_Data.objectManager.getObjInfo(objId);

    vkCmdDrawIndexed(commandBuffer, objInfo.indexCount, 1, objInfo.indexOffset,
                     objInfo.vertexOffsetValue, 0);
  }

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}

void Renderer::UpdateUniformBuffer(Camera camera) {
  UniformBufferObject ubo{};

  ubo.model = glm::mat4(1.0f);
  ubo.view = camera.getViewMatrix();
  ubo.proj = camera.getProjectionMatrix();

  s_Data.uniformBufferManager[s_Data.syncManager.getFlightFrameIndex()]
      .writeData(&ubo);
}

void Renderer::BeginDraw() {
  s_Data.frameData.frameFence = s_Data.syncManager.getFrameFence();
  vkWaitForFences(s_Data.context.getDevice(), 1, &s_Data.frameData.frameFence,
                  VK_TRUE, UINT64_MAX);
  vkResetFences(s_Data.context.getDevice(), 1, &s_Data.frameData.frameFence);

  s_Data.frameData.adquireSemaphore = s_Data.syncManager.getAcquireSemaphore();

  VkResult result = vkAcquireNextImageKHR(
      s_Data.context.getDevice(), s_Data.swapchain.getSwapChain(), UINT64_MAX,
      s_Data.frameData.adquireSemaphore, VK_NULL_HANDLE,
      &s_Data.frameData.swapChainImageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    s_Data.swapchain.recreateSwapChain(s_Data.renderPass.getRenderPass());
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  s_Data.frameData.commandBuffer = s_Data.commandManager.getFrameCommandBuffer(
      s_Data.syncManager.getFlightFrameIndex());

  // Clear the draw queue for this frame
  s_Data.drawQueue.clear();
}

void Renderer::EndDraw() {
  RecordCommandBuffer(s_Data.frameData.commandBuffer,
                      s_Data.frameData.swapChainImageIndex);

  VkSemaphore submitSemaphore = s_Data.syncManager.getSubmitSemaphore(
      s_Data.frameData.swapChainImageIndex);
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {s_Data.frameData.adquireSemaphore};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &s_Data.frameData.commandBuffer;

  VkSemaphore signalSemaphores[] = {submitSemaphore};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(s_Data.context.getGraphicsQueue(), 1, &submitInfo,
                    s_Data.frameData.frameFence) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {s_Data.swapchain.getSwapChain()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &s_Data.frameData.swapChainImageIndex;

  VkResult result;
  result = vkQueuePresentKHR(s_Data.context.getPresentQueue(), &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      s_Data.framebufferResized) {
    s_Data.framebufferResized = false;
    s_Data.swapchain.recreateSwapChain(s_Data.renderPass.getRenderPass());
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  s_Data.syncManager.nextFlightFrame();
}

void Renderer::DrawObject(uint32_t objID) {
  // Add object to the draw queue
  s_Data.drawQueue.push_back(objID);
}

uint32_t Renderer::addObject(RenderObject &obj) {
  return s_Data.objectManager.addRenderObject(obj);
}
