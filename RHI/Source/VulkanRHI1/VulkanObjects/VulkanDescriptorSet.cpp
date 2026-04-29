#include <VulkanRHI1/VulkanObjects/VulkanDescriptorSet.h>
#include <VulkanRHI1/VulkanObjects/VulkanDevice.h>
#include <VulkanRHI1/VulkanObjects/VulkanBuffer.h>
#include <VulkanRHI1/VulkanObjects/VulkanDescriptorPool.h>
#include <VulkanRHI1/VulkanObjects/VulkanDescriptorSetLayout.h>
#include <stdexcept>


VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice* InDevice, VulkanDescriptorPool* InDescriptorPool, VulkanDescriptorSetLayout* InDescriptorSetLayout,
	const uint32_t descSetCount)
{
	Device = InDevice;
	DescriptorPool = InDescriptorPool;
	DescriptorSetLayout = InDescriptorSetLayout;

	const VkDescriptorSetLayout setLayout = InDescriptorSetLayout->Handle;
	VkDescriptorSetAllocateInfo descSetAllocInfo = {};
	descSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descSetAllocInfo.pNext = nullptr;
	descSetAllocInfo.descriptorPool = InDescriptorPool->Handle;
	descSetAllocInfo.descriptorSetCount = descSetCount;
	descSetAllocInfo.pSetLayouts = &setLayout;

	if (Device->AllocateDescriptorSets(&descSetAllocInfo, &Handle) != VK_SUCCESS) {
		throw std::runtime_error("failed to vkAllocateDescriptorSets!");
	} 
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{

}

void VulkanDescriptorSet::updateBuffer(const std::vector<VulkanBuffer*>& buffers, const uint32_t dstBinding, const uint32_t descCount,
	VkDescriptorType descType)
{
	std::vector<VkDescriptorBufferInfo> bufferInfos;
	bufferInfos.reserve(buffers.size());
	for (size_t i = 0; i < buffers.size(); ++i) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = buffers[i]->Handle;
		bufferInfo.offset = 0;
		bufferInfo.range = buffers[i]->GetSize();
		bufferInfos.emplace_back(std::move(bufferInfo));
	}

	VkWriteDescriptorSet writeSet = {};
	writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeSet.pNext = nullptr;
	writeSet.dstBinding = dstBinding;
	writeSet.dstSet = Handle;
	writeSet.descriptorCount = descCount;
	writeSet.descriptorType = descType;
	writeSet.pBufferInfo = bufferInfos.data();

	Device->UpdateDescriptorSets(1, &writeSet, 0, nullptr);
}

//void VulkanDescriptorSet::updateTexelBuffer(const BufferViewPtr& bufferView,
//	const uint32_t dstBinding,
//	const uint32_t descCount,
//	VkDescriptorType descType)
//{
//	const VkBufferView texelBufferView = bufferView->getBufferViewHandle();
//
//	VkWriteDescriptorSet writeSet = {};
//	writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//	writeSet.pNext = nullptr;
//	writeSet.dstBinding = dstBinding;
//	writeSet.dstSet = _descriptorSet;
//	writeSet.descriptorCount = descCount;
//	writeSet.descriptorType = descType;
//	writeSet.pTexelBufferView = &texelBufferView;
//
//	vkUpdateDescriptorSets(Device->Handle, 1, &writeSet, 0, nullptr);
//}

void VulkanDescriptorSet::updateImage(const std::vector<VkDescriptorImageInfo>& imageInfos, const uint32_t dstBinding, VkDescriptorType descType)
{
	VkWriteDescriptorSet writeSet = {};
	writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeSet.pNext = nullptr;
	writeSet.dstBinding = dstBinding;
	writeSet.dstSet = Handle;
	writeSet.descriptorCount = static_cast<uint32_t>(imageInfos.size());
	writeSet.descriptorType = descType;
	writeSet.pImageInfo = imageInfos.data(); 
	Device->UpdateDescriptorSets(1, &writeSet, 0, nullptr);
} 