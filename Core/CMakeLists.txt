set(SOURCES
  src/Core/Application.cpp
  src/Core/Window.cpp

  src/Renderer/Renderer.cpp
  src/Renderer/DescriptorManager/DescriptorManager.cpp
  src/Renderer/Common/CommandUtils/CommandUtils.cpp
  src/Renderer/Common/MemoryType/MemoryType.cpp
  src/Renderer/Common/Images/CreateImage.cpp
  src/Renderer/Common/Files/readFile.cpp
  src/Renderer/Common/SwapchainSupportDetails.cpp
  src/Renderer/Core/VulkanContext.cpp
  src/Renderer/Pipeline/RenderPass.cpp
  src/Renderer/Pipeline/Pipeline.cpp
  src/Renderer/BufferManager/BufferManager.cpp
  src/Renderer/Commands/CommandManager.cpp
  src/Renderer/Texture/Texture.cpp
  src/Renderer/Swapchain/Swapchain.cpp
  src/Renderer/VulkanSyncObjects/VulkanSyncObjects.cpp
  src/Renderer/BufferManager/UniformBufferManager.cpp
  src/Scene/Camera/Camera.cpp
  src/Renderer/RenderObjects/Mesh/Mesh.cpp

  src/Renderer/RenderObjects/ObjectManager.cpp
)

set(VULKAN_SDK_PATH "/home/ironowl/vulkan_sdk/1.4.328.1/x86_64/" CACHE PATH "Path to Vulkan SDK")

set(VULKAN_INCLUDE_DIR "${VULKAN_SDK_PATH}/include")
set(VULKAN_LIBRARY_DIR "${VULKAN_SDK_PATH}/lib")
set(VULKAN_LIBRARY "${VULKAN_SDK_PATH}/lib/libvulkan.so") 

add_library(Core STATIC)

target_sources(Core PRIVATE ${SOURCES})

# Match VS filters to directory structure on disk
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_compile_definitions(
    GLFW_INCLUDE_NONE
)

# target_link_libraries(Core glfw)
# target_link_libraries(Core glad)
# target_link_libraries(Core glm)

target_link_libraries(Core PRIVATE
    ${VULKAN_LIBRARY}
    glfw
)


target_include_directories(Core PUBLIC
    ${VULKAN_INCLUDE_DIR}
)

target_include_directories(Core PUBLIC "src" "vendor" "src/Renderer")
