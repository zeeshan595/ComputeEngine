#ifndef _VULKAN_COMMAND_BUFFER
#define _VULKAN_COMMAND_BUFFER

namespace ComputeEngine
{
    struct VulkanCommandBuffer
    {
        VkCommandPool command_pool;
        VkCommandBuffer command_buffer;
    };

    void DestroyCommandBuffer(SupportedDevice support_device, VulkanCommandBuffer command_buffer);
    VulkanCommandBuffer CreateCommandBuffer(SupportedDevice support_device, VulkanPipeline pipeline, VulkanDescriptor descriptor,
        uint32_t work_group_x, uint32_t work_group_y, uint32_t work_group_z);    
};

#include "VulkanCommandBuffer.cpp"
#endif