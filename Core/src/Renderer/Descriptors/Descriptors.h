#pragma once
#include "Swapchain/Swapchain.h"
#include <memory>
class DescriptorSetLayout {
public:
  DescriptorSetLayout(
      VulkanContext *p_context,
      std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~DescriptorSetLayout();
  DescriptorSetLayout(const DescriptorSetLayout &) = delete;
  DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

  VkDescriptorSetLayout getDescriptorSetLayout() const {
    return m_descriptorSetLayout;
  }

private:
  VulkanContext *mp_context;
  VkDescriptorSetLayout m_descriptorSetLayout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;

  friend class DescriptorWriter;
};

class DescriptorSetLayoutBuilder {
public:
  DescriptorSetLayoutBuilder(VulkanContext *p_context)
      : mp_context(p_context) {};

  DescriptorSetLayoutBuilder &addBinding(uint32_t binding,
                                         VkDescriptorType descriptorType,
                                         VkShaderStageFlags stageFlags,
                                         uint32_t count = 1);
  std::unique_ptr<DescriptorSetLayout> build() const;

private:
  VulkanContext *mp_context;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings{};
};

class DescriptorPool {
public:
  DescriptorPool(VulkanContext *p_context, uint32_t maxSets,
                 VkDescriptorPoolCreateFlags poolFlags,
                 const std::vector<VkDescriptorPoolSize> &poolSizes);
  ~DescriptorPool();
  DescriptorPool(const DescriptorPool &) = delete;
  DescriptorPool &operator=(const DescriptorPool &) = delete;

  bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout,
                          VkDescriptorSet &descriptor) const;

  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

  void resetPool();

private:
  VulkanContext *mp_context;
  VkDescriptorPool descriptorPool;

  friend class DescriptorWriter;
};

class DescriptorPoolBuilder {
public:
  DescriptorPoolBuilder(VulkanContext *p_context) : mp_context(p_context) {}

  DescriptorPoolBuilder &addPoolSize(VkDescriptorType descriptorType,
                                     uint32_t count);
  DescriptorPoolBuilder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
  DescriptorPoolBuilder &setMaxSets(uint32_t count);
  std::unique_ptr<DescriptorPool> build() const;

private:
  VulkanContext *mp_context;
  std::vector<VkDescriptorPoolSize> poolSizes{};
  uint32_t maxSets = 1000;
  VkDescriptorPoolCreateFlags poolFlags = 0;
};

class DescriptorWriter {
public:
  DescriptorWriter(DescriptorSetLayout &setLayout, DescriptorPool &pool);

  DescriptorWriter &writeBuffer(uint32_t binding,
                                VkDescriptorBufferInfo *bufferInfo);
  DescriptorWriter &writeImage(uint32_t binding,
                               VkDescriptorImageInfo *imageInfo);

  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);

private:
  DescriptorSetLayout &setLayout;
  DescriptorPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};
