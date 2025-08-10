#include <VulkanRHI/VulkanObjects/VulkanShader.h>
#include <iostream>
#include <cstring>
VulkanShader::VulkanShader(VulkanDevice* Device, const uint32_t* Code, size_t CodeSize)
	: Device(Device), Code(Code), CodeSize(CodeSize)
{
    VkShaderModuleCreateInfo CreateInfo;
	memset(&CreateInfo, 0, sizeof(VkShaderModuleCreateInfo));
	CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	CreateInfo.codeSize = CodeSize;
	CreateInfo.pCode = Code;
	if (Device->CreateShaderModule(&CreateInfo, nullptr, &Module) != VK_SUCCESS)
	{
		throw std::runtime_error("CreateShaderModule failed!");
	}
}

VulkanShader::VulkanShader(VulkanDevice* Device, VkShaderModule& module)
	: Device(Device), Module(module)
{

}

VulkanShader::~VulkanShader()
{
	if (Device && VK_NULL_HANDLE != Module)
		Device->DestroyShaderModule(Module, nullptr);
}

VulkanVertexShader::VulkanVertexShader(VulkanDevice* Device, const uint32_t* Code, size_t CodeSize)
	: VulkanShader(Device, Code, CodeSize)
{
	
}

VulkanVertexShader::VulkanVertexShader(VulkanDevice* Device, VkShaderModule& module)
	: VulkanShader(Device, module)
{ 
}

VulkanVertexShader::~VulkanVertexShader()
{
	
}

VulkanPixelShader::VulkanPixelShader(VulkanDevice* Device, const uint32_t* Code, size_t CodeSize)
	: VulkanShader(Device, Code, CodeSize)
{ 
}

VulkanPixelShader::VulkanPixelShader(VulkanDevice* Device, VkShaderModule& module)
	: VulkanShader(Device, module)
{
}

VulkanPixelShader::~VulkanPixelShader()
{
	
}

VulkanGeometryShader::VulkanGeometryShader(VulkanDevice* Device, const uint32_t* Code, size_t CodeSize)
	: VulkanShader(Device, Code, CodeSize)
{}

VulkanGeometryShader::~VulkanGeometryShader()
{}

VulkanComputeShader::VulkanComputeShader(VulkanDevice* Device, const uint32_t* Code, size_t CodeSize)
	: VulkanShader(Device, Code, CodeSize)
{}

VulkanComputeShader::VulkanComputeShader(VulkanDevice* Device, VkShaderModule& module)
	: VulkanShader(Device, module)
{
}

VulkanComputeShader::~VulkanComputeShader()
{}

VulkanRayGenShader::VulkanRayGenShader(VulkanDevice* Device, const std::uint32_t* Code, size_t CodeSize)
	: VulkanShader(Device, Code, CodeSize)
{

}

VulkanRayGenShader::VulkanRayGenShader(VulkanDevice* Device, VkShaderModule& module)
	: VulkanShader(Device, module)
{

}

VulkanRayGenShader::~VulkanRayGenShader()
{}

VulkanRayMissShader::VulkanRayMissShader(VulkanDevice* Device, VkShaderModule& module)
	: VulkanShader(Device, module)
{

}

VulkanRayMissShader::~VulkanRayMissShader()
{

}