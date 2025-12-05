#pragma once

#include "BufferManager/BufferManager.h"
#include "BufferManager/UniformBufferManager.h"
#include "Commands/CommandManager.h"
#include "Common/UniformBufferObject.h"
#include "DescriptorManager/DescriptorManager.h"
#include "Meshes/Mesh.h"
#include "Pipeline/Pipeline.h"
#include "Pipeline/RenderPass.h"
#include "Scene/Camera/Camera.h"
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

  void onFrameBufferResize();

private:
  void createVertexBuffer();
  void createIndexBuffer();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  void updateUniformBuffer(uint32_t currentImage);
  void drawFrame(uint32_t flightCurrentFrame);
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

  Mesh m_dragonMesh;

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;

  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  std::vector<UBOManager> m_uniformBufferManager;

  DescriptorManager m_descriptorManager;
  std::vector<VkDescriptorSet> m_descriptorSets;

  VulkanSyncManager m_syncManager;

  bool framebufferResized = false;

  Camera m_camera;
};
