#pragma once
#include <vector>
class VulkanCommandBuffer;
class VulkanCommandBufferPool
{
public:
	void RefreshFenceStatus(VulkanCommandBuffer* SkipCmdBuffer = nullptr);

	std::vector<VulkanCommandBuffer*> CmdBuffers;
	std::vector<VulkanCommandBuffer*> FreeCmdBuffers;
};