#include "vulkan/vulkan_core.h"
#include <vector>
class VulkanContext {
public:
  void init(bool validationLayersEnabled,
            std::vector<const char *> validationLayers);
  void shutdown();

  const VkInstance &getInstance() const { return m_instance; }

private:
  bool checkValidationLayerSupport(std::vector<const char *> validationLayers);

  std::vector<const char *> getRequiredExtensions(bool validationLayers = true);

  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo);

  void setupDebugMessenger(bool validationLayersEnabled);

  VkResult CreateDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
      const VkAllocationCallbacks *pAllocator,
      VkDebugUtilsMessengerEXT *pDebugMessenger);

  void destroyDebugUtilsMessengerEXT(VkInstance instance,
                                     VkDebugUtilsMessengerEXT debugMessenger,
                                     const VkAllocationCallbacks *pAllocator);

private:
  VkInstance m_instance;

  bool m_validationLayersEnabled;

  VkDebugUtilsMessengerEXT m_debugMessenger;
};
