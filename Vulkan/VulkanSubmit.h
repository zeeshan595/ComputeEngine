#ifndef _VULKAN_SUBMIT
#define _VULKAN_SUBMIT

namespace ComputeEngine
{
    void SubmitCommand(SupportedDevice supported_devices, VulkanCommandBuffer command_buffer, VkFence fence)
    {
        //Setup submit info
        VkSubmitInfo submit_info = {};
        {
            submit_info.sType                   = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submit_info.commandBufferCount      = 1; // submit a single command buffer
            submit_info.pCommandBuffers         = &command_buffer.command_buffer; // the command buffer to submit.
        }

        //Submit command to GPU
        VkResult result = vkQueueSubmit(supported_devices.queue, 1, &submit_info, fence);
        assert(result == VK_SUCCESS);
    }
};

#endif