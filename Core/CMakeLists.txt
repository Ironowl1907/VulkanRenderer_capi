set(SOURCES
  src/Core/Application.cpp
  src/Core/Window.cpp

  src/Renderer/Renderer.cpp
  src/Renderer/Descriptors/Descriptors.cpp
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

find_package(Vulkan REQUIRED COMPONENTS volk)

add_library(Core STATIC)

target_sources(Core PRIVATE ${SOURCES})

# Match VS filters to directory structure on disk
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_compile_definitions(
    GLFW_INCLUDE_NONE
)

# Link against the imported target Vulkan::Vulkan instead of the raw .so file
target_link_libraries(Core PUBLIC
    Vulkan::volk
    glfw
)

target_include_directories(Core PUBLIC "src" "vendor" "src/Renderer")
