#include <VulkanRHI1/VulkanObjects/VulkanPipelineCache.h>
VulkanPipelineCache::VulkanPipelineCache()
	: Device(nullptr), Handle(VK_NULL_HANDLE)
{

}
VulkanPipelineCache::VulkanPipelineCache(VulkanDevice* Device)
	: Device(Device), Handle(VK_NULL_HANDLE)
{

}

VulkanPipelineCache::~VulkanPipelineCache()
{
	if (Device && VK_NULL_HANDLE != Handle)
		Device->DestroyPipelineCache(Handle, nullptr);
}