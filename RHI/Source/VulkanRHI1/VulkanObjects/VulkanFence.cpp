#include <VulkanRHI1/VulkanObjects//VulkanFence.h>
#include <VulkanRHI1/VulkanObjects//VulkanDevice.h>
#include <VulkanRHI1/VulkanCommon.h>
#include <VulkanRHI1/Vulkan.h>
#include <stdexcept>

VulkanFence::VulkanFence(VulkanDevice *InDevice) 
	: Device(InDevice)
{	
	VkFenceCreateInfo CreateInfo{};
	CreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	CreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	VkResult Result = Device->CreateFence(&CreateInfo, nullptr, &Handle);
	if (Result != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create fence objects for a frame!");
	}
}

VulkanFence::~VulkanFence()
{
	if (Device)
		Device->DestroyFence(Handle, nullptr);
}

void VulkanFence::Wait()
{
	Device->WaitForFences(1, &Handle, VK_TRUE, UINT64_MAX);
}

VkResult VulkanFence::Reset()
{
	return Device->ResetFences(1, &Handle);
}

