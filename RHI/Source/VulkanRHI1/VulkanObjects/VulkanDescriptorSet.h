#pragma once
#include <VulkanRHI1/Vulkan.h>
#include <vector>

class VulkanDevice;
class VulkanBuffer;
class VulkanDescriptorPool;
class VulkanDescriptorSetLayout;

class VulkanDescriptorSet
{
public:
	explicit VulkanDescriptorSet() = default;
	explicit VulkanDescriptorSet(VulkanDevice* InDevice, VulkanDescriptorPool *InDescriptorPool, VulkanDescriptorSetLayout *InDescriptorSetLayout, 
		const uint32_t descSetCount);
	~VulkanDescriptorSet();


	void updateImage(const std::vector<VkDescriptorImageInfo>&imageInfos, const uint32_t dstBinding, VkDescriptorType descType);

	inline void updateUniformBuffer(const std::vector<VulkanBuffer*>&buffers, const uint32_t dstBinding, const uint32_t descCount) {
		updateBuffer(buffers, dstBinding, descCount, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	}

	inline void updateStorageBuffer(const std::vector<VulkanBuffer*>&buffers, const uint32_t dstBinding, const uint32_t descCount) {
		updateBuffer(buffers, dstBinding, descCount, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
	}	
	
	//inline void updateStorageTexelBuffer(const BufferViewPtr & bufferView, const uint32_t dstBinding, const uint32_t descCount) {
	//	updateTexelBuffer(bufferView, dstBinding, descCount, VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER);
	//}
	 
	inline VkDescriptorSet getHandle(void) const
	{
		return Handle;
	}

private:
	void updateBuffer(const std::vector<VulkanBuffer*>&buffers, const uint32_t dstBinding, const uint32_t descCount, VkDescriptorType descType);
	//void updateTexelBuffer(const BufferViewPtr & bufferView, const uint32_t dstBinding, const uint32_t descCount, VkDescriptorType descType);
public:
	VkDescriptorSet Handle = VK_NULL_HANDLE;


private:
	VulkanDevice*			Device;
	VulkanDescriptorPool*		DescriptorPool;
	VulkanDescriptorSetLayout*  DescriptorSetLayout;
 
};