#ifndef _VULKAN_BUFFER
#define _VULKAN_BUFFER

namespace ComputeEngine
{
    struct VulkanBuffer
    {
        uint32_t            buffer_size;
        VkBuffer            buffer;
        VkDeviceMemory      device_memory;
    };

    void DestroyBuffer(SupportedDevice supported_device, VulkanBuffer vulkan_buffer);
    VulkanBuffer CreateBuffer(SupportedDevice supported_device, uint32_t buffer_size, VkMemoryPropertyFlags memory_properties);
    uint32_t GetMemoryType(VkPhysicalDevice physical_device, uint32_t memory_type_bits, VkMemoryPropertyFlags properties);
};

#include "VulkanBuffer.cpp"
#endif