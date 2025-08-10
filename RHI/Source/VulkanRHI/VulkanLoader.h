#pragma once
#include <VulkanRHI/Vulkan.h>

#define VULKANRHI_API

#define VULKAN_RAYTRACING_EXT VKRT

#define ENUM_VK_ENTRYPOINTS_RAYTRACING(EnumMacro) \
	EnumMacro(PFN_vkCreateAccelerationStructureKHR, vkCreateAccelerationStructureKHR) \
	EnumMacro(PFN_vkDestroyAccelerationStructureKHR, vkDestroyAccelerationStructureKHR) \
	EnumMacro(PFN_vkCmdBuildAccelerationStructuresKHR, vkCmdBuildAccelerationStructuresKHR) \
	EnumMacro(PFN_vkGetAccelerationStructureBuildSizesKHR, vkGetAccelerationStructureBuildSizesKHR) \
	EnumMacro(PFN_vkGetAccelerationStructureDeviceAddressKHR, vkGetAccelerationStructureDeviceAddressKHR) \
	EnumMacro(PFN_vkCmdTraceRaysKHR, vkCmdTraceRaysKHR) \
	EnumMacro(PFN_vkCmdTraceRaysIndirectKHR, vkCmdTraceRaysIndirectKHR) \
	EnumMacro(PFN_vkCmdTraceRaysIndirect2KHR, vkCmdTraceRaysIndirect2KHR) \
	EnumMacro(PFN_vkCreateRayTracingPipelinesKHR, vkCreateRayTracingPipelinesKHR) \
	EnumMacro(PFN_vkGetRayTracingShaderGroupHandlesKHR, vkGetRayTracingShaderGroupHandlesKHR) \
	EnumMacro(PFN_vkCmdWriteAccelerationStructuresPropertiesKHR, vkCmdWriteAccelerationStructuresPropertiesKHR) \
	EnumMacro(PFN_vkCmdCopyAccelerationStructureKHR, vkCmdCopyAccelerationStructureKHR)\
	EnumMacro(PFN_vkGetBufferDeviceAddressKHR, vkGetBufferDeviceAddressKHR) 


// Declare all Vulkan functions
#define DECLARE_VK_ENTRYPOINTS(Type,Func) extern VULKANRHI_API Type Func;
;
namespace VKRT //VULKAN_RAYTRACING_EXT 
{
	/*
	 *	声明Vulkan Raytracing相关方法
	 */
	ENUM_VK_ENTRYPOINTS_RAYTRACING(DECLARE_VK_ENTRYPOINTS);
}