#pragma once
#include <VulkanRHI1/Vulkan.h>
#include <VulkanRHI1/VulkanObjects/VulkanDevice.h>

class VulkanPipelineCache
{
public:
	VulkanPipelineCache();
	VulkanPipelineCache(VulkanDevice* Device);
	~VulkanPipelineCache();
public:
	VkPipelineCache Handle = VK_NULL_HANDLE;
	VulkanDevice* Device;
};