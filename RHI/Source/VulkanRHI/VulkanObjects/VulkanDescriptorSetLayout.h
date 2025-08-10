#pragma once
#include <VulkanRHI/Vulkan.h>
#include <VulkanRHI/VulkanObjects/VulkanDevice.h>
#include <vector>
#include <map>
#include <cstdint>
 
class VulkanDescriptorSetLayout
{

public:
	//VulkanDescriptorSetLayout();
	VulkanDescriptorSetLayout(VulkanDevice* Device = nullptr);
	~VulkanDescriptorSetLayout();

	void addBinding(VkShaderStageFlags stageFlags, uint32_t bindingPoint, uint32_t descCount, VkDescriptorType descType, VkDescriptorBindingFlags bindingFlags) {
		VkDescriptorSetLayoutBinding bindingInfo = {};
		bindingInfo.stageFlags = stageFlags;
		bindingInfo.binding = bindingPoint;
		bindingInfo.descriptorCount = descCount;
		bindingInfo.descriptorType = descType;
		bindingInfo.pImmutableSamplers = nullptr; 
		Bindings.emplace_back(std::move(bindingInfo));

		BindingFlags.emplace_back(std::move(bindingFlags)); 
	}
 

	void create();

public:
	VkDescriptorSetLayout Handle; 
	
	std::vector<VkDescriptorSetLayoutBinding> Bindings;
	std::vector<VkDescriptorBindingFlags> BindingFlags; 
  
	VulkanDevice* Device;
};