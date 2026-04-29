#pragma once
#include <VulkanRHI1/Vulkan.h>
#include <VulkanRHI1/VulkanObjects/VulkanDevice.h>
#include <VulkanRHI1/VulkanObjects/VulkanPhysicalDevice.h>
#include <VulkanRHI1/VulkanObjects/VulkanSurface.h>
#include <vector>
#include <stdexcept>
#include <optional>

class VulkanCommandPool
{
public:
    VulkanCommandPool() = default;
    VulkanCommandPool(VulkanDevice* InDevice, std::uint32_t InQueueFamilyIndex);
    ~VulkanCommandPool();
    VkCommandPool GetHandle() const;

public:
    VkResult AllocateCommandBuffers(VkCommandBufferAllocateInfo* AllocateInfo, VkCommandBuffer* CommandBuffers);
    void FreeCommandBuffers(std::uint32_t CommandBufferCount, const VkCommandBuffer* CommandBuffers);
private:
    VkCommandPool Handle;
public:
    VulkanDevice* Device = nullptr;
};