#ifndef _VULKAN_DESCRIPTOR
#define _VULKAN_DESCRIPTOR

namespace ComputeEngine
{
    struct VulkanDescriptor
    {
        VulkanBuffer* attached_buffer;
        VkDescriptorSetLayout descriptor_layout;
        VkDescriptorPool descriptor_pool;
        VkDescriptorSet descriptor_set;
    };

    void DestroyDescriptorSet(SupportedDevice supported_device, VulkanDescriptor descriptor);
    VulkanDescriptor CreateDescriptorSet(SupportedDevice supported_device, VulkanBuffer vulkan_buffer, VkDescriptorType descriptor_type);
};

#include "VulkanDescriptor.cpp"
#endif