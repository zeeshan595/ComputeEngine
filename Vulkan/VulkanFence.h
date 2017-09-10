#ifndef _VULKAN_FENCE
#define _VULKAN_FENCE

namespace ComputeEngine
{
    void DestroyFence(SupportedDevice supported_device, VkFence fence);
    VkFence CreateFence(SupportedDevice supported_device);
};

#include "VulkanFence.cpp"
#endif