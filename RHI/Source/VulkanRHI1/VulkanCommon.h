#pragma once
#include <cstdint>
#include <cstring>
template<class T>
static inline void ZeroVulkanStruct(T& Struct, std::int32_t VkStructureType)
{
	(std::int32_t&)Struct.sType = VkStructureType;
	std::memset(((std::uint8_t*)&Struct) + sizeof(VkStructureType), 0, sizeof(T) - sizeof(VkStructureType));
}

#define VULKAN_SUPPORTS_GEOMETRY_SHADERS 0
#define RHI_RAYTRACING 0

inline VkShaderStageFlagBits RHIShaderStageToVKStageBit(RHIShaderStage Stage)
{
	switch (Stage)
	{
	case SS_Vertex:	return VK_SHADER_STAGE_VERTEX_BIT;
	case SS_Pixel:		return VK_SHADER_STAGE_FRAGMENT_BIT;
	case SS_Geometry:	return VK_SHADER_STAGE_GEOMETRY_BIT;
	case SS_Compute:	return VK_SHADER_STAGE_COMPUTE_BIT;
	default:
		break;
	}

	return VK_SHADER_STAGE_ALL;
}

inline VkShaderStageFlagBits FrequencyToVKStageBit(EShaderFrequency InStage)
{
	switch (InStage)
	{
	case SF_Vertex:			return VK_SHADER_STAGE_VERTEX_BIT;
	case SF_Pixel:			return VK_SHADER_STAGE_FRAGMENT_BIT;
	case SF_Geometry:		return VK_SHADER_STAGE_GEOMETRY_BIT;
	case SF_Compute:		return VK_SHADER_STAGE_COMPUTE_BIT;

#if VULKAN_RHI_RAYTRACING
	case SF_RayGen:			return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	case SF_RayMiss:		return VK_SHADER_STAGE_MISS_BIT_KHR;
	case SF_RayHitGroup:	return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR; // vkrt todo: How to handle VK_SHADER_STAGE_ANY_HIT_BIT_KHR?
	case SF_RayCallable:	return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
#endif // VULKAN_RHI_RAYTRACING

	default:
		//checkf(false, TEXT("Undefined shader stage %d"), (int32)InStage);
		break;
	}

	return VK_SHADER_STAGE_ALL;
}

static inline VkSampleCountFlagBits toVkMultisampleCount(uint8_t sampleCount)
{
	//VkPhysicalDeviceProperties physicalDeviceProperties;
	//vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
	//VkSampleCountFlags counts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts, physicalDeviceProperties.limits.framebufferDepthSampleCounts);
	if (sampleCount & VK_SAMPLE_COUNT_64_BIT) {
		return VK_SAMPLE_COUNT_64_BIT;
	}
	if (sampleCount & VK_SAMPLE_COUNT_32_BIT) {
		return VK_SAMPLE_COUNT_32_BIT;
	}
	if (sampleCount & VK_SAMPLE_COUNT_16_BIT) {
		return VK_SAMPLE_COUNT_16_BIT;
	}
	if (sampleCount & VK_SAMPLE_COUNT_8_BIT) {
		return VK_SAMPLE_COUNT_8_BIT;
	}
	if (sampleCount & VK_SAMPLE_COUNT_4_BIT) {
		return VK_SAMPLE_COUNT_4_BIT;
	}
	if (sampleCount & VK_SAMPLE_COUNT_2_BIT) {
		return VK_SAMPLE_COUNT_2_BIT;
	}
	return VK_SAMPLE_COUNT_1_BIT;
}
 
static inline VkCompareOp toVkCompareOp(ECompareFunction cmp) {
	switch (cmp) {
	case CF_Less:
		return VK_COMPARE_OP_LESS;
	case CF_LessEqual:
		return VK_COMPARE_OP_LESS_OR_EQUAL;
	case CF_Greater:
		return VK_COMPARE_OP_GREATER;
	case CF_GreaterEqual:
		return VK_COMPARE_OP_GREATER_OR_EQUAL;
	case CF_Equal:
		return VK_COMPARE_OP_EQUAL;
	case CF_NotEqual:
		return VK_COMPARE_OP_NOT_EQUAL;
	case CF_Never:
		return VK_COMPARE_OP_NEVER;
	case CF_Always:
		return VK_COMPARE_OP_ALWAYS;
	default:
		return VK_COMPARE_OP_LESS;
	}
}