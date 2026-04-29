#pragma once
#include <VulkanRHI1/Vulkan.h>
#include <VulkanRHI1/VulkanObjects/VulkanDevice.h>
#include <VulkanRHI1/VulkanObjects/VulkanCommandPool.h>
#include <VulkanRHI1/VulkanObjects/VulkanPipeline.h>
#include <VulkanRHI1/VulkanObjects/VulkanFence.h>
#include <VulkanRHI1/VulkanObjects/VulkanRaytracing.h>

#include <RHICommandBuffer.h>

#include <cstdint>

class VulkanRenderPass;
class VulkanFrameBuffer;
class VulkanPipeline;

/** \defgroup VulkanCommandBuffer
 *  @{
 */

 /**
 * @brief 命令缓冲区
 */
class VulkanCommandBuffer : public RHICommandBuffer
{
public:
	VulkanCommandBuffer();
	VulkanCommandBuffer(VulkanDevice* InDevice, VulkanCommandPool* InCommandPool);
	~VulkanCommandBuffer();
	VkCommandBuffer GetHandle() const;
public:
	/*
		Command Buffer
	*/	
	VkResult Begin(const VkCommandBufferUsageFlags& usage);
	VkResult End();
	VkResult Reset(VkCommandBufferResetFlags Flags);

	/*
		Pipeline Barrier
	*/
	void CmdPipelineBarrier2(const VkDependencyInfo* DependencyInfo);

	/*
		Render Pass
	*/
	void CmdBeginRenderPass(const VulkanRenderPass* renderPass, VulkanFrameBuffer* frameBuffer, VkSubpassContents Contents = VK_SUBPASS_CONTENTS_INLINE);
	void CmdEndRenderPass();
	void CmdNextSubpass(VkSubpassContents Contents);
	void CmdBeginRenderPass2(const VkRenderPassBeginInfo* RenderPassBegin, const VkSubpassBeginInfo* SubpassBeginInfo);
	void CmdEndRenderPass2(const VkSubpassEndInfo* SubpassEndInfo);
	void CmdNextSubpass2(const VkSubpassBeginInfo* SubpassBeginInfo, const VkSubpassEndInfo* SubpassEndInfo);

	/*
		Buffer
	*/
	void CmdFillBuffer(VkBuffer DstBuffer,
		VkDeviceSize DstOffset,
		VkDeviceSize Size,
		std::uint32_t Data);

	void CmdUpdateBuffer(VkBuffer DstBuffer,
		VkDeviceSize DstOffset,
		VkDeviceSize DataSize,
		const void* Data);

	void CmdCopyBuffer(VkBuffer SrcBuffer,
		VkBuffer DstBuffer,
		std::uint32_t RegionCount,
		const VkBufferCopy* Regions);

	void CmdCopyBuffer2(const VkCopyBufferInfo2* CopyBufferInfo);
	
	void CmdCopyBufferToImage(VkBuffer SrcBuffer,
		VkImage DstImage,
		VkImageLayout DstImageLayout,
		std::uint32_t RegionCount,
		const VkBufferImageCopy* Regions);

	void CmdCopyBufferToImage2(const VkCopyBufferToImageInfo2* CopyBufferToImageInfo);

	/*
		Image
	*/
	void CmdClearColorImage(VkImage Image,
		VkImageLayout ImageLayout,
		const VkClearColorValue* Color,
		std::uint32_t RangeCount,
		const VkImageSubresourceRange* Ranges);

	void CmdClearDepthStencilImage(VkImage Image,
		VkImageLayout ImageLayout,
		const VkClearDepthStencilValue* DepthStencil,
		std::uint32_t RangeCount,
		const VkImageSubresourceRange* Ranges);

	void CmdClearAttachments(uint32_t AttachmentCount,
		const VkClearAttachment* Attachments,
		std::uint32_t RectCount,
		const VkClearRect* Rects);

	void CmdCopyImage(VkImage SrcImage,
		VkImageLayout SrcImageLayout,
		VkImage DstImage,
		VkImageLayout DstImageLayout,
		std::uint32_t RegionCount,
		const VkImageCopy* Regions);

	void CmdCopyImage2(const VkCopyImageInfo2* CopyImageInfo);

	void CmdCopyImageToBuffer(VkImage SrcImage,
		VkImageLayout SrcImageLayout,
		VkBuffer DstBuffer,
		std::uint32_t RegionCount,
		const VkBufferImageCopy* Regions);

	void CmdCopyImageToBuffer2(const VkCopyImageToBufferInfo2* CopyImageToBufferInfo);

	void CmdBlitImage(VkImage SrcImage,
		VkImageLayout SrcImageLayout,
		VkImage DstImage,
		VkImageLayout DstImageLayout,
		std::uint32_t RegionCount,
		const VkImageBlit* Regions,
		VkFilter Filter);

	void CmdBlitImage2(const VkBlitImageInfo2* BlitImageInfo);

	void CmdResolveImage(VkImage srcImage,
		VkImageLayout SrcImageLayout,
		VkImage DstImage,
		VkImageLayout DstImageLayout,
		std::uint32_t RegionCount,
		const VkImageResolve* Regions);

	void CmdResolveImage2(const VkResolveImageInfo2* ResolveImageInfo);

	/*
		Draw Command
	*/  
	void CmdDrawIndirect(VkBuffer Buffer,
						 VkDeviceSize Offset,
						 std::uint32_t DrawCount,
						 std::uint32_t Stride);

	void CmdDrawIndexedIndirect(VkBuffer Buffer,
							    VkDeviceSize Offset,
							    std::uint32_t DrawCount,
							    std::uint32_t Stride);
	
	void CmdDispatch(std::uint32_t GroupCountX,
					 std::uint32_t GroupCountY,
					 std::uint32_t GroupCountZ);

	/*
		Pipeline
	*/ 
	void CmdBindDescriptorSets(VkPipelineBindPoint PipelineBindPoint, //ok
							   VkPipelineLayout Layout,
							   std::uint32_t FirstSet,
							   std::uint32_t DescriptorSetCount,
							   const VkDescriptorSet* DescriptorSets,
							   std::uint32_t DynamicOffsetCount,
							   const std::uint32_t* DynamicOffsets);



	/* Depth Test */
	void CmdSetDepthTestEnable(VkBool32 DepthTestEnable);

	void CmdSetDepthCompareOp(VkCompareOp DepthCompareOp);

	void CmdSetDepthWriteEnable(VkBool32 DepthWriteEnable);

	/* Scissor Test */
	void CmdSetScissor(std::uint32_t FirstScissor,
					   std::uint32_t ScissorCount,
					   const VkRect2D* Scissors);

	void CmdSetScissorWithCount(std::uint32_t ScissorCount, const VkRect2D* Scissors);

	/* Rasterizer */
	void CmdSetCullMode(VkCullModeFlags CullMode);

	void CmdSetFrontFace(VkFrontFace FrontFace);

	VkImageAspectFlags ComputeAspectFlagsForFormat(VkFormat format);
	VkAccessFlags GetAccessFlags(VkImageLayout layout);
	VkPipelineStageFlags GetPipelineStage(VkImageLayout layout);
	void SetImageMemoryBarrier(VkImage* image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
public: //virtual 
	virtual void BeginRecord();
	virtual void EndRecord();
	//virtual void BeginOneTimeGraphicsCommand();
	//virtual void EndOneTimeGraphicsCommand();

	virtual void SetImageBarrier(RHITexture* pTex, RHIPipelineStage srcStage, RHIPipelineStage dstStage, EImageLayout oldLayout, EImageLayout newLayout);

	void SetImageBarrier(VkImage& image, EImageLayout oldLayout, EImageLayout newLayout);

	//virtual void BeginRenderPass();
	virtual void BeginRenderPass(RHIRenderTarget* rt);
	virtual void BeginRenderPass(RHIRenderPass* pRenderPass, RHIFrameBuffer* pFrameBuffer, const std::vector<RHIClearValue>& clearValues);
	virtual void EndRenderPass();

	virtual void SetGraphicsPipeline(RHIGraphicsPipeline* pipeline);
	virtual void SetComputePipeline(RHIComputePipeline* pipeline);
	virtual void SetRaytracingPipeline(RHIRaytracingPipeline* pipeline);

	virtual void SetLineWidth(float lineWidth);

	virtual void SetViewport(float x, float y, float width, float height);
	virtual void SetScissor(int x, int y, int width, int height);

	virtual void BindVertexBuffers(const std::vector<RHIBuffer*>& buffers);
	virtual void BindIndexBuffer(RHIBuffer* buffer, EIndexType eIndexType);

	virtual void BindDescriptorSets();

	virtual void DrawArray(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
	virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance);

	virtual void Dispatch(std::uint32_t GroupCountX, std::uint32_t GroupCountY, std::uint32_t GroupCountZ);
	virtual void RaytraceDispatch(uint32_t width, uint32_t height, RHIShaderBindingTables* SBTs);

	virtual void BuildAccelerationStructures();
public:
	VulkanGraphicsPipeline* GraphicsPipeline = nullptr;
	VulkanComputePipeline* ComputePipeline = nullptr;
	VulkanRaytracingPipeline* RaytracingPipeline = nullptr;
	VulkanFence* Fence = nullptr;
private:
	VkCommandBuffer Handle = VK_NULL_HANDLE;
	VulkanDevice* Device = nullptr;
	VulkanCommandPool* CommandPool = nullptr;
};
/** @}*/