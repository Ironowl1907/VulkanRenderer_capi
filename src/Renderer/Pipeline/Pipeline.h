#pragma once

#include "Pipeline/RenderPass.h"
#include "Swapchain/Swapchain.h"
class Pipeline {
public:
  void init(VulkanContext *p_context, RenderPass &renderPass);
  void shutdown();

  VkDescriptorSetLayout &getDescriptionSetLayout() {
    return m_descriptorSetLayout;
  }
  VkPipelineLayout &getPipelineLayout() { return m_pipelineLayout; }
  VkPipeline &getPipeline() { return m_graphicsPipeline; }

private:
  void createDescriptorSetLayout();

  void createGraphicsPipeline(RenderPass &renderPass,
                              std::vector<char> vertShaderCode,
                              std::vector<char> fragShaderCode);

  VkShaderModule createShaderModule(const std::vector<char> &code);

private:
  VulkanContext *mp_context;

  VkDescriptorSetLayout m_descriptorSetLayout;
  VkPipelineLayout m_pipelineLayout;
  VkPipeline m_graphicsPipeline;
};
