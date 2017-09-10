void ComputeEngine::DestroyDescriptorSet(ComputeEngine::SupportedDevice supported_device, ComputeEngine::VulkanDescriptor descriptor)
{
    vkDestroyDescriptorPool(supported_device.device, descriptor.descriptor_pool, NULL);
    vkDestroyDescriptorSetLayout(supported_device.device, descriptor.descriptor_layout, NULL);
}

ComputeEngine::VulkanDescriptor ComputeEngine::CreateDescriptorSet(ComputeEngine::SupportedDevice supported_device, ComputeEngine::VulkanBuffer vulkan_buffer, VkDescriptorType descriptor_type)
{
    //Setup descriptor set layout binding information
    VkDescriptorSetLayoutBinding descriptor_layout_binding = {};
    {
        descriptor_layout_binding.binding               = 0; // binding = 0
        descriptor_layout_binding.descriptorType        = descriptor_type;
        descriptor_layout_binding.descriptorCount       = 1;
        descriptor_layout_binding.stageFlags            = VK_SHADER_STAGE_COMPUTE_BIT;
    }

    //Setup descriptor set create info using descriptor set layout binding
    VkDescriptorSetLayoutCreateInfo descriptor_layout_create_info = {};
    {
        descriptor_layout_create_info.sType             = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_layout_create_info.bindingCount      = 1;
        descriptor_layout_create_info.pBindings         = &descriptor_layout_binding; 
    }

    //Create descriptor set layout
    VkDescriptorSetLayout descriptor_layout;
    VkResult result = vkCreateDescriptorSetLayout(supported_device.device, &descriptor_layout_create_info, NULL, &descriptor_layout);
    assert(result == VK_SUCCESS && "Could not create vulkan descriptor set layout");


    //Setup descriptor pool size information
    VkDescriptorPoolSize descriptor_pool_size = {};
    {
        descriptor_pool_size.type                       = descriptor_type;
        descriptor_pool_size.descriptorCount            = 1;
    }
    //Create descriptor pool create information
    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
    {
        descriptor_pool_create_info.sType               = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_pool_create_info.maxSets             = 1; // we only need to allocate one descriptor set from the pool.
        descriptor_pool_create_info.poolSizeCount       = 1;
        descriptor_pool_create_info.pPoolSizes          = &descriptor_pool_size;
    }

    //Create descriptor pool
    VkDescriptorPool descriptor_pool;
    result = vkCreateDescriptorPool(supported_device.device, &descriptor_pool_create_info, NULL, &descriptor_pool);
    assert(result == VK_SUCCESS && "Could not create descriptor pool");

    //Setup info for descriptor set allocation
    VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
    {
        descriptor_set_allocate_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO; 
        descriptor_set_allocate_info.descriptorPool     = descriptor_pool; // pool to allocate from.
        descriptor_set_allocate_info.descriptorSetCount = 1; // allocate a single descriptor set.
        descriptor_set_allocate_info.pSetLayouts        = &descriptor_layout;
    }

    //Allocate descriptor sets
    VkDescriptorSet descriptor_set;
    result = vkAllocateDescriptorSets(supported_device.device, &descriptor_set_allocate_info, &descriptor_set);
    assert(result == VK_SUCCESS && "Could not create vulkan descriptor sets");

    //Bind buffer and descriptor
    VkDescriptorBufferInfo descriptor_buffer_info = {};
    {
        descriptor_buffer_info.buffer                   = vulkan_buffer.buffer;
        descriptor_buffer_info.offset                   = 0;
        descriptor_buffer_info.range                    = vulkan_buffer.buffer_size;
    }

    //Create descriptor set info for updating descriptor sets
    VkWriteDescriptorSet write_descriptor_set = {};
    {
        write_descriptor_set.sType                      = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_set.dstSet                     = descriptor_set; // write to this descriptor set.
        write_descriptor_set.dstBinding                 = 0; // write to the first, and only binding.
        write_descriptor_set.descriptorCount            = 1; // update a single descriptor.
        write_descriptor_set.descriptorType             = descriptor_type; // storage buffer.
        write_descriptor_set.pBufferInfo                = &descriptor_buffer_info;
    }

    //Update descriptor sets with information provided
    vkUpdateDescriptorSets(supported_device.device, 1, &write_descriptor_set, 0, NULL);

    return { &vulkan_buffer, descriptor_layout, descriptor_pool, descriptor_set };
}