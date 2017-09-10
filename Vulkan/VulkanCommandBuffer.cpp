void ComputeEngine::DestroyCommandBuffer(ComputeEngine::SupportedDevice supported_device, ComputeEngine::VulkanCommandBuffer command_buffer)
{
    vkFreeCommandBuffers(supported_device.device, command_buffer.command_pool, 1, &command_buffer.command_buffer);
    vkDestroyCommandPool(supported_device.device, command_buffer.command_pool, nullptr);
}

ComputeEngine::VulkanCommandBuffer ComputeEngine::CreateCommandBuffer(
    ComputeEngine::SupportedDevice supported_device,
    ComputeEngine::VulkanPipeline pipeline,
    ComputeEngine::VulkanDescriptor descriptor,
    uint32_t work_group_x, uint32_t work_group_y, uint32_t work_group_z
){
    //Create command pool info
    VkCommandPoolCreateInfo command_pool_info = {};
    {
        command_pool_info.sType                         = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        command_pool_info.flags                         = 0;
        command_pool_info.queueFamilyIndex              = supported_device.queue_index;
    }

    //Create command pool
    VkCommandPool command_pool;
    VkResult result = vkCreateCommandPool(supported_device.device, &command_pool_info, NULL, &command_pool);
    assert(result == VK_SUCCESS && "Could not create command pool");

    //Setup command buffer info using command pool
    VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
    {
        command_buffer_allocate_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_allocate_info.commandPool        = command_pool; // specify the command pool to allocate from. 
        command_buffer_allocate_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        command_buffer_allocate_info.commandBufferCount = 1; // allocate a single command buffer. 
    }
    //Allocate command buffer
    VkCommandBuffer command_buffer;
    result = vkAllocateCommandBuffers(supported_device.device, &command_buffer_allocate_info, &command_buffer);
    assert(result == VK_SUCCESS && "Could not allocate command buffers");

    //Setup command buffer begin info to use allocated command buffer
    VkCommandBufferBeginInfo begin_info = {};
    {
        begin_info.sType                                = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags                                = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // the buffer is only submitted and used once in this application.
    }
    //Begin command buffer
    result = vkBeginCommandBuffer(command_buffer, &begin_info); // start recording commands.
    assert(result == VK_SUCCESS && "Could not begin command buffer");

    //Bind pipeline and descriptor to command buffer
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.pipeline);
    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.pipeline_layout, 0, 1, &descriptor.descriptor_set, 0, NULL);

    //Dispatch command and give work group size
    vkCmdDispatch(command_buffer, work_group_x, work_group_y, work_group_z);

    //End command buffer
    result = vkEndCommandBuffer(command_buffer);
    assert(result == VK_SUCCESS && "Could not end command buffer");

    return { command_pool, command_buffer  };
}