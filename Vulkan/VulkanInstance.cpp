void ComputeEngine::DestroyVulkanInstance(ComputeEngine::VulkanInstance instance)
{
    DestroyDebugCallback(instance.vulkan_instance, instance.debug_report_callback);
    vkDestroyInstance(instance.vulkan_instance, nullptr);
}

void ComputeEngine::DestroyDebugCallback(VkInstance instance, VkDebugReportCallbackEXT debug_callback)
{
    auto DestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    assert(DestroyDebugReportCallback != nullptr);
    DestroyDebugReportCallback(instance, debug_callback, nullptr);
}

ComputeEngine::VulkanInstance ComputeEngine::CreateVulkanInstance()
{
    return CreateVulkanInstance(&DefaultDebugReportCallbackFunc);
}

ComputeEngine::VulkanInstance ComputeEngine::CreateVulkanInstance(PFN_vkDebugReportCallbackEXT debug_report_callback_func)
{
    //Get Supported Layers & Extensions
    CheckInstanceSupport();

    //Create application info
    VkApplicationInfo       application_info = {};
    {
        application_info.sType                              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        application_info.pApplicationName                   = "Compute Test";
        application_info.applicationVersion                 = VK_MAKE_VERSION(1, 0, 0);
        application_info.pEngineName                        = "Compute Engine";
        application_info.engineVersion                      = VK_MAKE_VERSION(1, 0, 0);
        application_info.apiVersion                         = VK_API_VERSION_1_0;
    }

    //Create instance info using application info
    VkInstanceCreateInfo    instance_create_info = {};
    {
        instance_create_info.sType                      = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pApplicationInfo           = &application_info;
        instance_create_info.enabledLayerCount          = instance_layer_names.size();
        instance_create_info.ppEnabledLayerNames        = instance_layer_names.data();
        instance_create_info.enabledExtensionCount      = instance_extension_names.size();
        instance_create_info.ppEnabledExtensionNames    = instance_extension_names.data();
    }
    //Create vulkan instance using instance create info
    VkInstance instance = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance);
    //Check for errors
    assert(result == VK_SUCCESS && "Could not create vulkan instance");

    //Get debug report callback
    VkDebugReportCallbackEXT debug_report_callback = CreateDebugCallback(instance, debug_report_callback_func);

    //Return vulkan instance
    return { instance, debug_report_callback };
}

VkDebugReportCallbackEXT ComputeEngine::CreateDebugCallback(VkInstance instance, PFN_vkDebugReportCallbackEXT debug_report_callback_func)
{
    //Create report callback create info using debug_report_callback_func
    VkDebugReportCallbackCreateInfoEXT create_info = {};
    create_info.sType           = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    create_info.flags           = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    create_info.pfnCallback     = debug_report_callback_func;

    //Get the pointer to `vkCreateDebugReportCallbackEXT` function since its not exposed.
    auto CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    assert(CreateDebugReportCallback != nullptr);
    //Create debug report callback
    VkDebugReportCallbackEXT debug_report_callback;
    VkResult result = CreateDebugReportCallback(instance, &create_info, nullptr, &debug_report_callback);
    //Check for errors
    assert(result == VK_SUCCESS && "Could not create debug report callback");
    
    //Return debug report callback handel
    return debug_report_callback;
}

void ComputeEngine::CheckInstanceSupport()
{
    {//Layer names
        {
            //Get layers supported count
            uint32_t layer_count;
            vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

            //Get list of layer supported
            std::vector<VkLayerProperties> layer_properties(layer_count);
            vkEnumerateInstanceLayerProperties(&layer_count, layer_properties.data());

            //Check if all required layers are supported
            for (uint32_t i = 0; i < instance_layer_names.size(); i++)
            {
                std::string layer_name(instance_layer_names[i]);
                bool found = false;
                for (uint32_t j = 0; j < layer_count; j++)
                {
                    if (layer_name == std::string(layer_properties[j].layerName))
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    std::cout << "`" << layer_name << "` is not a support layer by vulkan instance" << std::endl;
                    std::cin.get();
                    std::exit(-1);
                }
            }
        }
    }

    {//Extension names
        {
            //Get extension supported count
            uint32_t ext_count;
            vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, nullptr);

            //Get list of extensions supported
            std::vector<VkExtensionProperties> ext_properties(ext_count);
            vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, ext_properties.data());

            //Check if all required extensions are supported
            for (uint32_t i = 0; i < instance_extension_names.size(); i++)
            {
                std::string extension_name(instance_extension_names[i]);
                bool found = false;
                for (uint32_t j = 0; j < ext_count; j++)
                {
                    if (extension_name == std::string(ext_properties[j].extensionName))
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    std::cout << "`" << extension_name << "` is not a support extension by vulkan instance" << std::endl;
                    std::cin.get();
                    std::exit(-1);
                }
            }
        }
    }
}