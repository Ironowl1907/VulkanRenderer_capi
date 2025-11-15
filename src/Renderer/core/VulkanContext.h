#include "vulkan/vulkan_core.h"
#include <GLFW/glfw3.h>
#include <optional>
#include <vector>

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
  GLFWwindow *getWindow() { return m_window; }
  VkSurfaceKHR &getSurface() { return m_surface; }

  VkPhysicalDevice &getPhysicalDevice() { return m_physicalDevice; }
  VkDevice &getDevice() { return m_device; }

private:
  void initWindow(int width, int height,
                  void (*resizecallback)(GLFWwindow *window, int width,
                                         int height));
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

  void createSurface();

  void pickPhysicalDevice();

  bool isDeviceSuitable(VkPhysicalDevice device);

  void createLogicalDevice();

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  bool checkDeviceExtensionSupport(VkPhysicalDevice device);

private:
  VkInstance m_instance;

  bool m_validationLayersEnabled;

  VkDebugUtilsMessengerEXT m_debugMessenger;

  VkSurfaceKHR m_surface;

  GLFWwindow *m_window;

  VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

  VkDevice m_device;
};
