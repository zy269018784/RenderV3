#include <VulkanRHI1/Vulkan.h>
#include <VulkanRHI1/VulkanObjects//VulkanBuffer.h>
#include <VulkanRHI1/VulkanLoader.h>
#include <string>
#include <cstring>
#include <iostream>

VulkanBuffer::VulkanBuffer() 
    : Device(nullptr), Handle(VK_NULL_HANDLE)
{
 
} 

VulkanBuffer::VulkanBuffer(VulkanDevice* Device, std::uint32_t InStride, std::uint64_t InSize, BufferUsageFlags InUsage, const void* InData)
    : Device(Device)
    , RHIBuffer(InSize, InUsage, InStride)
{
    bool useDeviceAddress = false;

    VkBufferUsageFlags UsageFlags = 0x0;
    if (EnumHasAnyFlags(InUsage, BUF_VertexBuffer))
    {
        UsageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }

    if (EnumHasAnyFlags(InUsage, BUF_IndexBuffer)) {
        UsageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }

    if (EnumHasAnyFlags(InUsage, BUF_StorageBuffer))
    {
        UsageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }

    if (EnumHasAnyFlags(InUsage, BUF_UniformBuffer)) {
        UsageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }

    if (EnumHasAnyFlags(InUsage, BUF_IndrectBuffer)) {
        UsageFlags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    }

    if (EnumHasAnyFlags(InUsage, BUF_TransferSrcBuffer)) {
        UsageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }

    if (EnumHasAnyFlags(InUsage, BUF_TransferDstBuffer)) {
        UsageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }

    if (EnumHasAnyFlags(InUsage, BUF_AccelerationStructure)) {
        UsageFlags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
        UsageFlags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        if (InData) {
            UsageFlags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
        }
        useDeviceAddress = true;
    }

    if (EnumHasAnyFlags(InUsage, BUF_ShaderBindingTable)) {
        UsageFlags |= VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;
        UsageFlags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT; 
        useDeviceAddress = true;
    }

    if (EnumHasAnyFlags(InUsage, BUF_RayTracingScratch)) {
        UsageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        UsageFlags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        useDeviceAddress = true;
    }

    VkResult err = VK_SUCCESS;

    VkBufferCreateInfo CreateInfo;
    std::memset(&CreateInfo, 0, sizeof(CreateInfo));
    CreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    CreateInfo.size = InSize;
    CreateInfo.usage = UsageFlags;

    VkAllocationCallbacks* Allocator = nullptr;

    err = Device->CreateBuffer(&CreateInfo, Allocator, &Handle);
    if (err != VK_SUCCESS)
        std::cout << "Failed to create buffer: " << err;

    Device->GetBufferMemoryRequirements(Handle, &MemoryRequirements);

    VkMemoryAllocateInfo MemoryAllocateInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        MemoryRequirements.size,
        Device->HostVisibleMemoryTypeIndexWithMaxHeapSize
    };

    if (EnumHasAnyFlags(InUsage, BUF_RayTracingScratch)) {
        MemoryAllocateInfo.memoryTypeIndex = Device->DeviceLocalMemoryTypeIndexWithMaxHeapSize;
    }

    // If the buffer has VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT set we also need to enable the appropriate flag during allocation
    VkMemoryAllocateFlagsInfoKHR allocFlagsInfo{};
    if ( useDeviceAddress ) {
        allocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
        allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
        MemoryAllocateInfo.pNext = &allocFlagsInfo;
    } 
    DeviceMemory = new VulkanDeviceMemory(Device);
    VkResult Result = DeviceMemory->AllocateMemory(&MemoryAllocateInfo, nullptr);
    if (Result != VK_SUCCESS) 
    {
        std::cout << "Failed to allocate memory: " << MemoryRequirements.size  << " " << Device->HostVisibleMemoryTypeIndexWithMaxHeapSize << err;
        exit(-1);
    }
     
    if (InData) 
    {
        std::uint8_t* p;
        Result = DeviceMemory->MapMemory(0, MemoryRequirements.size, 0, reinterpret_cast<void**>(&p));
        if (Result != VK_SUCCESS)
            std::cout << "Failed to map memory: " << Result << std::endl;
        std::memcpy(p, InData, InSize);
        DeviceMemory->UnmapMemory();
    }  
    setDescriptor();

    Result = BindBufferMemory(DeviceMemory->GetHandle(), 0);
    if (VK_SUCCESS != Result) 
    {
        std::cout << "BindBufferMemory failed " << Result << std::endl;
        exit(-1);
    }
     
    if (useDeviceAddress) {
        GetBufferDeviceAddress();
    }
}

VulkanBuffer::~VulkanBuffer()
{ 
    if (Device)
    {   
        delete DeviceMemory;
        Device->DestroyBuffer(Handle, nullptr);
    }
}

VkBuffer VulkanBuffer::GetHandle() const 
{
    return Handle;
}

VkDeviceMemory VulkanBuffer::GetMemoryHandle()
{
    return DeviceMemory->GetHandle();
}

std::uint32_t VulkanBuffer::GetOffset() 
{
    return Offset;
}

VkIndexType VulkanBuffer::GetIndexType() 
{
    return (GetStride() == 4) ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16;
}

uint64_t VulkanBuffer::GetDeviceAddress()
{
    return DeviceAddress;
}

void VulkanBuffer::GetBufferDeviceAddress()
{
    VkBufferDeviceAddressInfoKHR DeviceAddressInfo = {};
    DeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    DeviceAddressInfo.buffer = Handle;
    //DeviceAddress = VKRT::vkGetBufferDeviceAddressKHR(Device->Handle, &DeviceAddressInfo);
    DeviceAddress = Device->GetBufferDeviceAddressKHR(&DeviceAddressInfo);
}

void VulkanBuffer::UpdateData(std::uint32_t InSize, const void* InData)
{ 
    if (InData != nullptr) 
    {
        std::uint8_t* p;
        VkResult Result = DeviceMemory->MapMemory(0, InSize, 0, reinterpret_cast<void**>(&p));
        if (VK_SUCCESS != Result)
            std::cout << "MapMemory failed: " << Result  << std::endl;
        std::memcpy(p, InData, InSize);
        DeviceMemory->UnmapMemory();
    }
}

VkResult VulkanBuffer::BindBufferMemory(VkDeviceMemory Memory, VkDeviceSize MemoryOffset)
{
    return Device->BindBufferMemory(Handle, Memory, MemoryOffset);
}

VkResult VulkanBuffer::MapMemory(VkDeviceSize Offset, VkDeviceSize Size, VkMemoryMapFlags Flags, void** Data)
{
    return DeviceMemory->MapMemory(Offset, Size, Flags, Data);
}

void VulkanBuffer::UnmapMemory()
{
    DeviceMemory->UnmapMemory();
}

/*

*/
VulkanASBuffer::VulkanASBuffer(VulkanDevice* Device, std::uint32_t InSize)
{
    BufferUsageFlags Usage = BUF_AccelerationStructure;
    Buffer = new VulkanBuffer(Device, 0, InSize, Usage, nullptr);
}

VulkanASBuffer::~VulkanASBuffer()
{
    if (Buffer)
        delete Buffer;
}