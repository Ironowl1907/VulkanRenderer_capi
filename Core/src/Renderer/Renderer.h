#pragma once
#include "BufferManager/BufferManager.h"
#include "BufferManager/UniformBufferManager.h"
#include "Commands/CommandManager.h"
#include "DescriptorManager/DescriptorManager.h"
#include "Pipeline/Pipeline.h"
#include "Pipeline/RenderPass.h"
#include "RenderObjects/ObjectManager.h"
#include "RenderObjects/RenderObject.h"
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
  std::vector<UBOManager> UniformBufferManager;
  DescriptorManager DescriptorManager;
  std::vector<VkDescriptorSet> DescriptorSets;
  VulkanSyncManager SyncManager;
  bool FramebufferResized = false;
  struct {
    uint32_t flightCurrentFrame;
    uint32_t SwapChainImageIndex;
    VkCommandBuffer CommandBuffer;
    // Sync
    VkSemaphore AdquireSemaphore;
    VkSemaphore SubmitSemaphore;
    VkFence FrameFence;
  } FrameData;
  ObjManager ObjectManager;

  // Draw queue for batch rendering
  std::vector<uint32_t> DrawQueue;
  Texture WhiteTexture;
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
