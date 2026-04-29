#pragma once
#include <VulkanRHI1/Vulkan.h>
#include <VulkanRHI1/VulkanObjects/VulkanDevice.h>
#include <VulkanRHI1/VulkanObjects/VulkanDescriptorSetLayout.h>
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