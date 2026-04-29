#pragma once
#include <RHIResource.h>
#include <RHIDefinitions.h>
#include <RHIVertexInput.h>
#include <RHIGraphicsPipelineState.h>
#include <RHIRenderPass.h>
#include <RHIShaderBindings.h>
#include <RHIShader.h>
#include <vector>

class RHIPipeline : public RHIResource
{
public:
	RHIPipeline() = default;
	 
}; 

class RHIGraphicsPipeline : public RHIPipeline
{
public: 
	RHIGraphicsPipeline() = default;
	 
	virtual void attachShaderModule(RHIShader* pShader) = 0;
	virtual void setVertexInput(RHIVertexInputInfo& info) = 0; 
	virtual void setInputAssembly(RHIPrimitiveType topo) = 0;
	virtual void setTessellation(RHITessellationInfo info) = 0;
	virtual void setViewport(RHIViewportInfo info) = 0;
	virtual void setRasterization(RHIRasterizationInfo info) = 0; 
	virtual void setMultiSampleCount(uint32_t sampleCount) = 0;
	virtual void setDepthStencil(RHIDepthStencilInfo info) = 0;
	virtual void setColorBlend(RHIColorBlendInfo info) = 0;
	virtual void setDynamic(RHIDynamicInfo info) = 0;

	virtual void setRenderPass(RHIRenderPass* pRenderPass) = 0;
	virtual void setShaderBindings(RHIShaderBindings* shaderBindings) = 0;
	virtual void create() = 0;

};

class RHIComputePipeline : public RHIPipeline
{
public:
	RHIComputePipeline() = default; 

	virtual void attachShaderModule(RHIShader* pShader) = 0;

	virtual void setShaderBindings(RHIShaderBindings* shaderBindings) = 0;
	virtual void create() = 0;
};

class RHIRaytracingPipeline : public RHIPipeline
{
public:
	RHIRaytracingPipeline() = default;

	virtual void attachShaderModule(RHIShader* pShader) = 0;
	virtual void setShaderBindings(RHIShaderBindings* shaderBindings) = 0;
	virtual RHIShaderBindings* getShaderBindings() = 0;
	virtual void create() = 0;
};





