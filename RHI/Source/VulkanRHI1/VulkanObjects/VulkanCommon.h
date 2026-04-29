#pragma once
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace vk {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		std::optional<uint32_t> computeFamily;

		bool isComplete(bool presentSupport = true) { 
			return (presentSupport) 
				? (graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value()) 
				: (graphicsFamily.has_value() && computeFamily.has_value());
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};


	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

}

