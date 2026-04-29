#include <VulkanRHI1/VulkanObjects/VulkanPipelineLayout.h>

VulkanPipelineLayout::VulkanPipelineLayout(VulkanDevice* Device)
	: Device(Device)
{

}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
	if (Device && VK_NULL_HANDLE != Handle)
		Device->DestroyPipelineLayout(Handle, nullptr);
}