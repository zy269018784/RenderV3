#pragma once
#include <VulkanRHI/Vulkan.h>
#include <VulkanRHI/VulkanObjects/VulkanDevice.h>
#include <VulkanRHI/VulkanObjects/VulkanDescriptorSetLayout.h>
class VulkanDescriptorPool
{
public:				
	VulkanDescriptorPool() {};
	VulkanDescriptorPool(VulkanDevice* Device, VulkanDescriptorSetLayout* Layout, std::uint32_t MaxSetsAllocations = 1);
	~VulkanDescriptorPool();
public:
	VkDescriptorPool Handle;
	VulkanDevice *Device;
	std::uint32_t MaxDescriptorSets;
};