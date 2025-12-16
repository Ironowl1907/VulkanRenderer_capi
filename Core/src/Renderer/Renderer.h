#pragma once
#include "BufferManager/BufferManager.h"
#include "BufferManager/UniformBufferManager.h"
#include "Commands/CommandManager.h"
#include "Descriptors/Descriptors.h"
#include "Pipeline/Pipeline.h"
#include "Pipeline/RenderPass.h"
#include "RenderObjects/ObjectManager.h"
#include "RenderObjects/RenderObject.h"
#include "Scene/Camera/Camera.h"
#include "Swapchain/Swapchain.h"
#include "Texture/Texture.h"
#include "VulkanSyncObjects/VulkanSyncObjects.h"
#include "vulkan/vulkan_core.h"
#include <memory>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>
#include <string>
#include <vector>

struct RendererData {
  std::string vertShaderPath;
  std::string fragShaderPath;
  VulkanContext context;
  Swapchain swapchain;
  RenderPass renderPass;
  Pipeline pipeline;
  VkSurfaceKHR surface;
  BufferManager bufferManager;
  CommandManager commandManager;
  std::vector<UBOManager> uniformBufferManager;

  std::vector<VkDescriptorSet> globalDescriptorSets;
  std::unique_ptr<DescriptorPool> globalPool;
  std::unique_ptr<DescriptorSetLayout> globalSetLayout;

  VulkanSyncManager syncManager;
  bool framebufferResized = false;
  struct {
    uint32_t flightCurrentFrame;
    uint32_t swapChainImageIndex;
    VkCommandBuffer commandBuffer;
    // Sync
    VkSemaphore adquireSemaphore;
    VkSemaphore submitSemaphore;
    VkFence frameFence;
  } frameData;
  ObjManager objectManager;

  // Draw queue for batch rendering
  std::vector<uint32_t> drawQueue;
  Texture whiteTexture;
};

class Renderer {
public:
  static void Init(const std::string &vertShaderPath,
                   const std::string &fragShaderPath);
  [[nodiscard]] static uint32_t addObject(RenderObject &obj);
  static void UpdateUniformBuffer(Camera camera);
  static void BeginDraw();
  static void EndDraw();
  static void DrawObject(uint32_t objID);
  static void SetClearColor(const glm::vec3 &color);
  static void Cleanup();
  static void OnFrameBufferResize();
  static inline RendererData &GetData() { return s_Data; }

private:
  static void CreateVertexBuffer();
  static void CreateIndexBuffer();
  static void RecordCommandBuffer(VkCommandBuffer commandBuffer,
                                  uint32_t imageIndex);
  static void InitVulkan();

private:
  static RendererData s_Data;
};
