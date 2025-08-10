#pragma once
#include <VulkanRHI/Vulkan.h>
#include <VulkanRHI/VulkanObjects//VulkanDevice.h>
#include <RHIResource.h>
#include <RHIShader.h>
#include <vector>
class VulkanShader
{
public:
	VulkanShader(VulkanDevice *Device, const std::uint32_t* Code, size_t CodeSize);
	VulkanShader(VulkanDevice* Device, VkShaderModule& module);
	~VulkanShader();
public:
	VkShaderModule Module = VK_NULL_HANDLE;
	VulkanDevice* Device = nullptr;
	const std::uint32_t* Code = nullptr;
	size_t CodeSize = 0;
};

class VulkanVertexShader : public VulkanShader, public RHIVertexShader
{
public:
	VulkanVertexShader(VulkanDevice *Device, const std::uint32_t* Code, size_t CodeSize);
	VulkanVertexShader(VulkanDevice* Device, VkShaderModule& module);
	~VulkanVertexShader();
};

class VulkanPixelShader : public VulkanShader, public RHIPixelShader
{
public:
	VulkanPixelShader(VulkanDevice* Device, const std::uint32_t* Code, size_t CodeSize);
	VulkanPixelShader(VulkanDevice* Device, VkShaderModule& module);
	~VulkanPixelShader();
};

class VulkanGeometryShader : public VulkanShader, public RHIGeometryShader
{
public:
	VulkanGeometryShader(VulkanDevice* Device, const std::uint32_t* Code, size_t CodeSize);
	~VulkanGeometryShader();
};

class VulkanComputeShader : public VulkanShader, public RHIComputeShader
{
public:
	VulkanComputeShader(VulkanDevice* Device, const std::uint32_t* Code, size_t CodeSize);
	VulkanComputeShader(VulkanDevice* Device, VkShaderModule& module);
	~VulkanComputeShader();
};

class VulkanRayGenShader : public VulkanShader, public RHIRayGenShader
{
public:
	VulkanRayGenShader(VulkanDevice* Device, const std::uint32_t* Code, size_t CodeSize);
	VulkanRayGenShader(VulkanDevice* Device, VkShaderModule& module);
	~VulkanRayGenShader();
};

class VulkanRayMissShader : public VulkanShader, public RHIRayMissShader
{
public:
	//VulkanRayMissShader(VulkanDevice* Device, const std::uint32_t* Code, size_t CodeSize);
	VulkanRayMissShader(VulkanDevice* Device, VkShaderModule& module);
	~VulkanRayMissShader();
};

class VulkanRayHitGroupShader : public VulkanShader, public RHIRayHitGroupShader
{
public:
	VulkanRayHitGroupShader(VulkanDevice* Device, VkShaderModule& module, VkShaderModule& moduleAnyhit, VkShaderModule& moduleIntersection)
		: VulkanShader(Device,module)
	{
		ModuleAnyHit = moduleAnyhit;
		ModuleIntersection = moduleIntersection;
	}

	~VulkanRayHitGroupShader()
	{
		Device->DestroyShaderModule(ModuleAnyHit, nullptr);
		Device->DestroyShaderModule(ModuleIntersection, nullptr);
	}
	VkShaderModule ModuleAnyHit = VK_NULL_HANDLE;
	VkShaderModule ModuleIntersection = VK_NULL_HANDLE;
};