#pragma once
#include "BufferManager/BufferManager.h"
#include "BufferManager/UniformBufferManager.h"
#include "Commands/CommandManager.h"
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
#include <string>
#include <vector>

struct RendererData {
  std::string VertShaderPath;
  std::string FragShaderPath;

  VulkanContext Context;
  Swapchain Swapchain;
  RenderPass RenderPass;
  Pipeline Pipeline;

  VkSurfaceKHR Surface;

  BufferManager BufferManager;
  CommandManager CommandManager;

  Texture DemoTexture;
  Mesh DragonMesh;

  VkBuffer VertexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory VertexBufferMemory = VK_NULL_HANDLE;
  VkBuffer IndexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory IndexBufferMemory = VK_NULL_HANDLE;

  std::vector<UBOManager> UniformBufferManager;
  DescriptorManager DescriptorManager;
  std::vector<VkDescriptorSet> DescriptorSets;

  VulkanSyncManager SyncManager;

  bool FramebufferResized = false;
};

class Renderer {
public:
  static void Init(const std::string &vertShaderPath,
                   const std::string &fragShaderPath);
  static void Update(Camera camera);
  static void Cleanup();
  static void OnFrameBufferResize();

  static inline RendererData &GetData() { return s_Data; }

private:
  static void CreateVertexBuffer();
  static void CreateIndexBuffer();
  static void RecordCommandBuffer(VkCommandBuffer commandBuffer,
                                  uint32_t imageIndex);
  static void UpdateUniformBuffer(uint32_t currentImage, Camera camera);
  static void DrawFrame(uint32_t flightCurrentFrame);
  static void InitVulkan();

private:
  static RendererData s_Data;
};
