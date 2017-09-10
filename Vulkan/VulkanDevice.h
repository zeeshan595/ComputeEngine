#ifndef _VULKAN_DEVICE
#define _VULKAN_DEVICE

namespace ComputeEngine
{
    std::vector<const char*> device_layer_names         = { "VK_LAYER_LUNARG_standard_validation" };
    std::vector<const char*> device_extension_names     = {  };

    struct SupportedPhysicalDevice
    {
        VkPhysicalDevice                physical_device;
        uint32_t                        queue_index;
        VkPhysicalDeviceProperties      device_properties;
    };

    struct SupportedDevice
    {
        VkPhysicalDevice                physical_device;
        VkDevice                        device;
        uint32_t                        queue_index;
        VkQueue                         queue;
        VkPhysicalDeviceProperties      device_properties;
    };

    std::vector<SupportedDevice> CreateDevices(VkInstance instance);
    std::vector<SupportedPhysicalDevice> FindPhysicalDevices(VkInstance instance);
    void DestroyDevices(std::vector<SupportedDevice> devices);
    SupportedDevice CreateDevice(SupportedPhysicalDevice physical_device);
    uint32_t GetQueueFamilyIndex(VkPhysicalDevice physical_device);
    bool CheckDeviceSupport(VkPhysicalDevice physical_device);
};

#include "VulkanDevice.cpp"
#endif