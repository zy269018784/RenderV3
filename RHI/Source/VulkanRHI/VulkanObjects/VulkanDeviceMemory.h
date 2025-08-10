#pragma once
#include <VulkanRHI/Vulkan.h>
#include <VulkanRHI/VulkanObjects/VulkanDevice.h>
class VulkanDeviceMemory
{
public:
	VulkanDeviceMemory(VulkanDevice *InDevice);
	~VulkanDeviceMemory();
	VkDeviceMemory GetHandle() const;
	VkResult AllocateMemory(const VkMemoryAllocateInfo* AllocateInfo, const VkAllocationCallbacks* Allocator = nullptr);
	VkResult MapMemory(VkDeviceSize Offset, VkDeviceSize Size, VkMemoryMapFlags Flags, void** Data);
	void UnmapMemory();
private:
	VkDeviceMemory Handle = VK_NULL_HANDLE;
	VulkanDevice* Device;
};