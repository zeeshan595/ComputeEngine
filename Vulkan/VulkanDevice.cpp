std::vector<ComputeEngine::SupportedDevice> ComputeEngine::CreateDevices(VkInstance instance)
{
    std::vector<SupportedPhysicalDevice> physical_devices = FindPhysicalDevices(instance);
    std::vector<SupportedDevice> devices(physical_devices.size());
    for (int i = 0; i < physical_devices.size(); i++)
    {
        devices[i] = CreateDevice(physical_devices[i]);
    }

    return devices;
}

std::vector<ComputeEngine::SupportedPhysicalDevice> ComputeEngine::FindPhysicalDevices(VkInstance instance)
{
    //Get number of compute device (GPU)
    uint32_t device_count;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);
    //Make sure the compute has atleast 1 compute device (GPU)
    assert(device_count != 0 && "No compute devices found");

    //Get list of all compute devices (GPU)
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    std::vector<SupportedPhysicalDevice> supported_devices;
    for (int i = 0; i < device_count; i++)
    {
        uint32_t queue_index = GetQueueFamilyIndex(devices[i]);
        if (queue_index != -1 && CheckDeviceSupport(devices[i]))
        {
            //Get GPU properties (name, api e.t.c.)
            VkPhysicalDeviceProperties device_properties;
            vkGetPhysicalDeviceProperties(devices[i], &device_properties);
            //Add supported GPUs to the list
            supported_devices.push_back({ devices[i], queue_index, device_properties });
        }
    }

    return supported_devices;
}

void ComputeEngine::DestroyDevices(std::vector<SupportedDevice> devices)
{
    for (int i = 0; i < devices.size(); i++)
        vkDestroyDevice(devices[i].device, nullptr);
}

ComputeEngine::SupportedDevice ComputeEngine::CreateDevice(SupportedPhysicalDevice physical_device)
{
    //Create queue create info using compute family index
    VkDeviceQueueCreateInfo queue_create_info = {};
    {
        float queue_priorities = 1.0f;
        queue_create_info.sType                     = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex          = physical_device.queue_index;
        queue_create_info.queueCount                = 1;
        queue_create_info.pQueuePriorities          = &queue_priorities;
    }
    
    //Create device create info using queue create info
    VkDeviceCreateInfo  device_create_info = {};
    {
        //Don't need any extra features for computational work load
        VkPhysicalDeviceFeatures device_features = {};

        device_create_info.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.enabledLayerCount        = device_layer_names.size();
        device_create_info.ppEnabledLayerNames      = device_layer_names.data();
        device_create_info.enabledExtensionCount    = device_extension_names.size();
        device_create_info.ppEnabledExtensionNames  = device_extension_names.data();
        device_create_info.queueCreateInfoCount     = 1;
        device_create_info.pQueueCreateInfos        = &queue_create_info;
        device_create_info.pEnabledFeatures         = &device_features;
    }
    //Get Device from Physical Device
    VkDevice device = VK_NULL_HANDLE;
    VkResult result = vkCreateDevice(physical_device.physical_device, &device_create_info, nullptr, &device);
    assert(result == VK_SUCCESS && "Could not create device");
    //Get Device Queue
    VkQueue queue;
    vkGetDeviceQueue(device, physical_device.queue_index, 0, &queue);

    return { physical_device.physical_device, device, physical_device.queue_index, queue, physical_device.device_properties };
}

uint32_t ComputeEngine::GetQueueFamilyIndex(VkPhysicalDevice physical_device)
{
    //Get queue family count
    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, NULL);

    // Retrieve all queue families.
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

    // Now find a family that supports compute.
    uint32_t i = 0;
    while (i < queue_families.size())
    {
        //Check if this device supports a family which can do compute operations
        VkQueueFamilyProperties props = queue_families[i];
        if (props.queueCount > 0 && (props.queueFlags & VK_QUEUE_COMPUTE_BIT))
        {
            break;
        }
        i++;
    }

    if (i == queue_families.size())
        return -1; //The GPU does not support compute operations
    else
        return i; //Return the family of queue which can do compute operations
}

bool ComputeEngine::CheckDeviceSupport(VkPhysicalDevice physical_device)
{
    {//Layer names
        {
            //Get layers supported count
            uint32_t layer_count;
            vkEnumerateDeviceLayerProperties(physical_device, &layer_count, nullptr);

            //Get list of layer supported
            std::vector<VkLayerProperties> layer_properties(layer_count);
            vkEnumerateDeviceLayerProperties(physical_device, &layer_count, layer_properties.data());

            //Check if all required layers are supported
            for (uint32_t i = 0; i < device_layer_names.size(); i++)
            {
                std::string layer_name(device_layer_names[i]);
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
                    std::cout << "`" << layer_name + "` is not a support layer by device" << std::endl;
                    return false;
                }
            }
        }
    }

    {//Extension names
        {
            //Get extension supported count
            uint32_t ext_count;
            vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &ext_count, nullptr);

            //Get list of extensions supported
            std::vector<VkExtensionProperties> ext_properties(ext_count);
            vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &ext_count, ext_properties.data());

            //Check if all required extensions are supported
            for (uint32_t i = 0; i < device_extension_names.size(); i++)
            {
                std::string extension_name(device_extension_names[i]);
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
                    std::cout << "`" << extension_name + "` is not a support extension by device" << std::endl;
                    return false;
                }
            }
        }
    }

    return true;
}