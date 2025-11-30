#include "DescriptorManager.h"
#include "Common/UniformBufferObject.h"
#include "Texture/Texture.h"
#include <array>
#include <stdexcept>
#include <vector>

void DescriptorManager::init(VulkanContext *p_context) {
  mp_context = p_context;
}

void DescriptorManager::createPool(uint32_t framesInFlight) {

  std::array<VkDescriptorPoolSize, 2> poolSizes{};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = static_cast<uint32_t>(framesInFlight);
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = static_cast<uint32_t>(framesInFlight);

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = static_cast<uint32_t>(framesInFlight);

  if (vkCreateDescriptorPool(mp_context->getDevice(), &poolInfo, nullptr,
                             &m_pool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}
// TODO: Hack Implementation
std::vector<VkDescriptorSet>
DescriptorManager::allocateSets(std::vector<VkDescriptorSetLayout> layouts,
                                std::vector<VkBuffer> &uniformBuffers,
                                Texture &texutre, uint32_t framesInFlight) {

  std::vector<VkDescriptorSet> descriptorSets;

  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = m_pool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(framesInFlight);
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets.resize(framesInFlight);
  if (vkAllocateDescriptorSets(mp_context->getDevice(), &allocInfo,
                               descriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  for (size_t i = 0; i < framesInFlight; i++) {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffers[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = texutre.getImageView();
    imageInfo.sampler = texutre.getSampler();

    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSets[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSets[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(mp_context->getDevice(),
                           static_cast<uint32_t>(descriptorWrites.size()),
                           descriptorWrites.data(), 0, nullptr);
  }
  return descriptorSets;
}

void DescriptorManager::update(std::vector<VkDescriptorSet> &sets,
                               const std::vector<VkBuffer> &uniformBuffers,
                               VkImageView textureView,
                               VkSampler textureSampler) {}

void DescriptorManager::shutdown() {

  vkDestroyDescriptorPool(mp_context->getDevice(), m_pool, nullptr);
}
