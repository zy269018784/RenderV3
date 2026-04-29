#include <VulkanRHI1/Vulkan.h>
#include <VulkanRHI1/VulkanObjects/VulkanDescriptorPool.h>
#include <cstring>
enum {
	VK_DESCRIPTOR_TYPE_BEGIN_RANGE = VK_DESCRIPTOR_TYPE_SAMPLER,
	VK_DESCRIPTOR_TYPE_END_RANGE = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
	VK_DESCRIPTOR_TYPE_RANGE_SIZE = (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT - VK_DESCRIPTOR_TYPE_SAMPLER + 1),
};


VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice* Device, VulkanDescriptorSetLayout* Layout, std::uint32_t MaxSetsAllocations)
	: Device(Device), Handle(VK_NULL_HANDLE)
{
	//MaxDescriptorSets = Layout->GetLayouts().size();  //todo...

	std::vector<VkDescriptorPoolSize> Types;
	
	Types.resize(VK_DESCRIPTOR_TYPE_RANGE_SIZE);

	for (std::uint32_t TypeIndex = VK_DESCRIPTOR_TYPE_BEGIN_RANGE; TypeIndex <= VK_DESCRIPTOR_TYPE_END_RANGE; ++TypeIndex)
	{
		VkDescriptorType DescriptorType = (VkDescriptorType)TypeIndex;
		std::uint32_t NumTypesUsed = 0;// Layout->GetTypesUsed(DescriptorType);//todo...
		if (NumTypesUsed > 0)
		{
			VkDescriptorPoolSize* Type = &Types.data()[TypeIndex];
			std::memset(Type, 0x0, sizeof(VkDescriptorPoolSize));
			Type->type = DescriptorType;
			Type->descriptorCount = NumTypesUsed * MaxSetsAllocations;
		}
	}

	VkDescriptorPoolCreateInfo PoolInfo;
	std::memset(&PoolInfo, 0x0, sizeof (VkDescriptorPoolCreateInfo));
	PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	//ZeroVulkanStruct(PoolInfo, VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO);
	// you don't need this flag because pool reset feature. Also this flag increase pool size in memory and vkResetDescriptorPool time.
	//PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	PoolInfo.poolSizeCount = Types.size();
	PoolInfo.pPoolSizes = Types.data();
	PoolInfo.maxSets = MaxDescriptorSets;

	Device->CreateDescriptorPool(&PoolInfo, nullptr, &Handle);
}
VulkanDescriptorPool::~VulkanDescriptorPool()
{
	if (Device && VK_NULL_HANDLE != Handle)
		Device->DestroyDescriptorPool(Handle, nullptr);
}

