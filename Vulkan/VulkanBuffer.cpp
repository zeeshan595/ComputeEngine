void ComputeEngine::DestroyBuffer(ComputeEngine::SupportedDevice supported_device, ComputeEngine::VulkanBuffer vulkan_buffer)
{
    vkFreeMemory(supported_device.device, vulkan_buffer.device_memory, nullptr);
    vkDestroyBuffer(supported_device.device, vulkan_buffer.buffer, nullptr);
}

//VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
ComputeEngine::VulkanBuffer ComputeEngine::CreateBuffer(ComputeEngine::SupportedDevice supported_device, uint32_t buffer_size, VkMemoryPropertyFlags memory_properties)
{
    //Setup buffer create info
    VkBufferCreateInfo buffer_create_info = {};
    {
        buffer_create_info.sType            = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_create_info.size             = buffer_size;
        buffer_create_info.usage            = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        buffer_create_info.sharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    }

    //Create buffer
    VkBuffer buffer = VK_NULL_HANDLE;
    VkResult result = vkCreateBuffer(supported_device.device, &buffer_create_info, NULL, &buffer);
    //Error checking
    assert(result == VK_SUCCESS && "Could not create buffer");

    //Check memory requirements
    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(supported_device.device, buffer, &memory_requirements);

    //Setup buffer memeory allocation
    VkMemoryAllocateInfo allocate_info = {};
    {
        allocate_info.sType                 = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocate_info.allocationSize        = memory_requirements.size;
        allocate_info.memoryTypeIndex       = GetMemoryType(supported_device.physical_device, memory_requirements.memoryTypeBits, memory_properties);
    }

    //Allocate appropriate memory on device
    VkDeviceMemory device_memory;
    result = vkAllocateMemory(supported_device.device, &allocate_info, NULL, &device_memory);
    assert(result == VK_SUCCESS && "Could not allocate memory on device");

    //Bind allocated memory with memory handle
    result = vkBindBufferMemory(supported_device.device, buffer, device_memory, 0);
    assert(result == VK_SUCCESS && "Could not bind memory handle with allocated memory");

    return { buffer_size, buffer, device_memory };
}

uint32_t ComputeEngine::GetMemoryType(VkPhysicalDevice physical_device, uint32_t memory_type_bits, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
    {
        if ((memory_type_bits & (1 << i)) && ((memory_properties.memoryTypes[i].propertyFlags & properties) == properties))
            return i;
    }
    assert(0 && "Memory properties are not supported by this device");
    return -1;
}