#include "Core/Window.h"
#include "vulkan/vulkan_core.h"
#include <GLFW/glfw3.h>
#include <cassert>
#include <optional>
#include <vector>

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

struct ApplicationInfo {
  int width, height;
  bool validationLayersEnabled;

  std::vector<const char *> validationLayers;

  ApplicationInfo(int width, int height, bool validationLayersEnabled,
                  std::vector<const char *> validationLayers)
      : width(width), height(height),
        validationLayersEnabled(validationLayersEnabled),
        validationLayers(validationLayers) {}

  ApplicationInfo() {}
};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};
class VulkanContext {
public:
  void init(ApplicationInfo info,
            void (*resizeCallback)(GLFWwindow *window, int width, int height));
  void shutdown();

  const VkInstance &getInstance() const { return m_instance; }
  VkSurfaceKHR getSurface() { return m_surface; }

  VkPhysicalDevice &getPhysicalDevice() { return m_physicalDevice; }
  VkDevice &getDevice() { return m_device; }

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  const VkQueue &getGraphicsQueue() {
    assert(m_graphicsQueue != nullptr);
    return m_graphicsQueue;
  }
  const VkQueue &getPresentQueue() {
    assert(m_graphicsQueue != nullptr);
    return m_presentQueue;
  }

private:
  void initVulkan(bool validationLayersEnabled,
                  std::vector<const char *> validationLayers);
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

  void pickPhysicalDevice();

  bool isDeviceSuitable(VkPhysicalDevice device);

  void createLogicalDevice();

  bool checkDeviceExtensionSupport(VkPhysicalDevice device);

private:
  VkInstance m_instance;

  bool m_validationLayersEnabled;

  VkDebugUtilsMessengerEXT m_debugMessenger;

  VkSurfaceKHR m_surface;

  VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

  VkDevice m_device;

  VkQueue m_graphicsQueue;

  VkQueue m_presentQueue;
};
