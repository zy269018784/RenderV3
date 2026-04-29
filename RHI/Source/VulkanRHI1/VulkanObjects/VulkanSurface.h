#pragma once
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
class VulkanInstance;
class VulkanSurface {
public:
	VulkanSurface() = default;
	VulkanSurface(VulkanInstance& instance, GLFWwindow* window);

public:
	VkSurfaceKHR Handle; 
};
