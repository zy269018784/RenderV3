#pragma once
#include <VulkanRHI1/Vulkan.h>
#include <cstdint>
#include <VulkanRHI1/VulkanObjects/VulkanDevice.h>

class VulkanCommandBuffer;
class VulkanSemaphore;
class VulkanSwapChain;
class VulkanFence;

class VulkanQueue
{
public:
	VulkanQueue();
	~VulkanQueue();
public:
	void Init(VulkanDevice* InDevice, uint32_t familyIndex);


	void Submit(VulkanCommandBuffer* pCmdBuffer, VulkanFence* fence);
	//void Submit(VulkanCommandBuffer* pCmdBuffer, VulkanFence* fence);	
	
	//void SubmitSync(VulkanCommandBuffer* pCmdBuffer, uint32_t NumSignalSemaphores = 0, VkSemaphore* SignalSemaphores = nullptr);
	void SubmitSync(VulkanCommandBuffer* pCmdBuffer, VulkanSemaphore* pWaitSemaphore,
		VkPipelineStageFlags waitDstStageMask,
		VulkanSemaphore* pSignalSemaphore, VulkanFence* fence);

	//VkResult Present(VkPresentInfoKHR &info);
	VkResult Present(VulkanSwapChain* pSwapChain, VulkanSemaphore* pSemaphore, uint32_t CurImageIndex);
	 	
	VkResult WaitIdle();
public:
	VkResult QueueSubmit(std::uint32_t SubmitCount, const VkSubmitInfo* Submits, VkFence Fence);
	VkResult QueueSubmit2(std::uint32_t SubmitCount,const VkSubmitInfo2* Submits, VkFence Fence);

public:
	VkQueue Handle;
	uint32_t FamilyIndex;
	VulkanDevice* Device = nullptr;
};
