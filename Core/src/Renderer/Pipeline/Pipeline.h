#pragma once

#include "Pipeline/RenderPass.h"
#include "Swapchain/Swapchain.h"


#define VERT_SHADER_PATH "../App/Shaders/vert.spv"
#define FRAG_SHADER_PATH "../App/Shaders/frag.spv"

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
