void ComputeEngine::DestroyPipeline(ComputeEngine::SupportedDevice supported_device, ComputeEngine::VulkanPipeline pipeline)
{
    vkDestroyShaderModule(supported_device.device, pipeline.shader_module, nullptr);
    vkDestroyPipelineLayout(supported_device.device, pipeline.pipeline_layout, nullptr);
    vkDestroyPipeline(supported_device.device, pipeline.pipeline, nullptr);
}

ComputeEngine::VulkanPipeline ComputeEngine::CreatePipeline(ComputeEngine::SupportedDevice supported_device, ComputeEngine::VulkanDescriptor descriptor, const char* shader_path)
{
    //Get shader code from file
    uint32_t file_length;
    uint32_t* code = ReadShaderFile(file_length, shader_path);
    //Setup shader module create info
    VkShaderModuleCreateInfo shader_module_info = {};
    {
        shader_module_info.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_info.pCode                    = code;
        shader_module_info.codeSize                 = file_length;
    }

    //Create shader module
    VkShaderModule compute_shader_module;
    VkResult result = vkCreateShaderModule(supported_device.device, &shader_module_info, NULL, &compute_shader_module);
    assert(result == VK_SUCCESS && "Could not create shader module");
    //Delete shader code stored in memory
    delete[] code;

    //Setup shader stage create info
    VkPipelineShaderStageCreateInfo shader_stage_info = {};
    {
        shader_stage_info.sType                     = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage_info.stage                     = VK_SHADER_STAGE_COMPUTE_BIT;
        shader_stage_info.module                    = compute_shader_module;
        shader_stage_info.pName                     = "main";
    }

    //Setup pipeline layout using descriptor
    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    {
        pipeline_layout_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount         = 1;
        pipeline_layout_info.pSetLayouts            = &descriptor.descriptor_layout; 
    }

    //Create pipeline layout
    VkPipelineLayout pipeline_layout;
    result = vkCreatePipelineLayout(supported_device.device, &pipeline_layout_info, NULL, &pipeline_layout);
    assert(result == VK_SUCCESS && "Could not create pipeline layout");

    //Setup pipeline create info using shader stage create info
    VkComputePipelineCreateInfo pipeline_create_info = {};
    {
        pipeline_create_info.sType                  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipeline_create_info.stage                  = shader_stage_info;
        pipeline_create_info.layout                 = pipeline_layout;
    }

    //Create compute pipeline
    VkPipeline pipeline;
    result = vkCreateComputePipelines(supported_device.device, VK_NULL_HANDLE, 1, &pipeline_create_info, NULL, &pipeline);
    assert(result == VK_SUCCESS && "Could not create vulkan pipeline");

    return { compute_shader_module, pipeline_layout, pipeline };
}

uint32_t* ComputeEngine::ReadShaderFile(uint32_t& length, const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    assert(fp != NULL && "Could not find or open shader file");

    //Get file size
    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    long filesizepadded = long(ceil(filesize / 4.0)) * 4;

    //Read file contents
    char *str = new char[filesizepadded];
    fread(str, filesize, sizeof(char), fp);
    fclose(fp);

    //Data padding
    for (int i = filesize; i < filesizepadded; i++)
    {
        str[i] = 0;
    }

    length = filesizepadded;
    return (uint32_t *)str;
}