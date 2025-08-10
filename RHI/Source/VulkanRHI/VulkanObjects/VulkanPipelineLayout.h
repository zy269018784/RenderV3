#pragma once
#include <VulkanRHI/Vulkan.h>
#include <VulkanRHI/VulkanObjects/VulkanDevice.h>
#include <VulkanRHI/VulkanObjects/VulkanDescriptorSetLayout.h>

class VulkanPipelineLayout
{
public:
	VulkanPipelineLayout(VulkanDevice* Device = nullptr);
	VulkanPipelineLayout(VulkanDevice* InDevice, const std::vector<VulkanDescriptorSetLayout*>& descSetLayouts, const std::vector<VkPushConstantRange>& pushConstants)
	{
		Device = InDevice;
		init(descSetLayouts, pushConstants);
	}

	bool init(const std::vector<VulkanDescriptorSetLayout*>& descSetLayouts, const std::vector<VkPushConstantRange>& pushConstants)
	{ 
		//std::vector<VkDescriptorSetLayout> layoutHandles(descSetLayouts.size());
		//for (size_t i = 0; i < descSetLayouts.size(); ++i) {
		//	layoutHandles[i] = descSetLayouts[i]->Handle;
		//}

		//VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		//pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		//pipelineLayoutInfo.pNext = nullptr;
		//pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layoutHandles.size());
		//pipelineLayoutInfo.pSetLayouts = layoutHandles.data();
		//pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
		//pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();
		//return vkCreatePipelineLayout(Device->Handle, &pipelineLayoutInfo, nullptr, &Handle) == VK_SUCCESS;
	}

	~VulkanPipelineLayout();
public:
	VkPipelineLayout Handle;
	VulkanDevice* Device;
};