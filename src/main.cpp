#include "BufferManager/BufferManager.h"
#include "Common/Vertex.h"
#include "DescriptorManager/DescriptorManager.h"
#include "Pipeline/Pipeline.h"
#include "Pipeline/RenderPass.h"
#include "Swapchain/Swapchain.h"
#include "Texture/Texture.h"
#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

#include "Common/Images/CreateImage.h"
#include "Common/UniformBufferObject.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}};

const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

class HelloTriangleApplication {
public:
  void run() {
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  VulkanContext m_context;
  Swapchain m_swapchain;
  RenderPass m_renderPass;
  Pipeline m_pipeline;

  VkCommandPool commandPool;

  BufferManager m_bufferManager;

  Texture m_demoTexture;

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;

  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void *> uniformBuffersMapped;

  DescriptorManager m_descriptorManager;

  std::vector<VkCommandBuffer> commandBuffers;

  std::vector<VkSemaphore> submitSemaphores;
  std::vector<VkSemaphore> adquiredSemaphore;
  std::vector<VkFence> frameFences;
  uint32_t inFlightCurrentFrame = 0;

  bool framebufferResized = false;

  static void framebufferResizeCallback(GLFWwindow *window, int width,
                                        int height) {
    auto app = reinterpret_cast<HelloTriangleApplication *>(
        glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
  }

  void initVulkan() {
    ApplicationInfo appInfo;
    appInfo.width = WIDTH, appInfo.height = HEIGHT;
    appInfo.validationLayersEnabled = enableValidationLayers;
    appInfo.validationLayers = validationLayers;

    m_context.init(appInfo, framebufferResizeCallback);

    m_swapchain.init(&m_context);
    m_swapchain.createSwapChain();
    m_swapchain.createImageViews();
    m_swapchain.createDepthResources();

    m_renderPass.init(&m_context, m_swapchain);

    m_swapchain.createFramebuffers(m_renderPass.getRenderPass());

    m_pipeline.init(&m_context, m_renderPass);

    m_bufferManager.init(&m_context, &commandPool);

    createCommandPool();
    m_demoTexture.init(&m_context, &commandPool);
    m_demoTexture.loadFromFile(&m_context, &m_bufferManager,
                               "../textures/mondongo.jpg");
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    m_descriptorManager.init(&m_context);
    m_descriptorManager.createPool(MAX_FRAMES_IN_FLIGHT);
    std::vector<VkDescriptorSetLayout> layouts(
        MAX_FRAMES_IN_FLIGHT, m_pipeline.getDescriptionSetLayout());
    m_descriptorManager.allocateSets(layouts, uniformBuffers, m_demoTexture,
                                     MAX_FRAMES_IN_FLIGHT);
    createCommandBuffers();
    createSyncObjects();
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(m_context.getWindow())) {
      glfwPollEvents();
      drawFrame();
    }

    vkDeviceWaitIdle(m_context.getDevice());
  }

  void cleanup() {
    m_pipeline.shutdown();
    m_renderPass.shutdown();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroyBuffer(m_context.getDevice(), uniformBuffers[i], nullptr);
      vkFreeMemory(m_context.getDevice(), uniformBuffersMemory[i], nullptr);
    }

    m_demoTexture.cleanup(&m_context);

    vkDestroyDescriptorSetLayout(m_context.getDevice(),
                                 m_pipeline.getDescriptionSetLayout(), nullptr);

    vkDestroyBuffer(m_context.getDevice(), indexBuffer, nullptr);
    vkFreeMemory(m_context.getDevice(), indexBufferMemory, nullptr);

    vkDestroyBuffer(m_context.getDevice(), vertexBuffer, nullptr);
    vkFreeMemory(m_context.getDevice(), vertexBufferMemory, nullptr);

    for (size_t i = 0; i < m_swapchain.getSwapChainImages().size(); i++) {
      vkDestroySemaphore(m_context.getDevice(), submitSemaphores[i], nullptr);
      vkDestroySemaphore(m_context.getDevice(), adquiredSemaphore[i], nullptr);
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroyFence(m_context.getDevice(), frameFences[i], nullptr);
    }

    vkDestroyCommandPool(m_context.getDevice(), commandPool, nullptr);
    m_swapchain.shutdown();

    m_context.shutdown();

    glfwTerminate();
  }

  void createCommandPool() {
    QueueFamilyIndices queueFamilyIndices =
        m_context.findQueueFamilies(m_context.getPhysicalDevice());

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(m_context.getDevice(), &poolInfo, nullptr,
                            &commandPool) != VK_SUCCESS) {
      throw std::runtime_error("failed to create graphics command pool!");
    }
  }

  bool hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
           format == VK_FORMAT_D24_UNORM_S8_UINT;
  }

  void createVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    m_bufferManager.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                 stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(m_context.getDevice(), stagingBufferMemory, 0, bufferSize, 0,
                &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(m_context.getDevice(), stagingBufferMemory);

    m_bufferManager.createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    m_bufferManager.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(m_context.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(m_context.getDevice(), stagingBufferMemory, nullptr);
  }

  void createIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    m_bufferManager.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                 stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(m_context.getDevice(), stagingBufferMemory, 0, bufferSize, 0,
                &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(m_context.getDevice(), stagingBufferMemory);

    m_bufferManager.createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    m_bufferManager.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(m_context.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(m_context.getDevice(), stagingBufferMemory, nullptr);
  }

  void createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      m_bufferManager.createBuffer(bufferSize,
                                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                   uniformBuffers[i], uniformBuffersMemory[i]);

      vkMapMemory(m_context.getDevice(), uniformBuffersMemory[i], 0, bufferSize,
                  0, &uniformBuffersMapped[i]);
    }
  }

  VkCommandBuffer beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_context.getDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
  }

  void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_context.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_context.getGraphicsQueue());

    vkFreeCommandBuffers(m_context.getDevice(), commandPool, 1, &commandBuffer);
  }

  void createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(m_context.getDevice(), &allocInfo,
                                 commandBuffers.data()) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate command buffers!");
    }
  }

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass.getRenderPass();
    renderPassInfo.framebuffer =
        m_swapchain.getSwapChainFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_swapchain.getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_pipeline.getPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_swapchain.getSwapChainExtent().width;
    viewport.height = (float)m_swapchain.getSwapChainExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swapchain.getSwapChainExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipeline.getPipelineLayout(), 0, 1,
        &m_descriptorManager.getDescriptorSets()[inFlightCurrentFrame], 0,
        nullptr);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0,
                     0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }

  void createSyncObjects() {
    submitSemaphores.resize(m_swapchain.getSwapChainImages().size());
    adquiredSemaphore.resize(m_swapchain.getSwapChainImages().size());
    frameFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      if (vkCreateFence(m_context.getDevice(), &fenceInfo, nullptr,
                        &frameFences[i]) != VK_SUCCESS) {
        throw std::runtime_error(
            "failed to create synchronization objects for a frame!");
      }
    }

    for (int i = 0; i < m_swapchain.getSwapChainImages().size(); i++) {
      if (vkCreateSemaphore(m_context.getDevice(), &semaphoreInfo, nullptr,
                            &submitSemaphores[i]) != VK_SUCCESS ||
          vkCreateSemaphore(m_context.getDevice(), &semaphoreInfo, nullptr,
                            &adquiredSemaphore[i]) != VK_SUCCESS) {
        throw std::runtime_error(
            "failed to create synchronization objects for a frame!");
      }
    }
  }

  void updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(
                     currentTime - startTime)
                     .count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view =
        glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj =
        glm::perspective(glm::radians(45.0f),
                         m_swapchain.getSwapChainExtent().width /
                             (float)m_swapchain.getSwapChainExtent().height,
                         0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
  }

  void drawFrame() {
    VkFence frameFence = frameFences[inFlightCurrentFrame];
    vkWaitForFences(m_context.getDevice(), 1, &frameFence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_context.getDevice(), 1, &frameFence);

    uint32_t swapChainImageIndex;
    VkSemaphore acquireSemaphore = adquiredSemaphore[inFlightCurrentFrame];

    VkResult result = vkAcquireNextImageKHR(
        m_context.getDevice(), m_swapchain.getSwapChain(), UINT64_MAX,
        acquireSemaphore, VK_NULL_HANDLE, &swapChainImageIndex);

    VkSemaphore submitSemaphore = submitSemaphores[swapChainImageIndex];

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      m_swapchain.recreateSwapChain(m_renderPass.getRenderPass());
      return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire swap chain image!");
    }

    VkCommandBuffer commandBuffer = commandBuffers[inFlightCurrentFrame];

    updateUniformBuffer(inFlightCurrentFrame);

    vkResetCommandBuffer(commandBuffer, 0);
    recordCommandBuffer(commandBuffer, swapChainImageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {acquireSemaphore};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = {submitSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_context.getGraphicsQueue(), 1, &submitInfo,
                      frameFence) != VK_SUCCESS) {
      throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_swapchain.getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &swapChainImageIndex;

    result = vkQueuePresentKHR(m_context.getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        framebufferResized) {
      framebufferResized = false;
      m_swapchain.recreateSwapChain(m_renderPass.getRenderPass());
    } else if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swap chain image!");
    }

    inFlightCurrentFrame = (inFlightCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }
};

int main() {
  HelloTriangleApplication app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
