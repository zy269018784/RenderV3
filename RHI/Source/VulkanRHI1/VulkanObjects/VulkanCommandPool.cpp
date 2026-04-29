#include <VulkanRHI1/VulkanObjects/VulkanCommandPool.h>
#include <iostream>

VulkanCommandPool::VulkanCommandPool(VulkanDevice* InDevice, std::uint32_t InQueueFamilyIndex)
    : Device(InDevice)
{
    VkCommandPoolCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CreateInfo.queueFamilyIndex = InQueueFamilyIndex;
    VkResult Result = Device->CreateCommandPool(&CreateInfo, nullptr, &Handle);
    if (VK_SUCCESS != Result)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

VulkanCommandPool::~VulkanCommandPool()
{
   // std::cout << "DestroyCommandPool " << Handle << std::endl;
    Device->DestroyCommandPool(Handle, nullptr);
}

VkCommandPool VulkanCommandPool::GetHandle() const
{
    return Handle;
}

VkResult VulkanCommandPool::AllocateCommandBuffers(VkCommandBufferAllocateInfo* AllocateInfo, VkCommandBuffer* CommandBuffers)
{
    AllocateInfo->commandPool = Handle;
    std::cout << "VulkanCommandPool::AllocateCommandBuffers  1" << std::endl;
    VkResult Result = Device->AllocateCommandBuffers(AllocateInfo, CommandBuffers);
    if (VK_SUCCESS != Result)
    {
        std::cout << "VulkanCommandPool::AllocateCommandBuffers  error " << Result << std::endl;
        throw std::runtime_error("failed to allocate command buffers!");
    } 
    std::cout << "VulkanCommandPool::AllocateCommandBuffers  2" << std::endl;
}

void VulkanCommandPool::FreeCommandBuffers(std::uint32_t CommandBufferCount, const VkCommandBuffer* CommandBuffers)
{
    Device->FreeCommandBuffers(Handle, CommandBufferCount, CommandBuffers);
}