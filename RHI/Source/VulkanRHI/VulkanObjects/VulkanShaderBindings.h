#pragma once
#include <VulkanRHI/Vulkan.h> 
#include <VulkanRHI/VulkanObjects/VulkanSampler.h>
#include <VulkanRHI/VulkanObjects/VulkanDevice.h>
#include <RHITexture.h>
#include <RHIShaderBindings.h>


class VulkanShaderBindings : public RHIShaderBindings
{
public:
	VulkanShaderBindings() = default;
	VulkanShaderBindings(VulkanDevice* InDevice);
	~VulkanShaderBindings();
	VkDescriptorSet GetDescriptorSet();
	VkDescriptorSetLayout GetDescriptorSetLayout();

	virtual void AddBinding(RHIShaderBinding& InBinding);
	virtual void UpdateBinding(uint32_t InBinding, RHIShaderBinding& InShaderBinding);
	virtual void SetBindings(std::vector<RHIShaderBinding> InBindings);
	virtual void Create();
	virtual void Update();

protected:	
	void CreateDescriptorPool();
	void CreateDescriptorSetLayout();
	void AllocateDescriptorSets(); 
	void UpdateDescriptorSet(); 
	
private:
	VkImageView GetImageViewFromTexture(RHITexture* pTexure);
	static VkDescriptorType toVkDescriptorType(RHIShaderBinding::EType type);
private:
	std::vector<RHIShaderBinding >	Bindings;
	VkDescriptorSet					DescriptorSet = VK_NULL_HANDLE;
	VkDescriptorPool				DescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout			DescSetLayout = VK_NULL_HANDLE;
	VulkanDevice*					Device = nullptr;
};