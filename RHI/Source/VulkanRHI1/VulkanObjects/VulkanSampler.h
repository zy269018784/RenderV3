#pragma once
#include <VulkanRHI1/Vulkan.h>
#include <VulkanRHI1/VulkanObjects/VulkanDevice.h>
#include <RHISampler.h>
#include <VulkanRHI1/VulkanCommon.h>


class VulkanSampler : public RHISampler {
public:
	VulkanSampler(VulkanDevice* InDevice);
	~VulkanSampler();
	virtual void create();

	virtual void setFilter(EFilter filter);
	virtual void setAddressMode(EAddressMode mode);
	virtual void setCompareOp(ECompareFunction cmp);


protected:
	void setDefault();

	inline VkFilter toVkFilter(EFilter filter) {
		switch (filter) {
		case NEAREST:
			return VK_FILTER_NEAREST;
		case LINEAR:
			return VK_FILTER_LINEAR;
		default:
			return VK_FILTER_LINEAR;
		}
	}

	inline VkSamplerAddressMode toVkAddressMode(EAddressMode mode) {
		switch (mode) {
		case REPEAT:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case MIRRORED_REPEAT :
			return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case CLAMP_TO_EDGE:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case CLAMP_TO_BORDER:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		case MIRRORED_CLAMP_TO_EDGE:
			return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		default:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		}
	}

public:
	VkSamplerCreateInfo CreateInfo = {};

	VkSampler Handle = VK_NULL_HANDLE;
	VulkanDevice* Device = nullptr;
};