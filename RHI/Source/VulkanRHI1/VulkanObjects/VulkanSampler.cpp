#include <VulkanRHI1/VulkanObjects/VulkanSampler.h>
#include <iostream>
VulkanSampler::VulkanSampler(VulkanDevice* InDevice) {
	Device = InDevice; 
	setDefault();
}

void VulkanSampler::setDefault()
{
#if 0
	CreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	CreateInfo.magFilter = VK_FILTER_LINEAR;
	CreateInfo.minFilter = VK_FILTER_LINEAR;
	CreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	CreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	CreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	CreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	CreateInfo.maxAnisotropy = 1.0f;
	CreateInfo.anisotropyEnable = VK_FALSE;
	CreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
#else
	CreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	CreateInfo.magFilter = VK_FILTER_LINEAR;
	CreateInfo.minFilter = VK_FILTER_LINEAR;
	CreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	CreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	CreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	CreateInfo.anisotropyEnable = VK_TRUE;
	CreateInfo.maxAnisotropy = 16.0f;
	CreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	CreateInfo.unnormalizedCoordinates = VK_FALSE;
	CreateInfo.compareEnable = VK_FALSE;
	CreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	CreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	CreateInfo.mipLodBias = 0.0f;
	CreateInfo.minLod = 0.0f;
	CreateInfo.maxLod = 0.0f;
#endif
}

void VulkanSampler::create() 
{
	VkResult ret = Device->CreateSampler(&CreateInfo, nullptr, &Handle);
	std::cout << "CreateSampler " << ret << std::endl;
}

VulkanSampler::~VulkanSampler()
{
	Device->DestroySampler(Handle, nullptr);
}

void VulkanSampler::setFilter(EFilter filter) 
{
	CreateInfo.magFilter = toVkFilter(filter);
	CreateInfo.minFilter = CreateInfo.magFilter;
}

void VulkanSampler::setAddressMode(EAddressMode mode) 
{
	CreateInfo.addressModeU = toVkAddressMode(mode);
	CreateInfo.addressModeV = CreateInfo.addressModeU;
	CreateInfo.addressModeW = CreateInfo.addressModeU;
}

void VulkanSampler::setCompareOp(ECompareFunction cmp)
{
	CreateInfo.compareOp = toVkCompareOp(cmp);
}