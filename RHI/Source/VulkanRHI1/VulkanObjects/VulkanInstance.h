#pragma once
#include <VulkanRHI1/Vulkan.h> 
#include <cstdint>
#include <vector>
//#define GLFW_INCLUDE_VULKAN
//#include <glfw3.h>
 
class VulkanInstance
{
public:
	VulkanInstance();
	~VulkanInstance();
	VkInstance GetHandle() const;
public:
	VkResult EnumeratePhysicalDevices(std::uint32_t* PhysicalDeviceCount, VkPhysicalDevice* PhysicalDevices);

	VkResult EnumeratePhysicalDeviceGroups(std::uint32_t* PhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties);

	PFN_vkVoidFunction GetInstanceProcAddr(const char* pName);
private:
	VkInstance Handle;
}; 