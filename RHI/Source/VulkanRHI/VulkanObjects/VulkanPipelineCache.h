#pragma once
#include <VulkanRHI/Vulkan.h>
#include <VulkanRHI/VulkanObjects/VulkanDevice.h>

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