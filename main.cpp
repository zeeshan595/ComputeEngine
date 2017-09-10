#include <vulkan/vulkan.hpp>
#include <iostream>
#include <assert.h>
#include <vector>

#include "lodepng.h"

#include "Vulkan/VulkanInstance.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanBuffer.h"
#include "Vulkan/VulkanDescriptor.h"
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanCommandBuffer.h"
#include "Vulkan/VulkanFence.h"
#include "Vulkan/VulkanSubmit.h"

struct Pixel
{
    float r, g, b, a;
};
uint32_t WIDTH = 3200;
uint32_t HEIGHT = 2400;
uint32_t work_groups = 32;

void SaveRenderedImage(uint32_t bufferSize, VkDevice device, VkDeviceMemory bufferMemory);

int main()
{
    //Init Vulkan
    ComputeEngine::VulkanInstance instance = ComputeEngine::CreateVulkanInstance();
    //Init Compute Devices (GPUs)
    std::vector<ComputeEngine::SupportedDevice> gpus = ComputeEngine::CreateDevices(instance.vulkan_instance);


    std::cout << "Supported GPUs: " << std::endl;
    for (int i = 0; i < gpus.size(); i++)
    {
        std::cout << "      " << gpus[i].device_properties.deviceName << std::endl;
    }

    //Create buffer on 1st GPU
    ComputeEngine::VulkanBuffer buffer_object =  ComputeEngine::CreateBuffer(
        gpus[0],
        sizeof(Pixel) * WIDTH * HEIGHT, //buffer size
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    //Create descriptor layout so GPU knows how to handle buffer
    ComputeEngine::VulkanDescriptor descriptor_set = ComputeEngine::CreateDescriptorSet(gpus[0], buffer_object, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

    //Create pipeline
    ComputeEngine::VulkanPipeline pipeline = ComputeEngine::CreatePipeline(gpus[0], descriptor_set, "comp.spv");

    //Create command buffer
    ComputeEngine::VulkanCommandBuffer command_buffer = ComputeEngine::CreateCommandBuffer(
        gpus[0], pipeline, descriptor_set,
        (uint32_t)ceil(WIDTH / float(work_groups)), (uint32_t)ceil(HEIGHT / float(work_groups)), 1 //gpu work groups x,y,z
    );

    //Create fence
    VkFence fence = ComputeEngine::CreateFence(gpus[0]);

    //Submit command buffer to gpu so it can be computed
    ComputeEngine::SubmitCommand(gpus[0], command_buffer, fence);

    //Use fence to wait for gpu command to finish
    vkWaitForFences(gpus[0].device, 1, &fence, VK_TRUE, 1000000000000);

    //Save Rendered Image
    SaveRenderedImage(sizeof(Pixel) * 3200 * 2400, gpus[0].device, buffer_object.device_memory);

    //Destroy fence
    ComputeEngine::DestroyFence(gpus[0], fence);

    //Destroy command buffer
    ComputeEngine::DestroyCommandBuffer(gpus[0], command_buffer);

    //Destroy pipeline
    ComputeEngine::DestroyPipeline(gpus[0], pipeline);

    //Descript descriptor sets
    ComputeEngine::DestroyDescriptorSet(gpus[0], descriptor_set);

    //Destroy Buffer
    ComputeEngine::DestroyBuffer(gpus[0], buffer_object);

    //De-init Compute Devices (GPUs)
    ComputeEngine::DestroyDevices(gpus);
    //De-init Vulkan
    ComputeEngine::DestroyVulkanInstance(instance);

    std::cin.get();
    return 0;
}

void SaveRenderedImage(uint32_t bufferSize, VkDevice device, VkDeviceMemory bufferMemory)
{
    void* mappedMemory = NULL;
    // Map the buffer memory, so that we can read from it on the CPU.
    vkMapMemory(device, bufferMemory, 0, bufferSize, 0, &mappedMemory);
    Pixel* pmappedMemory = (Pixel *)mappedMemory;

    // Get the color data from the buffer, and cast it to bytes.
    // We save the data to a vector.
    std::vector<unsigned char> image;
    image.reserve(WIDTH * HEIGHT * 4);
    for (int i = 0; i < WIDTH*HEIGHT; i += 1) {
        image.push_back((unsigned char)(255.0f * (pmappedMemory[i].r)));
        image.push_back((unsigned char)(255.0f * (pmappedMemory[i].g)));
        image.push_back((unsigned char)(255.0f * (pmappedMemory[i].b)));
        image.push_back((unsigned char)(255.0f * (pmappedMemory[i].a)));
    }
    // Done reading, so unmap.
    vkUnmapMemory(device, bufferMemory);

    // Now we save the acquired color data to a .png.
    unsigned error = lodepng::encode("mandelbrot.png", image, WIDTH, HEIGHT);
    if (error) printf("encoder error %d: %s", error, lodepng_error_text(error));
}