#include <VulkanRHI/VulkanObjects/VulkanPipeline.h>
#include <VulkanRHI/VulkanCommon.h>
#include <iostream>
#include <VulkanRHI/Vulkan.h>
#include <VulkanRHI/VulkanLoader.h>

VulkanPipeline::VulkanPipeline()
	: Device(nullptr), Handle(VK_NULL_HANDLE)
{

}

VulkanPipeline::VulkanPipeline(VulkanDevice* Device)
	: Device(Device), Handle(VK_NULL_HANDLE)
{

}

VulkanPipeline::VulkanPipeline(VulkanDevice* Device, VkPipeline Handle)
	: Device(Device), Handle(Handle)
{

}

VulkanPipeline::~VulkanPipeline()
{
	//std::cout << "~VulkanPipeline() " << Handle << " PipelineLayout " << PipelineLayout << " PipelineCache " << PipelineCache << std::endl;
	if (Device)
	{
		if (VK_NULL_HANDLE != Handle)
			Device->DestroyPipeline(Handle, nullptr);
		if (VK_NULL_HANDLE != PipelineLayout)
			Device->DestroyPipelineLayout(PipelineLayout, nullptr);
		if (VK_NULL_HANDLE != PipelineCache)
			Device->DestroyPipelineCache(PipelineCache, nullptr);
	}
}

///////////////////////////////////////////////////VulkanGraphicsPipeline
VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice* InDevice)
	: VulkanPipeline(InDevice)
{
	//VkPipelineVertexInputStateCreateInfo	vertexInput{};
	//VkPipelineInputAssemblyStateCreateInfo  inputAssembly{};
	//VkPipelineTessellationStateCreateInfo	tessellation{};
	//VkPipelineViewportStateCreateInfo		viewportInfo{};

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = 800.0f; //(float)swapChainExtent.width;
	viewport.height = 600.0f;// (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = { 800,600 };

	viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO; 
	viewportInfo.viewportCount = 1;
	viewportInfo.pViewports = &viewport;
	viewportInfo.scissorCount = 1;
	viewportInfo.pScissors = &scissor;
	//VkPipelineRasterizationStateCreateInfo  rasterization{};
	//VkPipelineMultisampleStateCreateInfo	multisample{};
	//VkPipelineDepthStencilStateCreateInfo	depthStencil{};
 
	//VkPipelineColorBlendStateCreateInfo		colorBlend{};

	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;  

	colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlend.logicOpEnable = VK_FALSE;
	colorBlend.logicOp = VK_LOGIC_OP_COPY;
	colorBlend.attachmentCount = 1;
	colorBlend.pAttachments = &colorBlendAttachment;
	colorBlend.blendConstants[0] = 0.0f;
	colorBlend.blendConstants[1] = 0.0f;
	colorBlend.blendConstants[2] = 0.0f;
	colorBlend.blendConstants[3] = 0.0f;
	//VkPipelineDynamicStateCreateInfo		dynamic{};

	dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR/*,VK_DYNAMIC_STATE_DEPTH_COMPARE_OP*/ };
	dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
	dynamic.pDynamicStates = dynamicStateEnables.data();
	dynamic.flags = 0;
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
}

void VulkanGraphicsPipeline::attachShaderModule(RHIShader* pShader)
{
	VkPipelineShaderStageCreateInfo pipelineShaderStageInfo = {};
	pipelineShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageInfo.pNext = nullptr;
	RHIShaderStage Stage = pShader->GetShaderType();

	pipelineShaderStageInfo.stage = RHIShaderStageToVKStageBit2((RHIShaderStage)Stage);
	printf("%d\n", pipelineShaderStageInfo.stage);
	if (SS_Vertex == Stage) {
		VulkanVertexShader* pVertexShader = static_cast<VulkanVertexShader*>(pShader);
		pipelineShaderStageInfo.module = pVertexShader->Module;
	}
	else if (SS_Pixel == Stage) {
		VulkanPixelShader* pPixelShader = static_cast<VulkanPixelShader*>(pShader);
		pipelineShaderStageInfo.module = pPixelShader->Module;
	}
	else if (SS_Geometry == Stage) {
		VulkanGeometryShader* pGeometryShader = static_cast<VulkanGeometryShader*>(pShader);
		pipelineShaderStageInfo.module = pGeometryShader->Module;
	} 
	else {
		printf("todo: not support Stage %d\n", Stage);
	}
	pipelineShaderStageInfo.pName = "main";
	//pipelineShaderStageInfo.pSpecializationInfo = specialInfo;
	shaderStages.emplace_back(std::move(pipelineShaderStageInfo));
}

void VulkanGraphicsPipeline::setVertexInput(RHIVertexInputInfo& info) 
{ 
	attributes.clear();
	bindingDescriptions.clear(); 
	vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO; 


	vertexInput.vertexAttributeDescriptionCount = 0;
	vertexInput.vertexBindingDescriptionCount = 0;
	vertexInput.pVertexAttributeDescriptions = nullptr;
	vertexInput.pVertexBindingDescriptions = nullptr;

	uint32_t size = info.attributes.size(); 
	VkVertexInputAttributeDescription AttributeDesc = {};
	if (size > 0) {
		for (const auto &attr : info.attributes) { 
			AttributeDesc.location = attr.location;
			AttributeDesc.binding = attr.binding; 
			AttributeDesc.format = ToVkBufferFormat(attr.format); 
			AttributeDesc.offset = attr.offset;
			attributes.push_back(AttributeDesc);
		} 
		printf("VkVertexInputAttributeDescription %d\n", size);
		vertexInput.vertexAttributeDescriptionCount = size;
		vertexInput.pVertexAttributeDescriptions = attributes.data();
	}
	
	size = info.bindings.size();
	VkVertexInputBindingDescription BindingDesc = {};
	if (size > 0) { 
		for (const auto &binding : info.bindings) {
			BindingDesc.binding = 0;
			BindingDesc.stride = binding.stride;
			BindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;// (binding.cls == RHIVertexInputBinding::Classification::PerVertex) ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE;
			bindingDescriptions.push_back(BindingDesc);
		}	 
		printf("VkVertexInputBindingDescription %d\n", size);
		vertexInput.vertexBindingDescriptionCount = size;
		vertexInput.pVertexBindingDescriptions = bindingDescriptions.data();
	} 
}

void VulkanGraphicsPipeline::setInputAssembly(RHIPrimitiveType topo) {
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = ToVkPrimitiveTopology(topo);
	inputAssembly.primitiveRestartEnable = VK_FALSE;
}

void VulkanGraphicsPipeline::setTessellation(RHITessellationInfo info) {
	//todo...
}

void VulkanGraphicsPipeline::setViewport(RHIViewportInfo info)
{
	//viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	//viewportInfo.flags = 0;
	//viewportInfo.viewportCount = 1;
	//VkViewport viewport = { 0,0,800,600,0,0 };
	//viewportInfo.pViewports = &viewport;
	//viewportInfo.scissorCount = 1;
	//VkRect2D rect;
	//viewportInfo.pScissors = &rect; 
}

void VulkanGraphicsPipeline::setRasterization(RHIRasterizationInfo info) {
	rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO; 
	rasterization.depthClampEnable = VK_FALSE;
	rasterization.rasterizerDiscardEnable = VK_FALSE;
	rasterization.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization.lineWidth = 1.0f;
	rasterization.depthBiasEnable = VK_FALSE; 
	rasterization.cullMode = info.cullMode; 
	rasterization.frontFace = toVkFrontFace(info.frontFace);
}

void VulkanGraphicsPipeline::setMultiSampleCount(uint32_t sampleCount) {
	multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample.rasterizationSamples = toVkMultisampleCount(sampleCount);//VK_SAMPLE_COUNT_1_BIT
	//multisample.sampleShadingEnable = VK_TRUE;
	//multisample.minSampleShading = .2f;
	multisample.flags = 0;
}

void VulkanGraphicsPipeline::setDepthStencil(RHIDepthStencilInfo info) {
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = info.depthTestEnable;
	depthStencil.depthWriteEnable = info.depthWriteEnable;
	if (info.eDepthCmpFunc == ECompareFunction::CF_Less)
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;// VK_COMPARE_OP_LESS;
	else if (info.eDepthCmpFunc == ECompareFunction::CF_LessEqual)
		depthStencil.depthCompareOp = VK_COMPARE_OP_EQUAL;
	else
		depthStencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;

	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;
}

void VulkanGraphicsPipeline::setColorBlend(RHIColorBlendInfo info) {
	int colorAttachmentCount = info.colorAttachmentCount;

	ColorBlendAttachments.clear();
	for (int i = 0; i < colorAttachmentCount; i++) {
		VkPipelineColorBlendAttachmentState ColorBlendAttachment = {}; 	
		ColorBlendAttachment.blendEnable = VK_FALSE;
		ColorBlendAttachment.colorWriteMask = 0xF;  
		ColorBlendAttachments.push_back(ColorBlendAttachment);
	} 
	colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlend.attachmentCount = colorAttachmentCount;
	colorBlend.pAttachments = ColorBlendAttachments.data();
	//colorBlend.logicOpEnable = VK_FALSE; 
	//colorBlend.logicOp = VK_LOGIC_OP_COPY;
	//colorBlend.blendConstants[0] = 0.0f;
	//colorBlend.blendConstants[1] = 0.0f;
	//colorBlend.blendConstants[2] = 0.0f;
	//colorBlend.blendConstants[3] = 0.0f;
}

void VulkanGraphicsPipeline::setDynamic(RHIDynamicInfo info)
{//todo...
	std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR }; 
	dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic.pDynamicStates = dynamicStateEnables.data();
	dynamic.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
	dynamic.flags = 0; 
}

void VulkanGraphicsPipeline::CreatePipelineLayout()
{ 
	VkDescriptorSetLayout DescriptorSetLayout = m_pVulkanShaderBindings->GetDescriptorSetLayout();

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &DescriptorSetLayout;
	Device->CreatePipelineLayout(&pipelineLayoutCreateInfo, nullptr, &PipelineLayout);
}

void VulkanGraphicsPipeline::create()
{
	CreatePipelineLayout(); 

	VkGraphicsPipelineCreateInfo Info = {};
	Info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	Info.pNext = nullptr;

	Info.stageCount = shaderStages.size();
	Info.pStages = shaderStages.data();

	Info.pVertexInputState = &vertexInput;
	Info.pInputAssemblyState = &inputAssembly;
	//Info.pTessellationState = &tessellation;
	Info.pViewportState = &viewportInfo;
	Info.pRasterizationState = &rasterization;
	Info.pMultisampleState = &multisample;
	Info.pDepthStencilState = &depthStencil;
	Info.pColorBlendState = &colorBlend;
	Info.pDynamicState = &dynamic;

	Info.layout = PipelineLayout;
	Info.renderPass = pRenderPass->Handle;
	Info.subpass = 0;
	Info.basePipelineHandle = VK_NULL_HANDLE;
	Info.basePipelineIndex = -1;

	//PipelineCache
	if (Device->CreateGraphicsPipelines(nullptr, 1, &Info, nullptr, &Handle) != VK_SUCCESS) {
		printf("pipeline create failed\n");
	} 
}


//////////////////////////////////ComputePipeline
VulkanComputePipeline::VulkanComputePipeline(VulkanDevice* InDevice)
{
	Device = InDevice;
	BindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
}

VulkanComputePipeline::~VulkanComputePipeline()
{

}

void VulkanComputePipeline::attachShaderModule(RHIShader* pShader)
{	
	RHIShaderStage Stage = pShader->GetShaderType();
	VulkanComputeShader* pComputeShader = static_cast<VulkanComputeShader*>(pShader);

	shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStage.pNext = nullptr; 
	shaderStage.stage = RHIShaderStageToVKStageBit2((RHIShaderStage)Stage); 
	shaderStage.module = pComputeShader->Module;
	shaderStage.pName = "main"; 
}

void VulkanComputePipeline::CreatePipelineLayout()
{
	VkDescriptorSetLayout DescriptorSetLayout = m_pVulkanShaderBindings->GetDescriptorSetLayout();

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &DescriptorSetLayout;
	Device->CreatePipelineLayout(&pipelineLayoutCreateInfo, nullptr, &PipelineLayout);
}

void VulkanComputePipeline::create()
{  
	CreatePipelineLayout();

	VkComputePipelineCreateInfo Info = {}; 
	Info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	Info.pNext = nullptr;
	Info.layout = PipelineLayout;
	Info.flags = 0;
	Info.stage = shaderStage;  
	if (Device->CreateComputePipelines(nullptr, 1, &Info, nullptr, &Handle) != VK_SUCCESS) {
		printf("ComputePipeline create failed\n");
	}
}
 
//static VkGeometryInstanceFlagsKHR toVkRayTracingInstanceFlags(ERayTracingInstanceFlags InFlags)
//{
//	VkGeometryInstanceFlagsKHR Result = 0;
//	if (EnumHasAnyFlags(InFlags, ERayTracingInstanceFlags::TriangleCullDisable)) {
//		Result |= VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
//	}
//	if (!EnumHasAnyFlags(InFlags, ERayTracingInstanceFlags::TriangleCullReverse)) {
//		Result |= VK_GEOMETRY_INSTANCE_TRIANGLE_FRONT_COUNTERCLOCKWISE_BIT_KHR;		// Counterclockwise is the default 
//	}
//	if (EnumHasAnyFlags(InFlags, ERayTracingInstanceFlags::ForceOpaque)) {
//		Result |= VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_KHR;
//	}
//	if (EnumHasAnyFlags(InFlags, ERayTracingInstanceFlags::ForceNonOpaque)) {
//		Result |= VK_GEOMETRY_INSTANCE_FORCE_NO_OPAQUE_BIT_KHR;
//	}
//	return Result;
//}


VulkanRaytracingPipeline::VulkanRaytracingPipeline(VulkanDevice* InDevice)
	: VulkanPipeline(InDevice)
{ 
	BindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
}

VulkanRaytracingPipeline::~VulkanRaytracingPipeline()
{

}

void VulkanRaytracingPipeline::attachShaderModule(RHIShader* pShader)
{
	VkRayTracingShaderGroupTypeKHR sgType = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;

	VkPipelineShaderStageCreateInfo pipelineShaderStageInfo = {};
	pipelineShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderStageInfo.pNext = nullptr;
	RHIShaderStage Stage = pShader->GetShaderType();

	pipelineShaderStageInfo.stage = RHIShaderStageToVKStageBit2((RHIShaderStage)Stage);
	if (SS_RayGen == Stage) {
		VulkanRayGenShader* pVkShader = static_cast<VulkanRayGenShader*>(pShader);
		pipelineShaderStageInfo.module = pVkShader->Module;
	}
	else if (SS_RayMiss == Stage) {
		VulkanRayMissShader* pVkShader = static_cast<VulkanRayMissShader*>(pShader);
		pipelineShaderStageInfo.module = pVkShader->Module;
		missTableCount++;
	}
	else if (SS_RayHitGroup == Stage) {
		VulkanRayHitGroupShader* pVkShader = static_cast<VulkanRayHitGroupShader*>(pShader);
		pipelineShaderStageInfo.module = pVkShader->Module;  
		hitgroupTableCount++;
	}
	else {
		printf("todo: not support Stage %d\n", Stage);
	}
	pipelineShaderStageInfo.pName = "main";
	//pipelineShaderStageInfo.pSpecializationInfo = specialInfo;
	//shaderStages.emplace_back(std::move(pipelineShaderStageInfo));
	if(pipelineShaderStageInfo.module != VK_NULL_HANDLE)
		shaderStages.push_back( pipelineShaderStageInfo );

	VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
	shaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR; 
	shaderGroup.pNext = nullptr;	
	shaderGroup.pShaderGroupCaptureReplayHandle = nullptr;
	if (SS_RayGen == Stage || SS_RayMiss == Stage) {
		shaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		shaderGroup.generalShader = static_cast<uint32_t>(shaderStages.size()) - 1;
		shaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
		shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
		shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
	}
	else {
		shaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
		shaderGroup.generalShader = VK_SHADER_UNUSED_KHR;
		VulkanRayHitGroupShader* pVkShader = static_cast<VulkanRayHitGroupShader*>(pShader);

		if (pVkShader->Module) {
			shaderGroup.closestHitShader = static_cast<uint32_t>(shaderStages.size()) - 1; 
		} 
		else {
			shaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR; 
		}


		if (pVkShader->ModuleAnyHit) {  
			pipelineShaderStageInfo.stage = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
			pipelineShaderStageInfo.module = pVkShader->ModuleAnyHit;  
			shaderStages.push_back(pipelineShaderStageInfo);
			shaderGroup.anyHitShader = static_cast<uint32_t>(shaderStages.size()) - 1;
		}
		else {
			shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
		}
			 
		shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
	}
	shaderGroups.push_back(shaderGroup);
}

void VulkanRaytracingPipeline::CreatePipelineLayout()
{
	VkDescriptorSetLayout DescriptorSetLayout = m_pVulkanShaderBindings->GetDescriptorSetLayout();

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &DescriptorSetLayout;

	//todo... 暂时不支持Constant,建议都用UBO或SSBO
	//pipelineLayoutCreateInfo.pushConstantRangeCount = 0; 
	//pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	Device->CreatePipelineLayout(&pipelineLayoutCreateInfo, nullptr, &PipelineLayout);
}

void VulkanRaytracingPipeline::create()
{ 	
	CreatePipelineLayout();
	VkRayTracingPipelineCreateInfoKHR rtInfo{};
	rtInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
	rtInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
	rtInfo.pStages = shaderStages.data();
	rtInfo.groupCount = static_cast<uint32_t>(shaderGroups.size());
	rtInfo.pGroups = shaderGroups.data();
	rtInfo.maxPipelineRayRecursionDepth = 2;
	rtInfo.layout = PipelineLayout;
	Device->CreateRayTracingPipelinesKHR(VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &rtInfo, nullptr, &Handle);


#if 0
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;


	// Ray generation group
	{
		shaderStages.push_back(loadShader(getShadersPath() + "raytracingbasic/raygen.rgen.spv", VK_SHADER_STAGE_RAYGEN_BIT_KHR));
		VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
		shaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		shaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		shaderGroup.generalShader = static_cast<uint32_t>(shaderStages.size()) - 1;
		shaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
		shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
		shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
		shaderGroups.push_back(shaderGroup);
	}

	// Miss group
	{
		shaderStages.push_back(loadShader(getShadersPath() + "raytracingbasic/miss.rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR));
		VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
		shaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		shaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		shaderGroup.generalShader = static_cast<uint32_t>(shaderStages.size()) - 1;
		shaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
		shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
		shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
		shaderGroups.push_back(shaderGroup);
	}

	// Closest hit group
	{
		shaderStages.push_back(loadShader(getShadersPath() + "raytracingbasic/closesthit.rchit.spv", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR));
		VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
		shaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		shaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
		shaderGroup.generalShader = VK_SHADER_UNUSED_KHR;
		shaderGroup.closestHitShader = static_cast<uint32_t>(shaderStages.size()) - 1;
		shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
		shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
		shaderGroups.push_back(shaderGroup);
	}
	/*
	Create the ray tracing pipeline
	*/
	VkRayTracingPipelineCreateInfoKHR rayTracingPipelineCI{};
	rayTracingPipelineCI.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
	rayTracingPipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
	rayTracingPipelineCI.pStages = shaderStages.data();
	rayTracingPipelineCI.groupCount = static_cast<uint32_t>(shaderGroups.size());
	rayTracingPipelineCI.pGroups = shaderGroups.data();
	rayTracingPipelineCI.maxPipelineRayRecursionDepth = 1; //最大递归深度
	rayTracingPipelineCI.layout = pipelineLayout;
	vkCreateRayTracingPipelinesKHR(Device->Handle, VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &rayTracingPipelineCI, nullptr, &Handle);

#endif
}