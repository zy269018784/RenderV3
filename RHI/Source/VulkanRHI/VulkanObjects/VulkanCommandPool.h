#pragma once
#include <VulkanRHI/Vulkan.h>
#include <VulkanRHI/VulkanObjects/VulkanDevice.h>
#include <VulkanRHI/VulkanObjects/VulkanPhysicalDevice.h>
#include <VulkanRHI/VulkanObjects/VulkanSurface.h>
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