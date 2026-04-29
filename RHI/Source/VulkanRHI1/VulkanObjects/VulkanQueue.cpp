#include <VulkanRHI1/VulkanObjects//VulkanQueue.h>
#include <VulkanRHI1/VulkanObjects/VulkanFence.h>
#include <VulkanRHI1/VulkanObjects/VulkanCommandBuffer.h>
#include <VulkanRHI1/VulkanObjects/VulkanSemaphore.h>

VulkanQueue::VulkanQueue()

{

}

VulkanQueue::~VulkanQueue()
{

}

void VulkanQueue::Init(VulkanDevice* InDevice, uint32_t InfamilyIndex)
{
	Device = InDevice;
	FamilyIndex = InfamilyIndex;
	Device->GetDeviceQueue(InfamilyIndex, 0, &Handle);
}

void VulkanQueue::Submit(VulkanCommandBuffer* CmdBuffer, VulkanFence* Fence)
{
	if (Fence) 
	{
		Fence->Wait();
		Fence->Reset();
	} 

	VkCommandBuffer CommandBuffer = CmdBuffer->GetHandle();

    VkSubmitInfo SubmitInfo = {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffer;
	QueueSubmit(1, &SubmitInfo, Fence == nullptr ? VK_NULL_HANDLE : Fence->Handle);
}

void VulkanQueue::SubmitSync(VulkanCommandBuffer* CmdBuffer, VulkanSemaphore* pWaitSemaphore,
	VkPipelineStageFlags waitDstStageMask,
	VulkanSemaphore* pSignalSemaphore, VulkanFence* fence)
{
	VkCommandBuffer CommandBuffer = CmdBuffer->GetHandle();

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO; 
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CommandBuffer;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &pWaitSemaphore->Handle;
	submitInfo.pWaitDstStageMask = &waitDstStageMask;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &pSignalSemaphore->Handle;
	vkQueueSubmit(Handle, 1, &submitInfo, fence == nullptr ? VK_NULL_HANDLE : fence->Handle);
}

//VkResult VulkanQueue::Present(VkPresentInfoKHR &info)
//{
//	return vkQueuePresentKHR(Handle, &info);
//}
#include <VulkanRHI1/VulkanObjects/VulkanSwapChain.h>
VkResult VulkanQueue::Present(VulkanSwapChain* pSwapChain, VulkanSemaphore* pSemaphore, uint32_t CurImageIndex)
{
	VkSemaphore signalSemaphores[] = { pSemaphore->Handle };
	VkSwapchainKHR swapChains[] = { pSwapChain->Handle };
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &CurImageIndex;
	return vkQueuePresentKHR(Handle, &presentInfo);
}

VkResult VulkanQueue::QueueSubmit(std::uint32_t SubmitCount, const VkSubmitInfo* Submits, VkFence Fence)
{
	return vkQueueSubmit(Handle, SubmitCount, Submits, Fence);
}

VkResult VulkanQueue::QueueSubmit2(std::uint32_t SubmitCount, const VkSubmitInfo2* Submits, VkFence Fence)
{
	return vkQueueSubmit2(Handle, SubmitCount, Submits, Fence);
}

VkResult VulkanQueue::WaitIdle()
{
	return vkQueueWaitIdle(Handle);
}