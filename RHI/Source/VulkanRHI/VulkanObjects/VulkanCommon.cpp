#include <VulkanRHI/VulkanObjects/VulkanCommon.h>
#include <iostream>


vk::SwapChainSupportDetails vk::querySwapChainSupport(VkPhysicalDevice device,VkSurfaceKHR surface)
{
	SwapChainSupportDetails details{};
	if (surface == VK_NULL_HANDLE)
		return details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		for (int i = 0; i < formatCount; i++)
		{
			std::cout << "querySwapChainSupport " << details.formats[i].format << std::endl;
		}
	}
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}
	return details;
}
 
vk::QueueFamilyIndices vk::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data()); 
	int i = 0;
	std::cout << "queueFamilyCount " << queueFamilyCount << std::endl;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}
		if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			indices.computeFamily = i;
		}
		VkBool32 presentSupport = false;
		if (surface != VK_NULL_HANDLE) {
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (presentSupport) {
				indices.presentFamily = i;
			}
		}

		if (indices.isComplete(presentSupport)) {
			break;
		}
		i++;
	}
	return indices;
}