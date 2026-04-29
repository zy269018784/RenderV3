#include "VulkanDeviceMemory.h"

VulkanDeviceMemory::VulkanDeviceMemory(VulkanDevice* InDevice)
	: Device(InDevice)
{

}


VulkanDeviceMemory::~VulkanDeviceMemory()
{
	if (Device)
		Device->FreeMemory(Handle, nullptr);
}

VkDeviceMemory VulkanDeviceMemory::GetHandle() const
{
	return Handle;
}

VkResult VulkanDeviceMemory::AllocateMemory(const VkMemoryAllocateInfo* AllocateInfo, const VkAllocationCallbacks* Allocator)
{
	return Device->AllocateMemory(AllocateInfo, Allocator, &Handle);
}

VkResult VulkanDeviceMemory::MapMemory(VkDeviceSize Offset, VkDeviceSize Size, VkMemoryMapFlags Flags, void** Data)
{
	return Device->MapMemory(Handle, Offset, Size, Flags, Data);
}

void VulkanDeviceMemory::UnmapMemory()
{
	Device->UnmapMemory(Handle);
}