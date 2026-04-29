#pragma once
#include <RHIBuffer.h>
#include <VulkanRHI1/VulkanObjects/VulkanDevice.h>
#include <VulkanRHI1/VulkanObjects/VulkanDeviceMemory.h>
#include <cstdint>
class VulkanBuffer : public RHIBuffer
{
public:
	VulkanBuffer();
	VulkanBuffer(VulkanDevice* Device, std::uint32_t InStride, std::uint64_t InSize, BufferUsageFlags Usage, const void* InData);
	~VulkanBuffer();
	VkBuffer GetHandle() const;
	VkDeviceMemory GetMemoryHandle();
	std::uint32_t GetOffset();
	virtual uint64_t GetDeviceAddress();
	void GetBufferDeviceAddress();
	VkIndexType GetIndexType();
	void UpdateData(std::uint32_t InSize, const void* InData);
public:
	VkResult BindBufferMemory(VkDeviceMemory Memory, VkDeviceSize MemoryOffset);
	VkResult MapMemory(VkDeviceSize Offset, VkDeviceSize Size, VkMemoryMapFlags Flags, void** Data);
	void UnmapMemory();
protected: 
	void setDescriptor(VkDeviceSize Size = VK_WHOLE_SIZE, VkDeviceSize Offset = 0)
	{
		Descriptor.buffer = Handle;
		Descriptor.range = Size;
		Descriptor.offset = Offset;
	}

	
private:public:
	VkBuffer Handle = VK_NULL_HANDLE;

	VkMemoryRequirements MemoryRequirements;
	//VulkanDeviceMemory DeviceMemoryHandle;
	VulkanDeviceMemory *DeviceMemory;
	VulkanDevice* Device = nullptr;
	std::uint64_t Offset = 0;
	std::uint64_t Size = 0; 
	VkDeviceAddress DeviceAddress = 0;
	VkDescriptorBufferInfo Descriptor;
};

/*
	Acceleration Structure Buffer
*/
class VulkanASBuffer
{
public:
	VulkanASBuffer(VulkanDevice* Device, std::uint32_t InSize);
	~VulkanASBuffer();
private:
	VulkanBuffer* Buffer = nullptr;
	VkAccelerationStructureKHR Handle = VK_NULL_HANDLE;
};