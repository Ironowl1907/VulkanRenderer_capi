#include "Renderer.h"
#include "BufferManager/UniformBufferManager.h"
#include "Common/UniformBufferObject.h"
#include "Core/Application.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <glm/fwd.hpp>
#include <vector>

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

void Renderer::OnFrameBufferResize() { s_Data.FramebufferResized = true; }

void Renderer::Init(const std::string &vertShaderPath,
                    const std::string &fragShaderPath) {
  s_Data.VertShaderPath = vertShaderPath;
  s_Data.FragShaderPath = fragShaderPath;

  InitVulkan();
}

void Renderer::InitVulkan() {
  ApplicationInfo appInfo;
  appInfo.width = Core::Application::Get().getWindow()->getFramebufferSize().x;
  appInfo.height = Core::Application::Get().getWindow()->getFramebufferSize().y;
  appInfo.validationLayersEnabled = enableValidationLayers;
  appInfo.validationLayers = validationLayers;

  s_Data.Context.init(appInfo);

  s_Data.Swapchain.init(&s_Data.Context);
  s_Data.Swapchain.createSwapChain();
  s_Data.Swapchain.createImageViews();
  s_Data.Swapchain.createDepthResources();

  s_Data.RenderPass.init(&s_Data.Context, s_Data.Swapchain);

  s_Data.Swapchain.createFramebuffers(s_Data.RenderPass.getRenderPass());

  s_Data.Pipeline.init(&s_Data.Context, s_Data.RenderPass,
                       s_Data.VertShaderPath, s_Data.FragShaderPath);

  s_Data.CommandManager.init(&s_Data.Context);
  s_Data.CommandManager.createCommandPools();

  s_Data.BufferManager.init(&s_Data.Context, &s_Data.CommandManager);

  s_Data.ObjectManager.init(&s_Data.Context, &s_Data.BufferManager);

  s_Data.WhiteTexture.init(&s_Data.Context, &s_Data.CommandManager);
  s_Data.WhiteTexture.createDefaultWhite(&s_Data.BufferManager);

  s_Data.UniformBufferManager.resize(MAX_FRAMES_IN_FLIGHT);
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    s_Data.UniformBufferManager[i] = UBOManager(
        &s_Data.Context, &s_Data.BufferManager, sizeof(UniformBufferObject));
  }

  s_Data.DescriptorManager.init(&s_Data.Context);
  s_Data.DescriptorManager.createPool(MAX_FRAMES_IN_FLIGHT);
  std::vector<VkDescriptorSetLayout> layouts(
      MAX_FRAMES_IN_FLIGHT, s_Data.Pipeline.getDescriptionSetLayout());
  s_Data.DescriptorSets = s_Data.DescriptorManager.allocateSets(
      layouts, s_Data.UniformBufferManager, s_Data.WhiteTexture,
      MAX_FRAMES_IN_FLIGHT);

  s_Data.CommandManager.allocateFrameCommandBuffers(MAX_FRAMES_IN_FLIGHT);

  s_Data.SyncManager.init(&s_Data.Context, MAX_FRAMES_IN_FLIGHT,
                          s_Data.Swapchain.getSwapChainImages().size());
}

void Renderer::Cleanup() {
  vkDeviceWaitIdle(s_Data.Context.getDevice());

  s_Data.ObjectManager.shutdown();

  s_Data.Pipeline.shutdown();
  s_Data.RenderPass.shutdown();

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    s_Data.UniformBufferManager[i].shutdown();
  }

  vkDestroyDescriptorSetLayout(s_Data.Context.getDevice(),
                               s_Data.Pipeline.getDescriptionSetLayout(),
                               nullptr);

  s_Data.DescriptorManager.shutdown();

  s_Data.SyncManager.cleanup();

  s_Data.CommandManager.shutdown();

  s_Data.Swapchain.shutdown();

  s_Data.Context.shutdown();

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
  renderPassInfo.renderPass = s_Data.RenderPass.getRenderPass();
  renderPassInfo.framebuffer =
      s_Data.Swapchain.getSwapChainFramebuffers()[imageIndex];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = s_Data.Swapchain.getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
  clearValues[1].depthStencil = {1.0f, 0};

  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    s_Data.Pipeline.getPipeline());

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)s_Data.Swapchain.getSwapChainExtent().width;
  viewport.height = (float)s_Data.Swapchain.getSwapChainExtent().height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = s_Data.Swapchain.getSwapChainExtent();
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  // Bind shared vertex and index buffers
  VkBuffer vertexBuffers[] = {s_Data.ObjectManager.getVertexBuffer()};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
  vkCmdBindIndexBuffer(commandBuffer, s_Data.ObjectManager.getIndexBuffer(), 0,
                       VK_INDEX_TYPE_UINT32);

  // Bind descriptor sets once
  vkCmdBindDescriptorSets(
      commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
      s_Data.Pipeline.getPipelineLayout(), 0, 1,
      &s_Data.DescriptorSets[s_Data.SyncManager.getFlightFrameIndex()], 0,
      nullptr);

  // Draw all queued objects
  for (uint32_t objId : s_Data.DrawQueue) {
    const ObjBufferInfo &objInfo = s_Data.ObjectManager.getObjInfo(objId);

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

  s_Data.UniformBufferManager[s_Data.SyncManager.getFlightFrameIndex()]
      .writeData(&ubo);
}

void Renderer::BeginDraw() {
  s_Data.FrameData.FrameFence = s_Data.SyncManager.getFrameFence();
  vkWaitForFences(s_Data.Context.getDevice(), 1, &s_Data.FrameData.FrameFence,
                  VK_TRUE, UINT64_MAX);
  vkResetFences(s_Data.Context.getDevice(), 1, &s_Data.FrameData.FrameFence);

  s_Data.FrameData.AdquireSemaphore = s_Data.SyncManager.getAcquireSemaphore();

  VkResult result = vkAcquireNextImageKHR(
      s_Data.Context.getDevice(), s_Data.Swapchain.getSwapChain(), UINT64_MAX,
      s_Data.FrameData.AdquireSemaphore, VK_NULL_HANDLE,
      &s_Data.FrameData.SwapChainImageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    s_Data.Swapchain.recreateSwapChain(s_Data.RenderPass.getRenderPass());
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  s_Data.FrameData.CommandBuffer = s_Data.CommandManager.getFrameCommandBuffer(
      s_Data.SyncManager.getFlightFrameIndex());

  // Clear the draw queue for this frame
  s_Data.DrawQueue.clear();
}

void Renderer::EndDraw() {
  RecordCommandBuffer(s_Data.FrameData.CommandBuffer,
                      s_Data.FrameData.SwapChainImageIndex);

  VkSemaphore submitSemaphore = s_Data.SyncManager.getSubmitSemaphore(
      s_Data.FrameData.SwapChainImageIndex);
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {s_Data.FrameData.AdquireSemaphore};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &s_Data.FrameData.CommandBuffer;

  VkSemaphore signalSemaphores[] = {submitSemaphore};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(s_Data.Context.getGraphicsQueue(), 1, &submitInfo,
                    s_Data.FrameData.FrameFence) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {s_Data.Swapchain.getSwapChain()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &s_Data.FrameData.SwapChainImageIndex;

  VkResult result;
  result = vkQueuePresentKHR(s_Data.Context.getPresentQueue(), &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      s_Data.FramebufferResized) {
    s_Data.FramebufferResized = false;
    s_Data.Swapchain.recreateSwapChain(s_Data.RenderPass.getRenderPass());
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  s_Data.SyncManager.nextFlightFrame();
}

void Renderer::DrawObject(uint32_t objID) {
  // Add object to the draw queue
  s_Data.DrawQueue.push_back(objID);
}

uint32_t Renderer::addObject(RenderObject &obj) {
  return s_Data.ObjectManager.addRenderObject(obj);
}
