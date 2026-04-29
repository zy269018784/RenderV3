#pragma once 
#include <RHIPipeline.h>
#include <RHIFrameBuffer.h>
#include <RHIRenderTarget.h>
#include <RHITexture.h>
#include <RHIRaytracing.h>

enum class RHIAccess;

class RHICommandBuffer {
public:
	virtual void BeginRecord() = 0;
	virtual void EndRecord() = 0;
	//virtual void BeginOneTimeGraphicsCommand() = 0;
	//virtual void EndOneTimeGraphicsCommand() = 0;
	virtual void SetImageBarrier(RHITexture* pTex, RHIPipelineStage srcStage, RHIPipelineStage dstStage, EImageLayout oldLayout, EImageLayout newLayout) = 0;

	//virtual void BeginRenderPass() = 0; //获取内部默认数据
	virtual void BeginRenderPass(RHIRenderTarget* rt) = 0;
	virtual void BeginRenderPass(RHIRenderPass* pRenderPass,RHIFrameBuffer* pFrameBuffer, const std::vector<RHIClearValue>& clearValues) = 0;
	virtual void EndRenderPass() = 0; 	
	
	virtual void SetGraphicsPipeline(RHIGraphicsPipeline *pipeline) = 0;
	virtual void SetComputePipeline(RHIComputePipeline* pipeline) = 0;
	virtual void SetRaytracingPipeline(RHIRaytracingPipeline* pipeline) = 0;

	virtual void SetLineWidth(float lineWidth) = 0;
	virtual void SetViewport(float x, float y, float width, float height) = 0;
	virtual void SetScissor(int x, int y, int width, int height) = 0;

	virtual void BindVertexBuffers(const std::vector<RHIBuffer*>& buffers) = 0;
	virtual void BindIndexBuffer(RHIBuffer* buffer, EIndexType eIndexType) = 0;
	 
	virtual void BindDescriptorSets() = 0;

	virtual void DrawArray(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
	virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) = 0;

	virtual void Dispatch(std::uint32_t GroupCountX, std::uint32_t GroupCountY, std::uint32_t GroupCountZ) = 0;
	virtual void RaytraceDispatch(uint32_t width,uint32_t height,RHIShaderBindingTables* SBTs) = 0;

	//virtual void ImageMemoryBarrier(RHITexture* tex, EImageLayout oldLayout, EImageLayout newLayout) = 0;
};