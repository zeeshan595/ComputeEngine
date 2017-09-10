void ComputeEngine::DestroyFence(ComputeEngine::SupportedDevice supported_device, VkFence fence)
{
    vkDestroyFence(supported_device.device, fence, nullptr);
}
VkFence ComputeEngine::CreateFence(ComputeEngine::SupportedDevice supported_device)
{
    //Setup fence create info
    VkFenceCreateInfo fence_create_info = {};
    {
        fence_create_info.sType     = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_create_info.flags     = 0;
    }

    //Create fence
    VkFence fence;
    VkResult result = vkCreateFence(supported_device.device, &fence_create_info, NULL, &fence);
    assert(result == VK_SUCCESS);

    return fence;
}