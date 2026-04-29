#pragma once
#include <VulkanRHI1/Vulkan.h>
class VulkanDevice;
class VulkanFenceManager;
class VulkanFence
{
public:
	VulkanFence(VulkanDevice *InDevice);
	~VulkanFence();  

	void Wait();

	VkResult Reset();

public:
	VkFence Handle; 
 
	VulkanDevice *Device = nullptr;
};