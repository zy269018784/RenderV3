#pragma once
#include <VulkanRHI1/Vulkan.h>
#include <RHIDefinitions.h>
#include <VulkanRHI1/VulkanObjects/VulkanCommon.h>
#include <optional>
#include <stdexcept>


class VulkanPhysicalDevice
{
public:
	VulkanPhysicalDevice();
	VulkanPhysicalDevice(VkPhysicalDevice InHandle);
	~VulkanPhysicalDevice();
	/*
		获取物理设备句柄
	*/
	VkPhysicalDevice GetHandle() const;
	vk::QueueFamilyIndices GetQueueFamilyIndices();
	void SetQueueFamilyIndices(vk::QueueFamilyIndices &InQueueFamilyIndices);
	/*
		获取物理设备属性
	*/
	VkPhysicalDeviceProperties GetProperties();
	/*
		获取物理设备支持的光追管线属性
	*/
	VkPhysicalDeviceRayTracingPipelinePropertiesKHR GetRayTracingPipelineProperties();
	/*
		获取物理设备支持的加速结构属性
	*/
	VkPhysicalDeviceAccelerationStructurePropertiesKHR GetAccelerationStructureProperties();
public:
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	/*
		Get Physical Device Properties
	*/
	void GetPhysicalDeviceProperties();
	void GetPhysicalDeviceProperties(VkPhysicalDeviceProperties* Properties);
	void GetPhysicalDeviceProperties2();

	/*
		Get Physical Device Queue Properties
	*/
	void GetPhysicalDeviceQueueFamilyProperties(std::uint32_t* QueueFamilyPropertyCount, VkQueueFamilyProperties* QueueFamilyProperties);
	void GetPhysicalDeviceQueueFamilyProperties2(std::uint32_t* QueueFamilyPropertyCount, VkQueueFamilyProperties2* QueueFamilyProperties);

	/*
		Get Physical Device Features
	*/
	void GetPhysicalDeviceFeatures(VkPhysicalDeviceFeatures* Features);
	void GetPhysicalDeviceFeatures2();

	/*
		Get Physical Device Format Properties
	*/
	void GetPhysicalDeviceFormatProperties(VkFormat  Format, VkFormatProperties* FormatProperties);
	void GetPhysicalDeviceFormatProperties2(VkFormat Format, VkFormatProperties2* FormatProperties);

	/*
		Get Physical Device Memory Properties
	*/
	void GetPhysicalDeviceMemoryProperties(VkPhysicalDeviceMemoryProperties* MemoryProperties);

	/*
		EnumeratePhysical Device Extension Properties
	*/
	VkResult EnumerateDeviceExtensionProperties(const char* LayerName, uint32_t* PropertyCount, VkExtensionProperties* Properties);
	/*
		Device
	*/
	VkResult CreateDevice(const VkDeviceCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkDevice* Device);
private:
	/*
		物理设备句柄
	*/
	VkPhysicalDevice Handle;

	vk::QueueFamilyIndices QueueFamilyIndices;
	/*
		物理设备属性
	*/
	VkPhysicalDeviceProperties PhysicalDeviceProperties;
	/*
		物理设备内存属性
	*/
	VkPhysicalDeviceMemoryProperties MemProperties;
	/*
		物理设备支持的加速结构属性
	*/
	VkPhysicalDeviceAccelerationStructurePropertiesKHR AccelerationStructureProperties{};
	/*
		物理设备支持的光追管线属性
	*/
	VkPhysicalDeviceRayTracingPipelinePropertiesKHR  RayTracingPipelineProperties{};
	/*
		物理设备支持的加速结构特性
	*/
	VkPhysicalDeviceAccelerationStructureFeaturesKHR AccelerationStructureFeatures{};
};