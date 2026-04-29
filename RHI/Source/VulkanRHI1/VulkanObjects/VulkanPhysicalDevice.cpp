#include <VulkanRHI1/VulkanObjects//VulkanPhysicalDevice.h>


VulkanPhysicalDevice::VulkanPhysicalDevice() 
	: Handle(VK_NULL_HANDLE)
{

}

VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice InHandle)
	: Handle(InHandle)
{

}

VulkanPhysicalDevice::~VulkanPhysicalDevice()
{

}

VkPhysicalDevice VulkanPhysicalDevice::GetHandle() const
{
	return Handle;
}

vk::QueueFamilyIndices VulkanPhysicalDevice::GetQueueFamilyIndices()
{
	return QueueFamilyIndices;
}

void VulkanPhysicalDevice::SetQueueFamilyIndices(vk::QueueFamilyIndices& InQueueFamilyIndices)
{
	QueueFamilyIndices = InQueueFamilyIndices;
}

VkPhysicalDeviceProperties VulkanPhysicalDevice::GetProperties()
{
	return PhysicalDeviceProperties;
}

VkPhysicalDeviceRayTracingPipelinePropertiesKHR VulkanPhysicalDevice::GetRayTracingPipelineProperties()
{
	return RayTracingPipelineProperties;
}

VkPhysicalDeviceAccelerationStructurePropertiesKHR VulkanPhysicalDevice::GetAccelerationStructureProperties()
{
	return AccelerationStructureProperties;
}

uint32_t VulkanPhysicalDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	vkGetPhysicalDeviceMemoryProperties(Handle, &MemProperties);
	for (uint32_t i = 0; i < MemProperties.memoryTypeCount; ++i)
	{
		if ((typeFilter & (1 << i)) && (MemProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("Failed to find suitable memory type!");
}

/*
	Get Physical Device Properties
*/
void VulkanPhysicalDevice::GetPhysicalDeviceProperties(VkPhysicalDeviceProperties* Properties)
{
	vkGetPhysicalDeviceProperties(Handle, Properties);
}

void VulkanPhysicalDevice::GetPhysicalDeviceProperties()
{ 
	vkGetPhysicalDeviceProperties(Handle, &PhysicalDeviceProperties);
}


void VulkanPhysicalDevice::GetPhysicalDeviceProperties2()
{
 
	//加速结构属性  用到minAccelerationStructureScratchOffsetAlignment
	AccelerationStructureProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
	RayTracingPipelineProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
	RayTracingPipelineProperties.pNext = &AccelerationStructureProperties;  

	VkPhysicalDeviceProperties2 Properties2{};
	Properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	Properties2.pNext = &RayTracingPipelineProperties;
	vkGetPhysicalDeviceProperties2(Handle, &Properties2);
}

/*
	Get Physical Device Queue Properties：
*/
void VulkanPhysicalDevice::GetPhysicalDeviceQueueFamilyProperties(std::uint32_t* QueueFamilyPropertyCount, VkQueueFamilyProperties* QueueFamilyProperties)
{
	vkGetPhysicalDeviceQueueFamilyProperties(Handle, QueueFamilyPropertyCount, QueueFamilyProperties);
}

void VulkanPhysicalDevice::GetPhysicalDeviceQueueFamilyProperties2(std::uint32_t* QueueFamilyPropertyCount, VkQueueFamilyProperties2* QueueFamilyProperties)
{
	vkGetPhysicalDeviceQueueFamilyProperties2(Handle, QueueFamilyPropertyCount, QueueFamilyProperties);
}

/*
	Get Physical Device Features
*/
void VulkanPhysicalDevice::GetPhysicalDeviceFeatures(VkPhysicalDeviceFeatures* Features)
{
	vkGetPhysicalDeviceFeatures(Handle, Features);
}

void VulkanPhysicalDevice::GetPhysicalDeviceFeatures2()
{
	AccelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	VkPhysicalDeviceFeatures2 deviceFeatures2{};
	deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	deviceFeatures2.pNext = &AccelerationStructureFeatures;	
	
	VkPhysicalDeviceDescriptorIndexingFeatures indexing_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT, nullptr };
	AccelerationStructureFeatures.pNext = &indexing_features;

	deviceFeatures2.features.shaderInt64 = true;
	vkGetPhysicalDeviceFeatures2(Handle, &deviceFeatures2);

	 

	//bool bindless_supported = indexing_features.descriptorBindingPartiallyBound && indexing_features.runtimeDescriptorArray;
	//if (bindless_supported) {
	//	printf("bindless_supported \n");
	//}
}
/*
	Get Physical Device Format Properties
*/
void VulkanPhysicalDevice::GetPhysicalDeviceFormatProperties(VkFormat Format, VkFormatProperties* FormatProperties)
{
	vkGetPhysicalDeviceFormatProperties(Handle, Format, FormatProperties);
}

void VulkanPhysicalDevice::GetPhysicalDeviceFormatProperties2(VkFormat Format, VkFormatProperties2* FormatProperties)
{
	vkGetPhysicalDeviceFormatProperties2(Handle, Format, FormatProperties);
}

/*
	Get Physical Device Memory Properties
*/
void VulkanPhysicalDevice::GetPhysicalDeviceMemoryProperties(VkPhysicalDeviceMemoryProperties* MemoryProperties)
{
	vkGetPhysicalDeviceMemoryProperties(Handle, MemoryProperties);
}

/*
	EnumeratePhysical Device Extension Properties
*/
VkResult VulkanPhysicalDevice::EnumerateDeviceExtensionProperties(const char* LayerName, uint32_t* PropertyCount, VkExtensionProperties* Properties)
{
	return vkEnumerateDeviceExtensionProperties(Handle, LayerName, PropertyCount, Properties);
}

/*
	Device
*/
VkResult VulkanPhysicalDevice::CreateDevice(const VkDeviceCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkDevice* Device)
{
	return vkCreateDevice(Handle, CreateInfo, Allocator, Device);
}

