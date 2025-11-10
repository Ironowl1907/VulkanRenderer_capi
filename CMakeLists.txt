cmake_minimum_required(VERSION 3.15)
project(VulkanRenderer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Custom Vulkan SDK path - modify this to your SDK location
set(VULKAN_SDK_PATH "~/vulkan_sdk/1.4.328.1/x86_64/" CACHE PATH "Path to Vulkan SDK")

# Set Vulkan include and library directories
set(VULKAN_INCLUDE_DIR "${VULKAN_SDK_PATH}/include")
set(VULKAN_LIBRARY_DIR "${VULKAN_SDK_PATH}/lib")

# Find Vulkan library
find_library(VULKAN_LIBRARY
    NAMES vulkan-1 vulkan
    PATHS ${VULKAN_LIBRARY_DIR}
    REQUIRED
)

# Create executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Renderer.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${VULKAN_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${VULKAN_LIBRARY}
)

# Print configuration info
message(STATUS "Vulkan SDK Path: ${VULKAN_SDK_PATH}")
message(STATUS "Vulkan Include: ${VULKAN_INCLUDE_DIR}")
message(STATUS "Vulkan Library: ${VULKAN_LIBRARY}")
