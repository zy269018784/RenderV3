#include <VulkanRHI1/VulkanObjects/VulkanDescriptorSetLayout.h>


VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice* Device) 
	: Device(Device)
{
	Handle = VK_NULL_HANDLE;
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	if (Device && VK_NULL_HANDLE != Handle) 
		Device->DestroyDescriptorSetLayout(Handle, nullptr);
}

void VulkanDescriptorSetLayout::create() {
	VkDescriptorSetLayoutCreateFlags flags = 0;

	VkDescriptorSetLayoutBindingFlagsCreateInfo flagsInfo = {};
	flagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
	flagsInfo.pNext = nullptr;
	flagsInfo.bindingCount = static_cast<uint32_t>(BindingFlags.size());
	flagsInfo.pBindingFlags = BindingFlags.data();

	VkDescriptorSetLayoutCreateInfo descSetLayoutInfo = {};
	descSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutInfo.bindingCount = static_cast<uint32_t>(Bindings.size());
	descSetLayoutInfo.pBindings = Bindings.data();
	descSetLayoutInfo.pNext = &flagsInfo;
	descSetLayoutInfo.flags = flags;

	if (Device->CreateDescriptorSetLayout(&descSetLayoutInfo, nullptr, &Handle) != VK_SUCCESS) {
		//return false;
	}
	//return true;
}