#include <VulkanRHI1/VulkanObjects//VulkanDevice.h>
#include <VulkanRHI1/VulkanObjects//VulkanQueue.h>
#include <VulkanRHI1/VulkanLoader.h>
#include <cstring>
#include <iostream>
VulkanDevice::VulkanDevice()
{

}

VulkanDevice::VulkanDevice(VkDevice InHandle)
	: Handle(InHandle)
{

}

VulkanDevice::~VulkanDevice()
{
	//std::cout << __FUNCTION__ << " handle " << Handle << std::endl;
	vkDestroyDevice(Handle, nullptr);
}

VkDevice VulkanDevice::GetHandle() const
{
	return Handle;
}

/*
	Device
*/
void  VulkanDevice::DestroyDevice(const VkAllocationCallbacks* Allocator)
{
	vkDestroyDevice(Handle, Allocator);
}

/*
	Device Queue
*/
void VulkanDevice::GetDeviceQueue(std::uint32_t QueueFamilyIndex,
	std::uint32_t QueueIndex,
	VkQueue* Queue)
{
	vkGetDeviceQueue(Handle, QueueFamilyIndex, QueueIndex, Queue);
}

void VulkanDevice::GetDeviceQueue2(const VkDeviceQueueInfo2* QueueInfo, VkQueue* Queue)
{
	vkGetDeviceQueue2(Handle, QueueInfo, Queue);
}

/*
	Command Pool
*/
VkResult VulkanDevice::CreateCommandPool(const VkCommandPoolCreateInfo* CreateInfo,
	const VkAllocationCallbacks* Allocator,
	VkCommandPool* CommandPool)
{
	return vkCreateCommandPool(Handle, CreateInfo, nullptr, CommandPool);;
}

void VulkanDevice::DestroyCommandPool(VkCommandPool CommandPool, const VkAllocationCallbacks* Allocator)
{
	vkDestroyCommandPool(Handle, CommandPool, Allocator);
}

void VulkanDevice::TrimCommandPool(VkCommandPool CommandPool, VkCommandPoolTrimFlags Flags)
{
	vkTrimCommandPool(Handle, CommandPool, Flags);
}

VkResult VulkanDevice::ResetCommandPool(VkCommandPool CommandPool, VkCommandPoolResetFlags Flags)
{
	return vkResetCommandPool(Handle, CommandPool, Flags);
}

/*
	Command Buffer
*/
VkResult VulkanDevice::AllocateCommandBuffers(const VkCommandBufferAllocateInfo* AllocateInfo, VkCommandBuffer* CommandBuffers)
{
	return vkAllocateCommandBuffers(Handle, AllocateInfo, CommandBuffers);
}

void VulkanDevice::FreeCommandBuffers(VkCommandPool CommandPool, std::uint32_t CommandBufferCount, const VkCommandBuffer* CommandBuffers)
{
	return vkFreeCommandBuffers(Handle, CommandPool, CommandBufferCount, CommandBuffers);
}


/*
	Fence
*/
VkResult VulkanDevice::CreateFence(const VkFenceCreateInfo* CreateInfo,
	const VkAllocationCallbacks* Allocator,
	VkFence* Fence)
{
	return vkCreateFence(Handle, CreateInfo, Allocator, Fence);
}

void VulkanDevice::DestroyFence(VkFence Fence, const VkAllocationCallbacks* Allocator)
{
	vkDestroyFence(Handle, Fence, Allocator);
}


VkResult VulkanDevice::WaitForFences(uint32_t FenceCount, const VkFence* Fences, VkBool32 WaitAll, uint64_t Timeout)
{
	return vkWaitForFences(Handle, FenceCount, Fences, WaitAll, Timeout);
}

VkResult VulkanDevice::ResetFences(uint32_t FenceCount, const VkFence* Fences)
{
	return vkResetFences(Handle, FenceCount, Fences);
}

VkResult VulkanDevice::CreateSampler(const VkSamplerCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkSampler* Sampler)
{
	return vkCreateSampler(Handle, CreateInfo, nullptr, Sampler);
}

void VulkanDevice::DestroySampler(VkSampler Sampler, const VkAllocationCallbacks* Allocator)
{
	vkDestroySampler(Handle, Sampler, Allocator);
}

/*
	Semaphore
*/
VkResult VulkanDevice::CreateSemaphore(const VkSemaphoreCreateInfo* CreateInfo,
	const VkAllocationCallbacks* Allocator,
	VkSemaphore* Semaphore)
{
	return vkCreateSemaphore(Handle, CreateInfo, Allocator, Semaphore);
}

void VulkanDevice::DestroySemaphore(VkSemaphore Semaphore, const VkAllocationCallbacks* Allocator)
{
	vkDestroySemaphore(Handle, Semaphore, Allocator);
}

/*
	Event
*/
VkResult VulkanDevice::CreateEvent(const VkEventCreateInfo* CreateInfo,
	const VkAllocationCallbacks* Allocator,
	VkEvent* Event)
{
	return vkCreateEvent(Handle, CreateInfo, Allocator, Event);
}

void VulkanDevice::DestroyEvent(VkEvent Event, const VkAllocationCallbacks* Allocator)
{
	vkDestroyEvent(Handle, Event, Allocator);
}

/*
	Render Pass
*/
VkResult VulkanDevice::CreateRenderPass(const VkRenderPassCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkRenderPass* RenderPass)
{
	return vkCreateRenderPass(Handle, CreateInfo, Allocator, RenderPass);
}

void VulkanDevice::DestroyRenderPass(VkRenderPass RenderPass, const VkAllocationCallbacks* Allocator)
{
	vkDestroyRenderPass(Handle, RenderPass, Allocator);
}

/*
	Physical Storage Buffer Access
*/
VkDeviceAddress VulkanDevice::GetBufferDeviceAddress(const VkBufferDeviceAddressInfo* Info)
{
	return vkGetBufferDeviceAddress(Handle, Info);
}

std::uint64_t VulkanDevice::GetBufferOpaqueCaptureAddress(const VkBufferDeviceAddressInfo* Info)
{
	return vkGetBufferOpaqueCaptureAddress(Handle, Info);
}

/*
	Framebuffer
*/
VkResult VulkanDevice::CreateFramebuffer(const VkFramebufferCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkFramebuffer* Framebuffer)
{
	return vkCreateFramebuffer(Handle, CreateInfo, nullptr, Framebuffer);
}

void VulkanDevice::DestroyFramebuffer(VkFramebuffer Framebuffer, const VkAllocationCallbacks* Allocator)
{
	vkDestroyFramebuffer(Handle, Framebuffer, Allocator);
}

/*
	Shader
*/
VkResult VulkanDevice::CreateShaderModule(const VkShaderModuleCreateInfo* CreateInfo,
	const VkAllocationCallbacks* Allocator,
	VkShaderModule* ShaderModule)
{
	return vkCreateShaderModule(Handle, CreateInfo, nullptr, ShaderModule);
}

void VulkanDevice::DestroyShaderModule(VkShaderModule ShaderModule,
	const VkAllocationCallbacks* Allocator)
{
	vkDestroyShaderModule(Handle, ShaderModule, Allocator);
}

/*
	Pipeline
*/
VkResult VulkanDevice::CreateGraphicsPipelines(VkPipelineCache PipelineCache,
	std::uint32_t CreateInfoCount,
	const VkGraphicsPipelineCreateInfo* CreateInfos,
	const VkAllocationCallbacks* Allocator,
	VkPipeline* Pipelines)
{
	return vkCreateGraphicsPipelines(Handle, PipelineCache, CreateInfoCount, CreateInfos, nullptr, Pipelines);
}

VkResult VulkanDevice::CreateComputePipelines(VkPipelineCache PipelineCache,
	std::uint32_t CreateInfoCount,
	const VkComputePipelineCreateInfo* CreateInfos,
	const VkAllocationCallbacks* Allocator,
	VkPipeline* Pipelines)
{
	return vkCreateComputePipelines(Handle, PipelineCache, 1, CreateInfos, nullptr, Pipelines);
}

void VulkanDevice::DestroyPipeline(VkPipeline Pipeline, const VkAllocationCallbacks* Allocator)
{
	vkDestroyPipeline(Handle, Pipeline, Allocator);
}

/*
	Pipeline Cache
*/
VkResult VulkanDevice::CreatePipelineCache(const VkPipelineCacheCreateInfo* CreateInfo,
	const VkAllocationCallbacks* Allocator,
	VkPipelineCache* PipelineCache)
{
	return vkCreatePipelineCache(Handle, CreateInfo, nullptr, PipelineCache);
}

void VulkanDevice::DestroyPipelineCache(VkPipelineCache PipelineCache,
	const VkAllocationCallbacks* Allocator)
{
	vkDestroyPipelineCache(Handle, PipelineCache, Allocator);
}

/*
	Pipeline Layout
*/
VkResult VulkanDevice::CreatePipelineLayout(const VkPipelineLayoutCreateInfo* CreateInfo,
	const VkAllocationCallbacks* Allocator,
	VkPipelineLayout* PipelineLayout)
{
	return vkCreatePipelineLayout(Handle, CreateInfo, nullptr, PipelineLayout);
}

void VulkanDevice::DestroyPipelineLayout(VkPipelineLayout PipelineLayout, const VkAllocationCallbacks* Allocator)
{
	vkDestroyPipelineLayout(Handle, PipelineLayout, Allocator);
}

/* 
	Memory 
*/
VkResult VulkanDevice::AllocateMemory(const VkMemoryAllocateInfo* AllocateInfo,
	const VkAllocationCallbacks* Allocator,
	VkDeviceMemory* Memory)
{
	return vkAllocateMemory(Handle, AllocateInfo, nullptr, Memory);
}

void VulkanDevice::FreeMemory(VkDeviceMemory Memory,
	const VkAllocationCallbacks* Allocator)
{
	vkFreeMemory(Handle, Memory, Allocator);
}

VkResult VulkanDevice::MapMemory(VkDeviceMemory Memory,
	VkDeviceSize Offset,
	VkDeviceSize Size,
	VkMemoryMapFlags Flags,
	void** Data)
{
	return vkMapMemory(Handle, Memory, Offset, Size, Flags, Data);
}

void VulkanDevice::UnmapMemory(VkDeviceMemory Memory)
{
	vkUnmapMemory(Handle, Memory);
}

/*
	Buffer
*/
VkResult VulkanDevice::CreateBuffer(const VkBufferCreateInfo* CreateInfo,
	const VkAllocationCallbacks* Allocator,
	VkBuffer* Buffer)
{
	return vkCreateBuffer(Handle, CreateInfo, Allocator, Buffer);
}

void VulkanDevice::CopyBuffer(VkDeviceMemory Memory, VkDeviceSize MemorySize, void* BufferData)
{
	void* data;
	vkMapMemory(Handle, Memory, 0, MemorySize, 0, &data);
	memcpy(data, BufferData, static_cast<size_t>(MemorySize));
	vkUnmapMemory(Handle, Memory);
}

void VulkanDevice::DestroyBuffer(VkBuffer Buffer,
	const VkAllocationCallbacks* Allocator)
{
	vkDestroyBuffer(Handle, Buffer, Allocator);
}

VkResult VulkanDevice::BindBufferMemory(VkBuffer Buffer,
	VkDeviceMemory Memory,
	VkDeviceSize MemoryOffset)
{
	return vkBindBufferMemory(Handle, Buffer, Memory, MemoryOffset);
}

void VulkanDevice::GetBufferMemoryRequirements(VkBuffer Buffer,
	VkMemoryRequirements* MemoryRequirements)
{
	vkGetBufferMemoryRequirements(Handle, Buffer, MemoryRequirements);
}

void VulkanDevice::GetImageMemoryRequirements(VkImage Image,
	VkMemoryRequirements* MemoryRequirements)
{
	vkGetImageMemoryRequirements(Handle, Image, MemoryRequirements);
}
/*
	Buffer View
*/
VkBufferView VulkanDevice::CreateBufferView(VkBuffer Buffer, VkFormat Format, VkDeviceSize Offset, VkDeviceSize Range)
{
	VkBufferView BufferView;
	VkBufferViewCreateInfo CreateInfo;
	memset(&CreateInfo, 0, sizeof(VkBufferViewCreateInfo));
	CreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
	CreateInfo.format = Format;
	CreateInfo.offset = Offset;
	CreateInfo.range = Range;
	vkCreateBufferView(Handle, &CreateInfo, nullptr, &BufferView);
	return BufferView;
}

void VulkanDevice::DestroyBufferView(
	VkBufferView BufferView,
	const VkAllocationCallbacks* Allocator)
{
	vkDestroyBufferView(Handle, BufferView, Allocator);
}

/*
	Image
*/ 
void VulkanDevice::MapFormatSupport(RHIPixelFormat UserFormat, VkFormat InFormat, const VkComponentMapping& ComponentMapping)
{
	GPixelFormats[UserFormat].PlatformFormat = InFormat;
	//ComponentMapping todo...
}

void VulkanDevice::SetupFormats()
{
	//for (uint32_t i = 0; i < VK_FORMAT_RANGE_SIZE; ++i)
	//{
	//	const VkFormat Format = (VkFormat)i;
	//	FMemory::Memzero(FormatProperties[i]);
	//	VulkanRHI::vkGetPhysicalDeviceFormatProperties(Gpu, Format, &FormatProperties[i]);
	//}

	//static_assert(sizeof(VkFormat) <= sizeof(GPixelFormats[0].PlatformFormat), "PlatformFormat must be increased!");

	// Create shortcuts for the possible component mappings
	const VkComponentMapping ComponentMappingRGBA = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	const VkComponentMapping ComponentMappingBGRA = { VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_A };
	const VkComponentMapping ComponentMappingRGB1 = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_ONE };
	const VkComponentMapping ComponentMappingRG01 = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_ZERO, VK_COMPONENT_SWIZZLE_ONE };
	const VkComponentMapping ComponentMappingRRRG = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G };
	const VkComponentMapping ComponentMappingR001 = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_ZERO, VK_COMPONENT_SWIZZLE_ZERO, VK_COMPONENT_SWIZZLE_ONE };
	const VkComponentMapping ComponentMappingRIII = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	const VkComponentMapping ComponentMapping000R = { VK_COMPONENT_SWIZZLE_ZERO, VK_COMPONENT_SWIZZLE_ZERO, VK_COMPONENT_SWIZZLE_ZERO, VK_COMPONENT_SWIZZLE_R };
	const VkComponentMapping ComponentMappingR000 = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_ZERO, VK_COMPONENT_SWIZZLE_ZERO, VK_COMPONENT_SWIZZLE_ZERO };
	const VkComponentMapping ComponentMappingRR01 = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_ZERO, VK_COMPONENT_SWIZZLE_ONE };


	// Initialize the platform pixel format map.
	for (int32_t i = 0; i < PF_MAX; ++i)
	{
		GPixelFormats[i].PlatformFormat = VK_FORMAT_UNDEFINED;
		//GPixelFormats[i].Supported = false;
		//GVulkanBufferFormat[Index] = VK_FORMAT_UNDEFINED;

		//// Set default component mapping
		//PixelFormatComponentMapping[Index] = ComponentMappingRGBA;
	}

	//const EPixelFormatCapabilities ColorRenderTargetRequiredCapabilities = (EPixelFormatCapabilities::TextureSample | EPixelFormatCapabilities::RenderTarget);

	// Default formats
	//PixelFormatInfo.PlatformFormat = VulkanFormat;

	MapFormatSupport(RHIPixelFormat::PF_R8_SBYTE, VK_FORMAT_R8_SNORM, ComponentMappingR001);
	MapFormatSupport(RHIPixelFormat::PF_R8_UBYTE, VK_FORMAT_R8_UNORM, ComponentMappingR001);
	MapFormatSupport(RHIPixelFormat::PF_R16_SINT, VK_FORMAT_R16_SINT, ComponentMappingR001);
	MapFormatSupport(RHIPixelFormat::PF_R16_UINT, VK_FORMAT_R16_UINT, ComponentMappingR001);
	MapFormatSupport(RHIPixelFormat::PF_R32_SINT, VK_FORMAT_R32_SINT, ComponentMappingR001);
	MapFormatSupport(RHIPixelFormat::PF_R32_UINT, VK_FORMAT_R32_UINT, ComponentMappingR001);
	MapFormatSupport(RHIPixelFormat::PF_R16_FLOAT, VK_FORMAT_R16_SFLOAT, ComponentMappingR001);
	MapFormatSupport(RHIPixelFormat::PF_R32_FLOAT, VK_FORMAT_R32_SFLOAT, ComponentMappingR001);

	MapFormatSupport(RHIPixelFormat::PF_R8G8_SBYTE, VK_FORMAT_R8G8_SNORM, ComponentMappingRG01);
	MapFormatSupport(RHIPixelFormat::PF_R8G8_UBYTE, VK_FORMAT_R8G8_UNORM, ComponentMappingRG01);

	//MapFormatSupport(RHIPixelFormat::PF_R8G8_UBYTE, VK_FORMAT_R8G8_UNORM, ComponentMappingRRRG); //映射成RRRG 对应GL_LUMINANCE

	MapFormatSupport(RHIPixelFormat::PF_R16G16_SINT, VK_FORMAT_R16G16_SINT, ComponentMappingRG01);
	MapFormatSupport(RHIPixelFormat::PF_R16G16_UINT, VK_FORMAT_R16G16_UINT, ComponentMappingRG01);
	MapFormatSupport(RHIPixelFormat::PF_R32G32_SINT, VK_FORMAT_R32G32_SINT, ComponentMappingRG01);
	MapFormatSupport(RHIPixelFormat::PF_R32G32_UINT, VK_FORMAT_R32G32_UINT, ComponentMappingRG01);
	MapFormatSupport(RHIPixelFormat::PF_R16G16_FLOAT, VK_FORMAT_R16G16_SFLOAT, ComponentMappingRG01);
	MapFormatSupport(RHIPixelFormat::PF_R32G32_FLOAT, VK_FORMAT_R32G32_SFLOAT, ComponentMappingRG01); // Requirement for GPU particles

	MapFormatSupport(RHIPixelFormat::PF_R8G8B8_SBYTE, VK_FORMAT_R8G8B8_SNORM, ComponentMappingRGB1);
	MapFormatSupport(RHIPixelFormat::PF_R8G8B8_UBYTE, VK_FORMAT_R8G8B8_UNORM, ComponentMappingRGB1);
	MapFormatSupport(RHIPixelFormat::PF_R16G16B16_SINT, VK_FORMAT_R16G16B16_SINT, ComponentMappingRGB1);
	MapFormatSupport(RHIPixelFormat::PF_R16G16B16_UINT, VK_FORMAT_R16G16B16_UINT, ComponentMappingRGB1);
	MapFormatSupport(RHIPixelFormat::PF_R32G32B32_SINT, VK_FORMAT_R32G32B32_SINT, ComponentMappingRGB1);
	MapFormatSupport(RHIPixelFormat::PF_R32G32B32_UINT, VK_FORMAT_R32G32B32_UINT, ComponentMappingRGB1);
	MapFormatSupport(RHIPixelFormat::PF_R16G16B16_FLOAT, VK_FORMAT_R16G16B16_SFLOAT, ComponentMappingRGB1);
	MapFormatSupport(RHIPixelFormat::PF_R32G32B32_FLOAT, VK_FORMAT_R32G32B32_SFLOAT, ComponentMappingRGB1); 

	/*
		VK_FORMAT_B8G8R8A8_SRGB:报错
			Validation Error: [ VUID-VkImageCreateInfo-imageCreateMaxMipLevels-02251 ] | MessageID = 0xbebcae79
			vkCreateImage(): pCreateInfo The following VkImageCreateInfo
			format (VK_FORMAT_B8G8R8A8_SRGB)
			type (VK_IMAGE_TYPE_2D)
			tiling (VK_IMAGE_TILING_LINEAR)
			usage (VK_IMAGE_USAGE_TRANSFER_SRC_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_STORAGE_BIT)
			flags (VkImageCreateFlags(0))
			returned (VK_ERROR_FORMAT_NOT_SUPPORTED) when calling vkGetPhysicalDeviceImageFormatProperties2.
			The Vulkan spec states: Each of the following values (as described in Image Creation Limits) must not be undefined : imageCreateMaxMipLevels, 
			imageCreateMaxArrayLayers, imageCreateMaxExtent, and imageCreateSampleCounts (https://vulkan.lunarg.com/doc/view/1.4.309.0/windows/antora/spec/latest/chapters/resources.html#VUID-VkImageCreateInfo-imageCreateMaxMipLevels-02251)
			
			Validation Error: [ VUID-VkImageViewCreateInfo-usage-02275 ] | MessageID = 0x618ab1e7
			vkCreateImageView(): pCreateInfo->format VK_FORMAT_B8G8R8A8_SRGB with tiling VK_IMAGE_TILING_LINEAR doesn't support VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT.
			(supported features: VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT|VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BLEND_BIT|VK_FORMAT_FEATURE_2_BLIT_SRC_BIT|VK_FORMAT_FEA
			TURE_2_BLIT_DST_BIT|VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_FILTER_LINEAR_BIT|VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT|VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT|VK_FOR
			MAT_FEATURE_2_SAMPLED_IMAGE_FILTER_MINMAX_BIT|VK_FORMAT_FEATURE_2_LINEAR_COLOR_ATTACHMENT_BIT_NV|VK_FORMAT_FEATURE_2_HOST_IMAGE_TRANSFER_BIT).
			The Vulkan spec states: If usage contains VK_IMAGE_USAGE_STORAGE_BIT, then the image view's format features must contain 
			VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT (https://vulkan.lunarg.com/doc/view/1.4.309.0/windows/antora/spec/latest/chapters/resources.html#VUID-VkImageViewCreateInfo-usage-02275)
	*/
	MapFormatSupport(RHIPixelFormat::PF_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, ComponentMappingBGRA); //MapFormatSupport(RHIPixelFormat::PF_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_SRGB, ComponentMappingBGRA);

	MapFormatSupport(RHIPixelFormat::PF_R8G8B8A8_SRGB, VK_FORMAT_R8G8B8A8_SRGB, ComponentMappingRGBA);

	MapFormatSupport(RHIPixelFormat::PF_R8G8B8A8_SBYTE, VK_FORMAT_R8G8B8A8_SNORM, ComponentMappingRGBA); 
	MapFormatSupport(RHIPixelFormat::PF_R8G8B8A8_UBYTE, VK_FORMAT_R8G8B8A8_UNORM, ComponentMappingRGBA);

	MapFormatSupport(RHIPixelFormat::PF_R16G16B16A16_SINT, VK_FORMAT_R16G16B16A16_SINT, ComponentMappingRGBA);
	MapFormatSupport(RHIPixelFormat::PF_R16G16B16A16_UINT, VK_FORMAT_R16G16B16A16_UINT, ComponentMappingRGBA);
	MapFormatSupport(RHIPixelFormat::PF_R32G32B32A32_SINT, VK_FORMAT_R32G32B32A32_SINT, ComponentMappingRGBA);
	MapFormatSupport(RHIPixelFormat::PF_R32G32B32A32_UINT, VK_FORMAT_R32G32B32A32_UINT, ComponentMappingRGBA);
	MapFormatSupport(RHIPixelFormat::PF_R16G16B16A16_FLOAT, VK_FORMAT_R16G16B16A16_SFLOAT, ComponentMappingRGBA);
	MapFormatSupport(RHIPixelFormat::PF_R32G32B32A32_FLOAT, VK_FORMAT_R32G32B32A32_SFLOAT, ComponentMappingRGBA);

	MapFormatSupport(RHIPixelFormat::PF_DepthStencil, VK_FORMAT_D32_SFLOAT_S8_UINT, ComponentMappingRIII);

	/*
		B G R A
	*/
	MapFormatSupport(RHIPixelFormat::PF_B8G8R8A8_SBYTE, VK_FORMAT_B8G8R8A8_SNORM, ComponentMappingBGRA);
	MapFormatSupport(RHIPixelFormat::PF_B8G8R8A8_UBYTE, VK_FORMAT_B8G8R8A8_UNORM, ComponentMappingBGRA);
}

VkResult VulkanDevice::CreateImage(const VkImageCreateInfo* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkImage* pImage)
{ 
	return vkCreateImage(Handle, pCreateInfo, pAllocator, pImage); 
}

void VulkanDevice::BindImageMemory(VkImage Image,
	VkDeviceMemory Memory,
	VkDeviceSize MemoryOffset)
{
	vkBindImageMemory(Handle, Image, Memory, MemoryOffset);
}

void VulkanDevice::DestroyImage(VkImage Image, const VkAllocationCallbacks* Allocator)
{
	vkDestroyImage(Handle, Image, Allocator);
}

void VulkanDevice::GetImageSubresourceLayout(VkImage Image, const VkImageSubresource* Subresource, VkSubresourceLayout* Layout)
{
	vkGetImageSubresourceLayout(Handle, Image, Subresource, Layout);
}

/*
	Image View
*/

VkResult VulkanDevice::CreateImageView(const VkImageViewCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkImageView* View)
{
	return vkCreateImageView(Handle, CreateInfo, nullptr, View);
}

void VulkanDevice::DestroyImageView(VkImageView ImageView, const VkAllocationCallbacks* Allocator)
{
	vkDestroyImageView(Handle, ImageView, Allocator);
}

/*
	Descriptor Set Layout
*/
VkResult VulkanDevice::AllocateDescriptorSets(const VkDescriptorSetAllocateInfo* AllocateInfo, VkDescriptorSet* DescriptorSets)
{
	return vkAllocateDescriptorSets(Handle, AllocateInfo, DescriptorSets);
}

void VulkanDevice::UpdateDescriptorSets(uint32_t DescriptorWriteCount, const VkWriteDescriptorSet* DescriptorWrites, uint32_t DescriptorCopyCount, const VkCopyDescriptorSet* DescriptorCopies)
{
	vkUpdateDescriptorSets(Handle, DescriptorWriteCount, DescriptorWrites, DescriptorCopyCount, DescriptorCopies);
}

VkResult  VulkanDevice::CreateDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkDescriptorSetLayout* SetLayout)
{
	return vkCreateDescriptorSetLayout(Handle, CreateInfo, Allocator, SetLayout);
}

void VulkanDevice::DestroyDescriptorSetLayout(VkDescriptorSetLayout DescriptorSetLayout,
	const VkAllocationCallbacks* Allocator)
{
	vkDestroyDescriptorSetLayout(Handle, DescriptorSetLayout, Allocator);
}

/*
	Descriptor Pool
*/
VkResult VulkanDevice::CreateDescriptorPool(const VkDescriptorPoolCreateInfo* CreateInfo,
	const VkAllocationCallbacks* Allocator,
	VkDescriptorPool* DescriptorPool) 
{
	return vkCreateDescriptorPool(Handle, CreateInfo, nullptr, DescriptorPool);
}

void VulkanDevice::DestroyDescriptorPool(VkDescriptorPool DescriptorPool,
	const VkAllocationCallbacks* Allocator)
{
	vkDestroyDescriptorPool(Handle, DescriptorPool, Allocator);
}

VkQueryPool VulkanDevice::CreateQueryPool()
{
	VkQueryPool QueryPool;
	VkQueryPoolCreateInfo CreateInfo;

	vkCreateQueryPool(Handle, &CreateInfo, nullptr, &QueryPool);
	return QueryPool;
}

VkBool32 VulkanDevice::GetSupportedDepthFormat(VkFormat* depthFormat)
{
	// Since all depth formats may be optional, we need to find a suitable depth format to use 
	std::vector<VkFormat> formatList = {// Start with the highest precision packed format
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};
	for (auto& format : formatList) {
		VkFormatProperties formatProps;
		PhysicalDevice->GetPhysicalDeviceFormatProperties(format, &formatProps);
		if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			*depthFormat = format;
			return true;
		}
	}
	return false;
}

VkBool32 VulkanDevice::GetSupportedBlitting(VkFormat* format)
{
	VkFormatProperties formatProps;
	PhysicalDevice->GetPhysicalDeviceFormatProperties(*format, &formatProps);
	if (!(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT)) {// Check if the device supports blitting from optimal images (the swapchain images are in optimal format)
		printf("Device does not support blitting from optimal tiled images, using copy instead of blit!\n");
		return false;
	}
	PhysicalDevice->GetPhysicalDeviceFormatProperties(VK_FORMAT_R8G8B8A8_UNORM, &formatProps);
	if (!(formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT)) {// Check if the device supports blitting to linear images
		printf("Device does not support blitting to linear tiled images, using copy instead of blit!\n");
		return false;
	}
	return true;
}

/*
	Ray Tracing Pipeline
*/
VkResult VulkanDevice::CreateRayTracingPipelinesKHR(VkDeferredOperationKHR DeferredOperation,
	VkPipelineCache                             PipelineCache,
	uint32_t                                    CreateInfoCount,
	const VkRayTracingPipelineCreateInfoKHR* CreateInfos,
	const VkAllocationCallbacks* Allocator,
	VkPipeline* Pipelines)
{
	return VKRT::vkCreateRayTracingPipelinesKHR(Handle, DeferredOperation, PipelineCache, CreateInfoCount, CreateInfos, Allocator, Pipelines);
}

/*
	Acceleration Structure
*/
VkResult VulkanDevice::CreateAccelerationStructure(VkAccelerationStructureCreateInfoKHR& accelerationStructureCreateInfo, VkAccelerationStructureKHR* pAS)
{
	return VKRT::vkCreateAccelerationStructureKHR(Handle, &accelerationStructureCreateInfo, nullptr, pAS);
}

void VulkanDevice::DestroyAccelerationStructureKHR(VkAccelerationStructureKHR  AccelerationStructure, const VkAllocationCallbacks* Allocator)
{
	VKRT::vkDestroyAccelerationStructureKHR(Handle, AccelerationStructure, Allocator);
}


VkDeviceAddress VulkanDevice::GetAccelerationStructureDeviceAddress(const VkAccelerationStructureDeviceAddressInfoKHR* pInfo)
{
	return VKRT::vkGetAccelerationStructureDeviceAddressKHR(Handle, pInfo);
}

void VulkanDevice::GetAccelerationStructureBuildSizes(const VkAccelerationStructureBuildGeometryInfoKHR* pBuildInfo, const uint32_t* pMaxPrimitiveCounts, VkAccelerationStructureBuildSizesInfoKHR* pSizeInfo) 
{
	VKRT::vkGetAccelerationStructureBuildSizesKHR(Handle,VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, pBuildInfo, pMaxPrimitiveCounts, pSizeInfo);
}

void VulkanDevice::GetRayTracingShaderGroupHandles(VkPipeline pipeline, uint32_t groupCount, size_t dataSize, void* pData) {
	VKRT::vkGetRayTracingShaderGroupHandlesKHR(Handle, pipeline, 0, groupCount, dataSize, pData);
}

/*
	Buffer Device Address
*/
VkDeviceAddress VulkanDevice::GetBufferDeviceAddressKHR(const VkBufferDeviceAddressInfo* Info)
{
	return VKRT::vkGetBufferDeviceAddressKHR(Handle, Info);
}

/*
	Wait
*/
VkResult VulkanDevice::DeviceWaitIdle()
{
	return vkDeviceWaitIdle(Handle);
}


/*
	Swap Chain
*/
VkResult  VulkanDevice::CreateSwapchainKHR(const VkSwapchainCreateInfoKHR* CreateInfo, const VkAllocationCallbacks* Allocator, VkSwapchainKHR* Swapchain)
{
	return vkCreateSwapchainKHR(Handle, CreateInfo, Allocator, Swapchain);
}

void  VulkanDevice::DestroySwapchainKHR(VkSwapchainKHR Swapchain, const VkAllocationCallbacks* Allocator)
{
	vkDestroySwapchainKHR(Handle, Swapchain, Allocator);
}

VkResult  VulkanDevice::GetSwapchainImagesKHR(VkSwapchainKHR Swapchain, uint32_t* SwapchainImageCount, VkImage* SwapchainImages)
{
	return vkGetSwapchainImagesKHR(Handle, Swapchain, SwapchainImageCount, SwapchainImages);
}

VkResult  VulkanDevice::AcquireNextImageKHR(VkSwapchainKHR Swapchain, uint64_t Timeout, VkSemaphore Semaphore, VkFence Fence, uint32_t* ImageIndex)
{
	return vkAcquireNextImageKHR(Handle, Swapchain, Timeout, Semaphore, Fence, ImageIndex);
}

VkResult  VulkanDevice::AcquireNextImage2KHR(const VkAcquireNextImageInfoKHR* AcquireInfo, uint32_t* ImageIndex)
{
	return vkAcquireNextImage2KHR(Handle, AcquireInfo, ImageIndex);
}

/*
	Get Device Group Present Capabilitie
*/
VkResult  VulkanDevice::GetDeviceGroupPresentCapabilitiesKHR(VkDeviceGroupPresentCapabilitiesKHR* DeviceGroupPresentCapabilities)
{
	return vkGetDeviceGroupPresentCapabilitiesKHR(Handle, DeviceGroupPresentCapabilities);
}

VkResult  VulkanDevice::GetDeviceGroupSurfacePresentModesKHR(VkSurfaceKHR Surface, VkDeviceGroupPresentModeFlagsKHR* Modes)
{
	return vkGetDeviceGroupSurfacePresentModesKHR(Handle, Surface, Modes);
}