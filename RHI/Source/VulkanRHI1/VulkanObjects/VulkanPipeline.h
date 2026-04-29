#pragma once
#include <VulkanRHI1/Vulkan.h>
#include <VulkanRHI1/VulkanObjects/VulkanDevice.h>
#include <VulkanRHI1/VulkanObjects/VulkanRenderPass.h>
#include <VulkanRHI1/VulkanObjects/VulkanShader.h>
#include <VulkanRHI1/VulkanObjects/VulkanShaderBindings.h>

#include <RHIPipeline.h>
#include <RHIDefinitions.h>
#include <RHIVertexInput.h>

#define USE_VULKAN_RAYTRACING 1


static inline VkFormat ToVkBufferFormat(RHIVertexInputAttribute::Format Type)
{
	switch (Type)
	{
	case RHIVertexInputAttribute::VET_Float1:
		return VK_FORMAT_R32_SFLOAT;
	case RHIVertexInputAttribute::VET_Float2:
		return VK_FORMAT_R32G32_SFLOAT;
	case RHIVertexInputAttribute::VET_Float3:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case RHIVertexInputAttribute::VET_Float4:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	case RHIVertexInputAttribute::VET_PackedNormal:
		return VK_FORMAT_R8G8B8A8_SNORM;
	case RHIVertexInputAttribute::VET_UByte4:
		return VK_FORMAT_R8G8B8A8_UINT;
	case RHIVertexInputAttribute::VET_UByte4N:
		return VK_FORMAT_R8G8B8A8_UNORM;
	case RHIVertexInputAttribute::VET_Color:
		return VK_FORMAT_B8G8R8A8_UNORM;
	case RHIVertexInputAttribute::VET_Short2:
		return VK_FORMAT_R16G16_SINT;
	case RHIVertexInputAttribute::VET_Short4:
		return VK_FORMAT_R16G16B16A16_SINT;
	case RHIVertexInputAttribute::VET_Short2N:
		return VK_FORMAT_R16G16_SNORM;
	case RHIVertexInputAttribute::VET_Half2:
		return VK_FORMAT_R16G16_SFLOAT;
	case RHIVertexInputAttribute::VET_Half4:
		return VK_FORMAT_R16G16B16A16_SFLOAT;
	case RHIVertexInputAttribute::VET_Short4N:		// 4 X 16 bit word: normalized
		return VK_FORMAT_R16G16B16A16_SNORM;
	case RHIVertexInputAttribute::VET_UShort2:
		return VK_FORMAT_R16G16_UINT;
	case RHIVertexInputAttribute::VET_UShort4:
		return VK_FORMAT_R16G16B16A16_UINT;
	case RHIVertexInputAttribute::VET_UShort2N:		// 16 bit word normalized to (value/65535.0:value/65535.0:0:0:1)
		return VK_FORMAT_R16G16_UNORM;
	case RHIVertexInputAttribute::VET_UShort4N:		// 4 X 16 bit word unsigned: normalized
		return VK_FORMAT_R16G16B16A16_UNORM;
	case RHIVertexInputAttribute::VET_URGB10A2N:
		return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
	case RHIVertexInputAttribute::VET_UInt:
		return VK_FORMAT_R32_UINT;
	default:
		break;
	}

	return VK_FORMAT_UNDEFINED;
}

static inline VkShaderStageFlagBits RHIShaderStageToVKStageBit2(RHIShaderStage Stage)
{
	switch (Stage)
	{
	case SS_Vertex:		return VK_SHADER_STAGE_VERTEX_BIT;
	case SS_Pixel:		return VK_SHADER_STAGE_FRAGMENT_BIT;
	case SS_Geometry:	return VK_SHADER_STAGE_GEOMETRY_BIT;
	case SS_Compute:	return VK_SHADER_STAGE_COMPUTE_BIT;
#if USE_VULKAN_RAYTRACING
	case SS_RayGen:		return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	case SS_RayMiss:	return VK_SHADER_STAGE_MISS_BIT_KHR;
	case SS_RayCallable:return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
	case SS_RayHitGroup:
		return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
	//return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
#endif // USE_VULKAN_RAYTRACING

	default:
		break;
	} 
	return VK_SHADER_STAGE_ALL;
}

static inline VkPrimitiveTopology ToVkPrimitiveTopology(RHIPrimitiveType Type)
{
	switch (Type)
	{
	case PT_PointList:
		return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	case PT_LineList:
		return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	case PT_TriangleList:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case PT_TriangleStrip:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	default:
		break;
	}
	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

static inline VkFrontFace toVkFrontFace(EFrontFace frontFace) {
	switch (frontFace)
	{
	case CCW:
		return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	case CW:
		return VK_FRONT_FACE_CLOCKWISE;
	default:
		return VK_FRONT_FACE_MAX_ENUM;
	}
}
  
class VulkanPipeline 
{
public:
	VulkanPipeline();
	VulkanPipeline(VulkanDevice* Device);
	VulkanPipeline(VulkanDevice* Device, VkPipeline Handle);
	~VulkanPipeline(); 

public:
	VkPipeline Handle = VK_NULL_HANDLE;
	VkPipelineBindPoint BindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
	VkPipelineCache PipelineCache = VK_NULL_HANDLE;
	VulkanDevice* Device = nullptr;

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
};
 

class VulkanGraphicsPipeline : public VulkanPipeline, public RHIGraphicsPipeline 
{
public: 
	VulkanGraphicsPipeline(VulkanDevice* InDevice); 
	~VulkanGraphicsPipeline();

	virtual void attachShaderModule(RHIShader* pShader);
	virtual void setVertexInput(RHIVertexInputInfo& info);
	virtual void setInputAssembly(RHIPrimitiveType topo);
	virtual void setTessellation(RHITessellationInfo info);
	virtual void setViewport(RHIViewportInfo info);
	virtual void setRasterization(RHIRasterizationInfo info);
	virtual void setMultiSampleCount(uint32_t sampleCount);
	virtual void setDepthStencil(RHIDepthStencilInfo info);
	virtual void setColorBlend(RHIColorBlendInfo info);
	virtual void setDynamic(RHIDynamicInfo info);

	virtual void setRenderPass(RHIRenderPass* pRenderPass) {
		this->pRenderPass = dynamic_cast<VulkanRenderPass*>(pRenderPass);
	}

	virtual void setShaderBindings(RHIShaderBindings* pShaderBindings) {
		m_pVulkanShaderBindings = dynamic_cast<VulkanShaderBindings*>(pShaderBindings);
	}

	virtual void create();

	void CreatePipelineLayout();
private: 
	std::vector< VkPipelineShaderStageCreateInfo > shaderStages;
	 
	std::vector< VkVertexInputAttributeDescription > attributes;
	std::vector< VkVertexInputBindingDescription > bindingDescriptions;

	VkPipelineVertexInputStateCreateInfo	vertexInput{};
	VkPipelineInputAssemblyStateCreateInfo  inputAssembly{};
	VkPipelineTessellationStateCreateInfo	tessellation{};
	VkPipelineViewportStateCreateInfo		viewportInfo{};	
	VkPipelineRasterizationStateCreateInfo  rasterization{}; 
	VkPipelineMultisampleStateCreateInfo	multisample{}; 
	VkPipelineDepthStencilStateCreateInfo	depthStencil{};

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};

	std::vector< VkPipelineColorBlendAttachmentState > ColorBlendAttachments;
	VkPipelineColorBlendStateCreateInfo		colorBlend{};

	std::vector<VkDynamicState> dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo		dynamic{};
	
	VulkanRenderPass* pRenderPass = nullptr;   
public:
	VulkanShaderBindings* m_pVulkanShaderBindings = nullptr; 

};

class VulkanComputePipeline : public VulkanPipeline, public RHIComputePipeline
{
public: 
	VulkanComputePipeline(VulkanDevice* InDevice);
	~VulkanComputePipeline();

	virtual void attachShaderModule(RHIShader* pShader);
	virtual void setShaderBindings(RHIShaderBindings* pShaderBindings) {
		m_pVulkanShaderBindings = dynamic_cast<VulkanShaderBindings*>(pShaderBindings);
	}

	virtual void create();

	void CreatePipelineLayout();
private:
	VkPipelineShaderStageCreateInfo shaderStage = {};
public:
	VulkanShaderBindings* m_pVulkanShaderBindings = nullptr;
};

class VulkanRaytracingPipeline : public VulkanPipeline, public RHIRaytracingPipeline 
{
public:
	VulkanRaytracingPipeline(VulkanDevice* InDevice);
	~VulkanRaytracingPipeline();
	virtual void attachShaderModule(RHIShader* pShader); 
	virtual void setShaderBindings(RHIShaderBindings* pShaderBindings) {
		m_pVulkanShaderBindings = dynamic_cast<VulkanShaderBindings*>(pShaderBindings);
	}
	virtual RHIShaderBindings* getShaderBindings() {
		return m_pVulkanShaderBindings;
	}
	virtual void create();	
	
protected:
	void CreatePipelineLayout();
public:

	std::vector<VkRayTracingShaderGroupCreateInfoKHR> shaderGroups{};

	uint32_t missTableCount = 0;
	uint32_t hitgroupTableCount = 0;
	//std::vector<RHIRayTracingShader*> RayGenTable;
	//TArrayView<FRHIRayTracingShader*> MissTable;
	//TArrayView<FRHIRayTracingShader*> HitGroupTable;
	//TArrayView<FRHIRayTracingShader*> CallableTable;

public:
	VulkanShaderBindings* m_pVulkanShaderBindings = nullptr;

};