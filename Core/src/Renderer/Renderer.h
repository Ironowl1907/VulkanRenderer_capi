#pragma once

#include "BufferManager/BufferManager.h"
#include "Commands/CommandManager.h"
#include "DescriptorManager/DescriptorManager.h"
#include "Pipeline/Pipeline.h"
#include "Pipeline/RenderPass.h"
#include "Swapchain/Swapchain.h"
#include "Texture/Texture.h"
#include "VulkanSyncObjects/VulkanSyncObjects.h"
#include "vulkan/vulkan_core.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <vector>

class Renderer {
public:
  void init(const std::string &vertShaderPath,
            const std::string &fragShaderPath);
  void update();
  void cleanup();

private:
  static void framebufferResizeCallback(GLFWwindow *window, int width,
                                        int height);
  void createVertexBuffer();
  void createIndexBuffer();
  void createUniformBuffers();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  void updateUniformBuffer(uint32_t currentImage);
  void drawFrame();
  void initVulkan();

private:
  std::string m_vertShaderPath;
  std::string m_fragShaderPath;
  VulkanContext m_context;
  Swapchain m_swapchain;
  RenderPass m_renderPass;
  Pipeline m_pipeline;

  VkSurfaceKHR m_surface;

  BufferManager m_bufferManager;
  CommandManager m_commandManager;

  Texture m_demoTexture;

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;

  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void *> uniformBuffersMapped;

  DescriptorManager m_descriptorManager;
  std::vector<VkDescriptorSet> m_descriptorSets;

  VulkanSyncManager m_syncManager;

  bool framebufferResized = false;
};
