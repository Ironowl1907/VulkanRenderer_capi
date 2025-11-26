#pragma once

#include "vulkan/vulkan_core.h"
void transitionImageLayout(VkImage image, VkFormat format,
                           VkImageLayout oldLayout, VkImageLayout newLayout);
