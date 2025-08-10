#include <VulkanRHI/VulkanObjects/VulkanCommandBuffer.h>
#include <VulkanRHI/VulkanObjects/VulkanRenderPass.h>
#include <VulkanRHI/VulkanObjects/VulkanFrameBuffer.h>
#include <VulkanRHI/VulkanObjects/VulkanPipeline.h>
#include <VulkanRHI/VulkanObjects/VulkanBuffer.h>
#include <VulkanRHI/VulkanLoader.h>
#include <RHI.h>
#include <iostream>

VulkanCommandBuffer::VulkanCommandBuffer()
{

}

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* InDevice, VulkanCommandPool* InCommandPool)
	: Device(InDevice), CommandPool(InCommandPool)
{	
	VkCommandBufferAllocateInfo AllocateInfo{};
	AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	AllocateInfo.commandPool = CommandPool->GetHandle();
	AllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	AllocateInfo.commandBufferCount = 1;
	if (!CommandPool)
		std::cout << "CommandPool is nullptr" << std::endl;

	if (!CommandPool->Device)
		std::cout << "CommandPool  Device is nullptr" << std::endl;
#if 1
	VkResult Result = Device->AllocateCommandBuffers(&AllocateInfo, &Handle);
	if (VK_SUCCESS != Result)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}

#else
	/*
		会崩溃原因不明确
	*/
	std::cout << "AllocateCommandBuffers  1" << std::endl;
	VkResult Result = CommandPool->AllocateCommandBuffers(&AllocateInfo, &Handle);
	if (VK_SUCCESS != Result)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
	std::cout << "AllocateCommandBuffers  2" << std::endl;
#endif
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
	CommandPool->FreeCommandBuffers(1, &Handle);
}


VkCommandBuffer VulkanCommandBuffer::GetHandle() const
{
	return Handle;
}

/*
	Command Buffer
*/
VkResult VulkanCommandBuffer::Begin(const VkCommandBufferUsageFlags &usage) //ok
{
	VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
	cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBufferBeginInfo.pNext = nullptr;
	cmdBufferBeginInfo.pInheritanceInfo = nullptr;
	cmdBufferBeginInfo.flags = usage;
	return vkBeginCommandBuffer(Handle, &cmdBufferBeginInfo);
}

VkResult VulkanCommandBuffer::End()
{
	return vkEndCommandBuffer(Handle);
}

VkResult VulkanCommandBuffer::Reset(VkCommandBufferResetFlags Flags)
{
	return vkResetCommandBuffer(Handle, Flags);
}

/*
	Pipeline Barrier
*/
void VulkanCommandBuffer::CmdPipelineBarrier2(const VkDependencyInfo* DependencyInfo)
{
	vkCmdPipelineBarrier2(Handle, DependencyInfo);
}

/*
	Render Pass
*/
void VulkanCommandBuffer::CmdBeginRenderPass(const VulkanRenderPass *renderPass, VulkanFrameBuffer *frameBuffer, VkSubpassContents Contents/* = VK_SUBPASS_CONTENTS_INLINE*/)
{
	std::vector<VkClearValue> clearValues(1);
	for (size_t i = 0; i < clearValues.size(); ++i) {
		clearValues[i].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	}

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = renderPass->Handle;
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = frameBuffer->getExtent();
	renderPassBeginInfo.framebuffer = frameBuffer->Handle;
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();
	 
	Contents = VK_SUBPASS_CONTENTS_INLINE;
	vkCmdBeginRenderPass(Handle, &renderPassBeginInfo, Contents);
}

void VulkanCommandBuffer::CmdEndRenderPass()
{
	vkCmdEndRenderPass(Handle);
}

void VulkanCommandBuffer::CmdNextSubpass(VkSubpassContents Contents)
{
	vkCmdNextSubpass(Handle, Contents);
}

void VulkanCommandBuffer::CmdBeginRenderPass2(const VkRenderPassBeginInfo* RenderPassBegin, const VkSubpassBeginInfo* SubpassBeginInfo)
{
	vkCmdBeginRenderPass2(Handle, RenderPassBegin, SubpassBeginInfo);
}

void VulkanCommandBuffer::CmdEndRenderPass2(const VkSubpassEndInfo* SubpassEndInfo)
{
	vkCmdEndRenderPass2(Handle, SubpassEndInfo);
}

void VulkanCommandBuffer::CmdNextSubpass2(const VkSubpassBeginInfo* SubpassBeginInfo, const VkSubpassEndInfo* SubpassEndInfo)
{
	vkCmdNextSubpass2(Handle, SubpassBeginInfo, SubpassEndInfo);
}

/*
	Buffer
*/
void VulkanCommandBuffer::CmdFillBuffer(VkBuffer DstBuffer,
	VkDeviceSize DstOffset,
	VkDeviceSize Size,
	std::uint32_t Data)
{
	vkCmdFillBuffer(Handle, DstBuffer, DstOffset, Size, Data);
}

void VulkanCommandBuffer::CmdUpdateBuffer(VkBuffer DstBuffer,
	VkDeviceSize DstOffset,
	VkDeviceSize DataSize,
	const void* Data)
{
	vkCmdUpdateBuffer(Handle, DstBuffer, DstOffset, DataSize, Data);
}

void VulkanCommandBuffer::CmdCopyBuffer(VkBuffer SrcBuffer,
	VkBuffer DstBuffer,
	std::uint32_t RegionCount,
	const VkBufferCopy* Regions)
{
	vkCmdCopyBuffer(Handle, SrcBuffer, DstBuffer, RegionCount, Regions);
}

void VulkanCommandBuffer::CmdCopyBuffer2(const VkCopyBufferInfo2* CopyBufferInfo)
{
	vkCmdCopyBuffer2(Handle, CopyBufferInfo);
}

void VulkanCommandBuffer::CmdCopyBufferToImage(VkBuffer SrcBuffer,
	VkImage DstImage,
	VkImageLayout DstImageLayout,
	std::uint32_t RegionCount,
	const VkBufferImageCopy* Regions)
{
	vkCmdCopyBufferToImage(Handle, SrcBuffer, DstImage, DstImageLayout, RegionCount, Regions);
}

void VulkanCommandBuffer::CmdCopyBufferToImage2(const VkCopyBufferToImageInfo2* CopyBufferToImageInfo)
{
	vkCmdCopyBufferToImage2(Handle, CopyBufferToImageInfo);
}

/*
	Image
*/
void VulkanCommandBuffer::CmdClearColorImage(VkImage Image,
	VkImageLayout ImageLayout,
	const VkClearColorValue* Color,
	uint32_t RangeCount,
	const VkImageSubresourceRange* Ranges)
{
	vkCmdClearColorImage(Handle, Image, ImageLayout, Color, RangeCount, Ranges);
}

void VulkanCommandBuffer::CmdClearDepthStencilImage(VkImage Image,
	VkImageLayout ImageLayout,
	const VkClearDepthStencilValue* DepthStencil,
	std::uint32_t RangeCount,
	const VkImageSubresourceRange* Ranges)
{
	vkCmdClearDepthStencilImage(Handle, Image, ImageLayout, DepthStencil, RangeCount, Ranges);
}

void VulkanCommandBuffer::CmdClearAttachments(std::uint32_t AttachmentCount,
	const VkClearAttachment* Attachments,
	std::uint32_t RectCount,
	const VkClearRect* Rects)
{
	vkCmdClearAttachments(Handle, AttachmentCount, Attachments, RectCount, Rects);
}

void VulkanCommandBuffer::CmdCopyImage(VkImage SrcImage,
	VkImageLayout SrcImageLayout,
	VkImage DstImage,
	VkImageLayout DstImageLayout,
	std::uint32_t RegionCount,
	const VkImageCopy* Regions)
{
	vkCmdCopyImage(Handle, SrcImage, SrcImageLayout, DstImage, DstImageLayout, RegionCount, Regions);
}

void VulkanCommandBuffer::CmdCopyImage2(const VkCopyImageInfo2* CopyImageInfo)
{
	vkCmdCopyImage2(Handle, CopyImageInfo);
}

void VulkanCommandBuffer::CmdCopyImageToBuffer(VkImage SrcImage,
	VkImageLayout SrcImageLayout,
	VkBuffer DstBuffer,
	std::uint32_t RegionCount,
	const VkBufferImageCopy* Regions)
{
	vkCmdCopyImageToBuffer(Handle, SrcImage, SrcImageLayout, DstBuffer, RegionCount, Regions);
}

void VulkanCommandBuffer::CmdCopyImageToBuffer2(const VkCopyImageToBufferInfo2* CopyImageToBufferInfo)
{
	vkCmdCopyImageToBuffer2(Handle, CopyImageToBufferInfo);
}

void VulkanCommandBuffer::CmdBlitImage(VkImage SrcImage,
	VkImageLayout SrcImageLayout,
	VkImage DstImage,
	VkImageLayout DstImageLayout,
	std::uint32_t RegionCount,
	const VkImageBlit* Regions,
	VkFilter Filter)
{

}

void VulkanCommandBuffer::CmdBlitImage2(const VkBlitImageInfo2* BlitImageInfo)
{

}

void VulkanCommandBuffer::CmdResolveImage(VkImage SrcImage,
	VkImageLayout SrcImageLayout,
	VkImage DstImage,
	VkImageLayout DstImageLayout,
	std::uint32_t RegionCount,
	const VkImageResolve* Regions)
{
	vkCmdResolveImage(Handle, SrcImage, SrcImageLayout, DstImage, DstImageLayout, RegionCount, Regions);
}

void VulkanCommandBuffer::CmdResolveImage2(const VkResolveImageInfo2* ResolveImageInfo)
{
	vkCmdResolveImage2(Handle, ResolveImageInfo);
}

/*
	Draw Command
*/  

void VulkanCommandBuffer::CmdDrawIndirect(VkBuffer Buffer,
										  VkDeviceSize Offset,
										  std::uint32_t DrawCount,
										  std::uint32_t Stride)
{
	vkCmdDrawIndirect(Handle, Buffer, Offset, DrawCount, Stride);
}

void VulkanCommandBuffer::CmdDrawIndexedIndirect(VkBuffer Buffer,
	VkDeviceSize Offset,
	std::uint32_t DrawCount,
	std::uint32_t Stride)
{
	vkCmdDrawIndexedIndirect(Handle, Buffer, Offset, DrawCount, Stride);
}

void VulkanCommandBuffer::CmdDispatch(std::uint32_t GroupCountX,
									  std::uint32_t GroupCountY,
									  std::uint32_t GroupCountZ)
{
	vkCmdDispatch(Handle, GroupCountX, GroupCountY, GroupCountZ);
}

/*
	Pipeline
*/ 

void VulkanCommandBuffer::CmdBindDescriptorSets(VkPipelineBindPoint PipelineBindPoint,
	VkPipelineLayout Layout,
	std::uint32_t FirstSet,
	std::uint32_t DescriptorSetCount,
	const VkDescriptorSet* DescriptorSets,
	std::uint32_t DynamicOffsetCount,
	const std::uint32_t* DynamicOffsets)
{
	vkCmdBindDescriptorSets(Handle, PipelineBindPoint, Layout, FirstSet, DescriptorSetCount, DescriptorSets, DynamicOffsetCount, DynamicOffsets);
}

/* Depth Test */
void VulkanCommandBuffer::CmdSetDepthTestEnable(VkBool32 DepthTestEnable)
{
	vkCmdSetDepthTestEnable(Handle, DepthTestEnable);
}

void VulkanCommandBuffer::CmdSetDepthCompareOp(VkCompareOp DepthCompareOp)
{
	vkCmdSetDepthCompareOp(Handle, DepthCompareOp);
}

void VulkanCommandBuffer::CmdSetDepthWriteEnable(VkBool32 DepthWriteEnable)
{
	vkCmdSetDepthWriteEnable(Handle, DepthWriteEnable);
}

/* Scissor Test */
void VulkanCommandBuffer::CmdSetScissor(std::uint32_t FirstScissor,
				   std::uint32_t ScissorCount,
				   const VkRect2D* Scissors)
{
	vkCmdSetScissor(Handle, FirstScissor, ScissorCount, Scissors);
}



void VulkanCommandBuffer::CmdSetScissorWithCount(std::uint32_t ScissorCount, const VkRect2D* Scissors)
{
	vkCmdSetScissorWithCount(Handle, ScissorCount, Scissors);
}

/* Rasterizer */
void VulkanCommandBuffer::CmdSetCullMode(VkCullModeFlags CullMode)
{

}

void VulkanCommandBuffer::CmdSetFrontFace(VkFrontFace FrontFace)
{

} 
 

////////////////////////////////////////////////virtual
void VulkanCommandBuffer::BeginRecord()
{
	VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
	cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBufferBeginInfo.pNext = nullptr;
	cmdBufferBeginInfo.pInheritanceInfo = nullptr;
	cmdBufferBeginInfo.flags = 0;// VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;// usage;// VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
	vkBeginCommandBuffer(Handle, &cmdBufferBeginInfo); 
}

void VulkanCommandBuffer::EndRecord()
{
	vkEndCommandBuffer(Handle);
}

//void VulkanCommandBuffer::BeginOneTimeGraphicsCommand()
//{
//	VkCommandBufferAllocateInfo allocInfo = {};
//	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//	allocInfo.commandPool = Device->m_CommandPool;// commandPool;
//	allocInfo.commandBufferCount = 1;
//	 
//	vkAllocateCommandBuffers(Device->Handle, &allocInfo, &Handle);
//	VkCommandBufferBeginInfo beginInfo = {};
//	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//	vkBeginCommandBuffer(Handle, &beginInfo); 
//}
//
//void VulkanCommandBuffer::EndOneTimeGraphicsCommand()
//{
//	vkEndCommandBuffer(Handle);
//	VkSubmitInfo submitInfo = {};
//	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//	submitInfo.commandBufferCount = 1;
//	submitInfo.pCommandBuffers = &Handle;
//	vkQueueSubmit(Device->m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
//	vkQueueWaitIdle(Device->m_graphicsQueue);
//	vkFreeCommandBuffers(Device->Handle, Device->m_CommandPool, 1, &Handle);
//}

static VkAccessFlags GetVkAccessMaskForLayout(const VkImageLayout Layout)
{
	VkAccessFlags Flags = 0; 
	switch (Layout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		Flags = VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		Flags = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		Flags = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
	case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
	case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
		Flags = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
	case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
		Flags = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		Flags = VK_ACCESS_SHADER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
	case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
	case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
		Flags = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		Flags = 0;
		break;

	case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
		Flags = VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;
		break;

	case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
		Flags = VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
		break;

	case VK_IMAGE_LAYOUT_GENERAL:
		// todo-jn: could be used for R64 in read layout
	case VK_IMAGE_LAYOUT_UNDEFINED:
		Flags = 0;
		break;

	case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL:
		// todo-jn: sync2 currently only used by depth/stencil targets
		Flags = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break; 
	case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL:
		// todo-jn: sync2 currently only used by depth/stencil targets
		Flags = VK_ACCESS_SHADER_READ_BIT;
		break; 
	default: 
		break;
	} 
	return Flags;
}

VkAccessFlags getVkAccessMask(RHIAccess access) {
	switch (access)
	{
	case RHIAccess::Unknown:
		break;
	case RHIAccess::CPURead:
		break;
	case RHIAccess::Present:
		break;
	case RHIAccess::IndirectArgs:
		break;
	case RHIAccess::VertexOrIndexBuffer:
		break;
	case RHIAccess::SRVCompute:
		break;
	case RHIAccess::SRVGraphics:
		break;
	case RHIAccess::CopySrc:
		break;
	case RHIAccess::ResolveSrc:
		break;
	case RHIAccess::DSVRead:
		break;
	case RHIAccess::UAVCompute:
		break;
	case RHIAccess::UAVGraphics:
		break;
	case RHIAccess::RTV:
		break;
	case RHIAccess::CopyDest:
		break;
	case RHIAccess::ResolveDst:
		break;
	case RHIAccess::DSVWrite:
		break;
	case RHIAccess::BVHRead:
		break;
	case RHIAccess::BVHWrite:
		break;
	case RHIAccess::Discard:
		break;
	case RHIAccess::ShadingRateSource:
		break; 
	case RHIAccess::Mask:
		break;
	case RHIAccess::SRVMask:
		break;
	case RHIAccess::UAVMask:
		break;
	case RHIAccess::ReadOnlyExclusiveMask:
		break;
	case RHIAccess::ReadOnlyMask:
		return VK_ACCESS_SHADER_READ_BIT; 
	case RHIAccess::ReadableMask:
		break;
	case RHIAccess::WriteOnlyExclusiveMask:
		break;
	case RHIAccess::WriteOnlyMask:
		break;
	case RHIAccess::WritableMask:
		break;
	default:
		break;
	}
}

enum RHITextureAccess
{
	TexSample = 1,
	TexColorOutput,
	TexDepthOutput,

	TexStorageLoad,
	TexStorageStore,

	TexStorageLoadStore
};



static inline VkImageLayout toVkLayout(EImageLayout layout)
{
	switch (layout) {
	case EImageLayout::Undefined:
		return VK_IMAGE_LAYOUT_UNDEFINED;
	case EImageLayout::General:
		return VK_IMAGE_LAYOUT_GENERAL; 
	case EImageLayout::ShaderReadOnlyOptimal:
		return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	case EImageLayout::ColorAttachmentOptimal:
		return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	case EImageLayout::DepthAttachmentOptimal:
		return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; 
	case EImageLayout::TransferSrcOptimal:
		return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	case EImageLayout::TransferDstOptimal:
		return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	case EImageLayout::PresentSrc:
		return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	default: 
		break;
	}
	return VK_IMAGE_LAYOUT_GENERAL;
}

static inline VkAccessFlags toVkAccess(RHITextureAccess access)
{
	switch (access) {
	case RHITextureAccess::TexSample:
		return VK_ACCESS_SHADER_READ_BIT;
	case RHITextureAccess::TexColorOutput:
		return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	case RHITextureAccess::TexDepthOutput:
		return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	case RHITextureAccess::TexStorageLoad:
		return VK_ACCESS_SHADER_READ_BIT;
	case RHITextureAccess::TexStorageStore:
		return VK_ACCESS_SHADER_WRITE_BIT;
	case RHITextureAccess::TexStorageLoadStore:
		return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
	default: 
		break;
	}
	return 0;
}
 

static inline VkPipelineStageFlags toVkPipelineStage(RHIPipelineStage stage)
{
	switch (stage) {
	case RHIPipelineStage::Top:
		return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	case RHIPipelineStage::VertexShader:
		return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
	case RHIPipelineStage::TCS:
		return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
	case RHIPipelineStage::TES:
		return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
	case RHIPipelineStage::FragmentShader:
		return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	case RHIPipelineStage::ColorAttachmentOutput:
		return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	case RHIPipelineStage::LateFragment:
		return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	case RHIPipelineStage::ComputeShader:
		return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	case RHIPipelineStage::GeometryShader:
		return VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
	case RHIPipelineStage::Bottom:
		return VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
	case RHIPipelineStage::ALL_COMMANDS_BIT:
		return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	default: 
		break;
	}
	return 0;
}
VkImageAspectFlags aspectMaskForTextureFormat(RHIPixelFormat format);
void VulkanCommandBuffer::SetImageBarrier(RHITexture* pTex, RHIPipelineStage srcStage, RHIPipelineStage dstStage, EImageLayout oldLayout, EImageLayout newLayout)
{
	VulkanTexture2D* pTex2D = dynamic_cast<VulkanTexture2D*>(pTex);
	VkImageAspectFlags aspect = aspectMaskForTextureFormat((RHIPixelFormat)pTex2D->GetFormat());
	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.image = pTex2D->GetImage();;
	imageMemoryBarrier.subresourceRange = { aspect, 0, 1, 0, 1 };	
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	imageMemoryBarrier.oldLayout = toVkLayout(oldLayout);// VK_IMAGE_LAYOUT_GENERAL;
	imageMemoryBarrier.newLayout = toVkLayout(newLayout);// VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	imageMemoryBarrier.srcAccessMask = GetVkAccessMaskForLayout(imageMemoryBarrier.oldLayout);// VK_ACCESS_SHADER_WRITE_BIT;
	imageMemoryBarrier.dstAccessMask = GetVkAccessMaskForLayout(imageMemoryBarrier.newLayout); //VK_ACCESS_SHADER_READ_BIT;

	VkPipelineStageFlags SrcStageMask = toVkPipelineStage(srcStage);// VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	VkPipelineStageFlags DstStageMask = toVkPipelineStage(dstStage);// VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	vkCmdPipelineBarrier(Handle, SrcStageMask, DstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void VulkanCommandBuffer::SetImageBarrier(VkImage& image, EImageLayout oldLayout, EImageLayout newLayout)
{ 
	VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange = { aspect, 0, 1, 0, 1 };
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	imageMemoryBarrier.oldLayout = toVkLayout(oldLayout);// VK_IMAGE_LAYOUT_GENERAL;
	imageMemoryBarrier.newLayout = toVkLayout(newLayout);// VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	imageMemoryBarrier.srcAccessMask = GetVkAccessMaskForLayout(imageMemoryBarrier.oldLayout);// VK_ACCESS_SHADER_WRITE_BIT;
	imageMemoryBarrier.dstAccessMask = GetVkAccessMaskForLayout(imageMemoryBarrier.newLayout); //VK_ACCESS_SHADER_READ_BIT;

	VkPipelineStageFlags SrcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	VkPipelineStageFlags DstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	vkCmdPipelineBarrier(Handle, SrcStageMask, DstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

VkImageAspectFlags VulkanCommandBuffer::ComputeAspectFlagsForFormat(VkFormat format)
{
	if (format == VK_FORMAT_D16_UNORM_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT) {
		return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else if (format == VK_FORMAT_D16_UNORM || format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_X8_D24_UNORM_PACK32) {
		return VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	else {
		return VK_IMAGE_ASPECT_COLOR_BIT;
	}
}


VkAccessFlags VulkanCommandBuffer::GetAccessFlags(VkImageLayout layout)
{
	switch (layout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		return 0;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		return VK_ACCESS_TRANSFER_READ_BIT;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		return VK_ACCESS_TRANSFER_WRITE_BIT;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		return 0;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	default:
		return 0;
	}
}

VkPipelineStageFlags VulkanCommandBuffer::GetPipelineStage(VkImageLayout layout)
{
	switch (layout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		return VK_PIPELINE_STAGE_TRANSFER_BIT;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		return VK_PIPELINE_STAGE_TRANSFER_BIT;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	default:
		return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	}
}

void VulkanCommandBuffer::SetImageMemoryBarrier(VkImage *image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	if (oldLayout == newLayout) {
		return;
	}

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = *image; 
	barrier.subresourceRange.aspectMask = ComputeAspectFlagsForFormat(format);
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;// image->m_mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;// image->m_arrayLayers;
	barrier.srcAccessMask = GetAccessFlags(oldLayout);
	barrier.dstAccessMask = GetAccessFlags(newLayout);

	VkPipelineStageFlags sourceStage = GetPipelineStage(oldLayout);
	VkPipelineStageFlags destinationStage = GetPipelineStage(newLayout); 
	vkCmdPipelineBarrier(Handle , sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier); 
}

//void VulkanCommandBuffer::BeginRenderPass()
//{
//	std::vector<VkClearValue> clearValues(1);
//	for (size_t i = 0; i < clearValues.size(); ++i) {
//		clearValues[i].color = { 0.0f, 0.0f, 0.0f, 1.0f };
//	}
//
//	VkRenderPassBeginInfo renderPassBeginInfo = {};
//	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//	renderPassBeginInfo.pNext = nullptr;
//	//renderPassBeginInfo.renderPass = renderPass->Handle;
//	//renderPassBeginInfo.renderArea.offset = { 0, 0 };
//	//renderPassBeginInfo.renderArea.extent = frameBuffer->getExtent();
//	//renderPassBeginInfo.framebuffer = frameBuffer->Handle;
//	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
//	renderPassBeginInfo.pClearValues = clearValues.data();
//
//	vkCmdBeginRenderPass(Handle, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
//}

void VulkanCommandBuffer::BeginRenderPass(RHIRenderTarget* rt) 
{
	//std::vector<VkClearValue> clearValues(2);
	//clearValues[0].color = { 0.0f, 1.0f, 0.0f, 1.0f };
	//clearValues[1].depthStencil = { 1.0f, 0 };
	
	//std::vector<VkClearValue> clearValues(4);
	//clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	//clearValues[1].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	//clearValues[2].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	//clearValues[3].depthStencil = { 1.0f, 0 }; 
	VulkanRenderTarget* vRt = dynamic_cast<VulkanRenderTarget*>(rt);
	VulkanRenderPass* pVkRenderPass = dynamic_cast<VulkanRenderPass*>(vRt->GetRenderPass());
	VulkanFrameBuffer* pVkFrameBuffer = dynamic_cast<VulkanFrameBuffer*>(vRt->GetFrameBuffer()); 
	int attCount = vRt->attachments.size(); 
	std::vector<VkClearValue> clearValues(attCount);
	for (int i = 0; i < attCount; i++) { 
		if(i == attCount - 1)
			clearValues[i].depthStencil = { 1.0f, 0 };
		else
			clearValues[i].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	}
	

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = pVkRenderPass->Handle;	
	renderPassBeginInfo.framebuffer = pVkFrameBuffer->Handle;
	renderPassBeginInfo.renderArea.offset = vRt->getOffset();
	renderPassBeginInfo.renderArea.extent = vRt->getExtent(); 
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();
	vkCmdBeginRenderPass(Handle, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::BeginRenderPass(RHIRenderPass* pRenderPass, RHIFrameBuffer* pFrameBuffer, const std::vector<RHIClearValue>& clearValues_)
{
	std::vector<VkClearValue> clearValues(2);
	//for (size_t i = 0; i < clearValues.size(); ++i) {
	//	clearValues[i].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	//}
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VulkanRenderPass* pVkRenderPass = dynamic_cast<VulkanRenderPass*>(pRenderPass);
	VulkanFrameBuffer* pVkFrameBuffer = dynamic_cast<VulkanFrameBuffer*>(pFrameBuffer);

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = pVkRenderPass->Handle;
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = pVkFrameBuffer->getExtent();
	renderPassBeginInfo.framebuffer = pVkFrameBuffer->Handle;
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(Handle, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::EndRenderPass()
{ 
	vkCmdEndRenderPass(Handle);
}

void VulkanCommandBuffer::SetGraphicsPipeline(RHIGraphicsPipeline* pipeline)
{
	GraphicsPipeline = dynamic_cast<VulkanGraphicsPipeline*>(pipeline);
	vkCmdBindPipeline(Handle, GraphicsPipeline->BindPoint, GraphicsPipeline->Handle); 
}

void VulkanCommandBuffer::SetComputePipeline(RHIComputePipeline* pipeline)
{
	ComputePipeline = dynamic_cast<VulkanComputePipeline*>(pipeline); 
	vkCmdBindPipeline(Handle, ComputePipeline->BindPoint, ComputePipeline->Handle);
}

void VulkanCommandBuffer::SetRaytracingPipeline(RHIRaytracingPipeline* pipeline)
{
	RaytracingPipeline = dynamic_cast<VulkanRaytracingPipeline*>(pipeline);
	vkCmdBindPipeline(Handle, RaytracingPipeline->BindPoint, RaytracingPipeline->Handle);
}

void VulkanCommandBuffer::SetLineWidth(float lineWidth)
{
	vkCmdSetLineWidth(Handle, lineWidth);
}

void VulkanCommandBuffer::SetViewport(float x, float y, float width, float height)
{
	VkViewport viewport = { x,y,width,height,0.0f,1.0f };
	vkCmdSetViewport(Handle, 0, 1, &viewport);
}

void VulkanCommandBuffer::SetScissor(int x, int y, int width, int height)
{
	VkRect2D Scissors = { x,y,width,height };
	vkCmdSetScissor(Handle, 0, 1, &Scissors);
}

void VulkanCommandBuffer::BindVertexBuffers(const std::vector<RHIBuffer*>& buffers)
{
	std::vector<VkDeviceSize> offsets(buffers.size(), 0);
	std::vector<VkBuffer> bufferHandles(buffers.size());
	for (size_t i = 0; i < bufferHandles.size(); ++i) {
		VulkanBuffer* pBuffer = dynamic_cast<VulkanBuffer*>(buffers[i]);
		bufferHandles[i] = pBuffer->Handle;
	}
	vkCmdBindVertexBuffers(Handle, 0, static_cast<uint32_t>(bufferHandles.size()), bufferHandles.data(), offsets.data());
}

void VulkanCommandBuffer::BindIndexBuffer(RHIBuffer* buffer, EIndexType eIndexType)
{
	VulkanBuffer* pBuffer = dynamic_cast<VulkanBuffer*>(buffer);
	VkIndexType iType = (eIndexType == EIndexType::Int32) ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16;
	VkDeviceSize Offset = { 0 };
	vkCmdBindIndexBuffer(Handle, pBuffer->Handle, Offset, iType);
}

void VulkanCommandBuffer::BindDescriptorSets()
{
	uint32_t FirstSet = 0;
	uint32_t DescriptorSetCount = 0;
	uint32_t DynamicOffsetCount = 0;
	uint32_t *DynamicOffsets = nullptr;
	VkDescriptorSet descSets = VK_NULL_HANDLE; 
	VulkanShaderBindings* pShaderBindings;
	if (GraphicsPipeline) {
		pShaderBindings = GraphicsPipeline->m_pVulkanShaderBindings;
		if (pShaderBindings) {
			DescriptorSetCount = 1;
			descSets = pShaderBindings->GetDescriptorSet();	
			vkCmdBindDescriptorSets(Handle, GraphicsPipeline->BindPoint, GraphicsPipeline->PipelineLayout,  FirstSet, 
			DescriptorSetCount, &descSets,
			DynamicOffsetCount, DynamicOffsets);
		}  
	} else if(ComputePipeline) {
		pShaderBindings = ComputePipeline->m_pVulkanShaderBindings;
		if (pShaderBindings) {
			DescriptorSetCount = 1;
			descSets = pShaderBindings->GetDescriptorSet();
			vkCmdBindDescriptorSets(Handle, ComputePipeline->BindPoint, ComputePipeline->PipelineLayout, FirstSet,
				DescriptorSetCount, &descSets,
				DynamicOffsetCount, DynamicOffsets);
		}
	}  
	else {
		pShaderBindings = RaytracingPipeline->m_pVulkanShaderBindings;
		if (pShaderBindings) {
			DescriptorSetCount = 1;
			descSets = pShaderBindings->GetDescriptorSet();
			vkCmdBindDescriptorSets(Handle, RaytracingPipeline->BindPoint, RaytracingPipeline->PipelineLayout, FirstSet,
				DescriptorSetCount, &descSets,
				DynamicOffsetCount, DynamicOffsets);
		}
	}
}

void VulkanCommandBuffer::DrawArray(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
	vkCmdDraw(Handle, vertexCount, instanceCount, firstVertex, firstInstance);
} 

void VulkanCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
{
	vkCmdDrawIndexed(Handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanCommandBuffer::Dispatch(std::uint32_t GroupCountX, std::uint32_t GroupCountY, std::uint32_t GroupCountZ)
{
	vkCmdDispatch(Handle, GroupCountX, GroupCountY, GroupCountZ);
}

void VulkanCommandBuffer::RaytraceDispatch(uint32_t width, uint32_t height, RHIShaderBindingTables* SBTs)
{
	VulkanShaderBindingTables* vkSBTs = dynamic_cast<VulkanShaderBindingTables*>(SBTs);
	VkStridedDeviceAddressRegionKHR raygenEntry = vkSBTs->raygenSbtEntry_;
	VkStridedDeviceAddressRegionKHR raymissEntry = vkSBTs->raymissSbtEntry_;
	VkStridedDeviceAddressRegionKHR rayhitEntry = vkSBTs->rayhitSbtEntry_;
	VkStridedDeviceAddressRegionKHR callableEntry = vkSBTs->callableSbtEntry_;
	VKRT::vkCmdTraceRaysKHR(Handle, &raygenEntry, &raymissEntry, &rayhitEntry, &callableEntry, width, height, 1);
}

void VulkanCommandBuffer::BuildAccelerationStructures()
{
#if 0
	VkAccelerationStructureBuildGeometryInfoKHR ASBuildGeometryInfo{};
	ASBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	ASBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	ASBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	ASBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	ASBuildGeometryInfo.dstAccelerationStructure = topLevelAS.handle;
	ASBuildGeometryInfo.geometryCount = 1;
	ASBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;
	ASBuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.deviceAddress;


	VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo{};
	accelerationStructureBuildRangeInfo.primitiveCount = 1;
	accelerationStructureBuildRangeInfo.primitiveOffset = 0;
	accelerationStructureBuildRangeInfo.firstVertex = 0;
	accelerationStructureBuildRangeInfo.transformOffset = 0;
	std::vector<VkAccelerationStructureBuildRangeInfoKHR*> accelerationBuildStructureRangeInfos = { &accelerationStructureBuildRangeInfo };

	VKRT::vkCmdBuildAccelerationStructuresKHR(Handle,1, &ASBuildGeometryInfo, accelerationBuildStructureRangeInfos.data());
#endif 
}