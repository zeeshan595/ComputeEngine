#ifndef _VULKAN_INSTANCE
#define _VULKAN_INSTANCE

namespace ComputeEngine
{
    struct VulkanInstance
    {
        VkInstance                  vulkan_instance;
        VkDebugReportCallbackEXT    debug_report_callback;
    };

    std::vector<const char*> instance_layer_names       = { "VK_LAYER_LUNARG_standard_validation" };
    std::vector<const char*> instance_extension_names   = { VK_EXT_DEBUG_REPORT_EXTENSION_NAME };

    void DestroyVulkanInstance(VulkanInstance instance);
    void DestroyDebugCallback(VkInstance instance, VkDebugReportCallbackEXT debug_callback);
    VulkanInstance CreateVulkanInstance();
    VulkanInstance CreateVulkanInstance(PFN_vkDebugReportCallbackEXT debug_report_callback_func);
    VkDebugReportCallbackEXT CreateDebugCallback(VkInstance instance, PFN_vkDebugReportCallbackEXT debug_report_callback_func);
    void CheckInstanceSupport();
};

//Debug callback function
VKAPI_ATTR VkBool32 VKAPI_CALL DefaultDebugReportCallbackFunc(
    VkDebugReportFlagsEXT                       flags,
    VkDebugReportObjectTypeEXT                  objectType,
    uint64_t                                    object,
    size_t                                      location,
    int32_t                                     messageCode,
    const char*                                 pLayerPrefix,
    const char*                                 pMessage,
    void*                                       pUserData)
{
    std::cout << "Debug Report: " << pLayerPrefix << ": " << pMessage << std::endl;

    return VK_TRUE;
}

#include "VulkanInstance.cpp"
#endif