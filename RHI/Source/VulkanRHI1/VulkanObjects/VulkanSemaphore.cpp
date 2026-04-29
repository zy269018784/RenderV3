#include <VulkanRHI1/VulkanObjects//VulkanSemaphore.h>
#include <VulkanRHI1/VulkanCommon.h>
#include <stdexcept>
#include <iostream>

VulkanSemaphore::VulkanSemaphore(VulkanDevice* InDevice)
	: Device(InDevice)
{
	if (Device)
	{
		VkSemaphoreCreateInfo CreateInfo{};
		CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		if (Device->CreateSemaphore(&CreateInfo, nullptr, &Handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create vkCreateSemaphore objects for a frame!");
		}
	}
}

VulkanSemaphore::~VulkanSemaphore()
{
	if (Device)
	{
		//std::cout << __FUNCTION__ << std::endl;
		Device->DestroySemaphore(Handle, nullptr);
	}
}