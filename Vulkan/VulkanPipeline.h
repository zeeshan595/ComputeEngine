#ifndef _VULKAN_PIPELINE
#define _VULKAN_PIPELINE

namespace ComputeEngine
{
    struct VulkanPipeline
    {
        VkShaderModule      shader_module;
        VkPipelineLayout    pipeline_layout;
        VkPipeline          pipeline;
    };

    void DestroyPipeline(SupportedDevice supported_device, VulkanPipeline pipeline);
    VulkanPipeline CreatePipeline(SupportedDevice supported_device, VulkanDescriptor descriptor, const char* shader_path);    
    uint32_t* ReadShaderFile(uint32_t& length, const char* filename);
};

#include "VulkanPipeline.cpp"
#endif