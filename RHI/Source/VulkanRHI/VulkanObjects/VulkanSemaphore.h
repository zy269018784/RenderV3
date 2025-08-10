#pragma once
#include <VulkanRHI/VulkanObjects//VulkanDevice.h>

class VulkanSemaphore
{
public:
	VulkanSemaphore(VulkanDevice* InDevice);
	virtual ~VulkanSemaphore();
public: 
	VkSemaphore Handle; 
private:
	VulkanDevice* Device = nullptr;
};