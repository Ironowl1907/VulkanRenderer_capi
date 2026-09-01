#pragma once

#include "volk/volk.h"
#include <cstdint>

uint32_t findMemoryType(VkPhysicalDevice device,  uint32_t typeFilter, VkMemoryPropertyFlags properties);
