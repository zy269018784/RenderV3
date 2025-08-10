#pragma once
#include <VulkanRHI/Vulkan.h>
#include <RHIDefinitions.h>
#include <RHIPixelFormat.h>
#include <VulkanRHI/VulkanObjects/VulkanPhysicalDevice.h>
#include <VulkanRHI/VulkanObjects/VulkanInstance.h>
class VulkanDevice
{
public:
	VulkanDevice();
	VulkanDevice(VkDevice InHandle);
	~VulkanDevice();
	VkDevice GetHandle() const;
public:
	/*
		Device
	*/
	void DestroyDevice(const VkAllocationCallbacks* Allocator);

	/*
		Device Queue
	*/
	void GetDeviceQueue(std::uint32_t QueueFamilyIndex, std::uint32_t QueueIndex, VkQueue* Queue);
	void GetDeviceQueue2(const VkDeviceQueueInfo2* QueueInfo, VkQueue* Queue);

	/*
		Command Pool
	*/
	VkResult CreateCommandPool(const VkCommandPoolCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkCommandPool* CommandPool);
	void DestroyCommandPool(VkCommandPool CommandPool, const VkAllocationCallbacks* Allocator);
	void TrimCommandPool(VkCommandPool CommandPool, VkCommandPoolTrimFlags Flags);
	VkResult ResetCommandPool(VkCommandPool commandPool, VkCommandPoolResetFlags Flags);

	/*
		Command Buffer
	*/
	VkResult AllocateCommandBuffers(const VkCommandBufferAllocateInfo* AllocateInfo, VkCommandBuffer* CommandBuffers);

	void FreeCommandBuffers(VkCommandPool CommandPool, std::uint32_t CommandBufferCount, const VkCommandBuffer* CommandBuffers);

	/*
		Fence
	*/
	VkResult CreateFence(const VkFenceCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkFence* Fence);
	void DestroyFence(VkFence Fence, const VkAllocationCallbacks* Allocator);
	VkResult WaitForFences(uint32_t FenceCount, const VkFence* Fences, VkBool32 WaitAll, uint64_t Timeout);
	VkResult ResetFences(uint32_t FenceCount, const VkFence* Fences);

	/*
		Sampler
	*/
	VkResult CreateSampler(const VkSamplerCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkSampler* Sampler);
	void DestroySampler(VkSampler Sampler, const VkAllocationCallbacks* Allocator);

	/*
		Semaphore
	*/
	VkResult CreateSemaphore(const VkSemaphoreCreateInfo* CreateInfo,
		const VkAllocationCallbacks* Allocator,
		VkSemaphore* Semaphore);

	void DestroySemaphore(VkSemaphore Semaphore, const VkAllocationCallbacks* Allocator);

	/*
		Event
	*/
	VkResult CreateEvent(const VkEventCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkEvent* Event);
	void DestroyEvent(VkEvent Event, const VkAllocationCallbacks* Allocator);

	/*
		Render Pass
	*/
	VkResult CreateRenderPass(const VkRenderPassCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkRenderPass* RenderPass);
	void DestroyRenderPass(VkRenderPass RenderPass, const VkAllocationCallbacks* Allocator);

	/*
		Physical Storage Buffer Access
	*/
	VkDeviceAddress GetBufferDeviceAddress(const VkBufferDeviceAddressInfo* Info);

	std::uint64_t GetBufferOpaqueCaptureAddress(const VkBufferDeviceAddressInfo* Info);

	/*
		Framebuffer
	*/
	VkResult CreateFramebuffer(const VkFramebufferCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkFramebuffer* Framebuffer);
	void DestroyFramebuffer(VkFramebuffer Framebuffer, const VkAllocationCallbacks* Allocator);

	/*
		Shader
	*/
	VkResult CreateShaderModule(const VkShaderModuleCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkShaderModule* ShaderModule);
	void DestroyShaderModule(VkShaderModule ShaderModule, const VkAllocationCallbacks* Allocator);

	/*
		Pipeline
	*/
	VkResult CreateGraphicsPipelines(VkPipelineCache PipelineCache,
		std::uint32_t CreateInfoCount,
		const VkGraphicsPipelineCreateInfo* CreateInfos,
		const VkAllocationCallbacks* Allocator,
		VkPipeline* Pipelines);
	VkResult CreateComputePipelines(VkPipelineCache PipelineCache,
		std::uint32_t CreateInfoCount,
		const VkComputePipelineCreateInfo* CreateInfos,
		const VkAllocationCallbacks* Allocator,
		VkPipeline* Pipelines);
	void DestroyPipeline(VkPipeline Pipeline, const VkAllocationCallbacks* Allocator);

	/*
		Pipeline Cache
	*/
	VkResult CreatePipelineCache(const VkPipelineCacheCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkPipelineCache* PipelineCache);
	void DestroyPipelineCache(VkPipelineCache PipelineCache, const VkAllocationCallbacks* Allocator);

	/*
		Pipeline Layout
	*/
	VkResult CreatePipelineLayout(const VkPipelineLayoutCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkPipelineLayout* PipelineLayout);
	void DestroyPipelineLayout(VkPipelineLayout PipelineLayout, const VkAllocationCallbacks* Allocator);

	/* 
		Memory 
	*/ 
	VkResult AllocateMemory(const VkMemoryAllocateInfo* AllocateInfo, const VkAllocationCallbacks* Allocator, VkDeviceMemory* Memory);
	void FreeMemory(VkDeviceMemory Memory, const VkAllocationCallbacks* Allocator);
	VkResult MapMemory(VkDeviceMemory Memory, VkDeviceSize Offset, VkDeviceSize Size, VkMemoryMapFlags Flags, void** Data);
	void UnmapMemory(VkDeviceMemory Memory);

	/*
		Buffer
	*/
	VkResult CreateBuffer(const VkBufferCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkBuffer* Buffer);
	void CopyBuffer(VkDeviceMemory Memory, VkDeviceSize MemorySize, void* BufferData);
	void DestroyBuffer(VkBuffer Buffer, const VkAllocationCallbacks* Allocator);
	VkResult BindBufferMemory(VkBuffer Buffer, VkDeviceMemory Memory, VkDeviceSize MemoryOffset);
	void GetBufferMemoryRequirements(VkBuffer Buffer, VkMemoryRequirements* MemoryRequirements);
	void GetImageMemoryRequirements(VkImage Image, VkMemoryRequirements* MemoryRequirements);

	/*
		Buffer View
	*/
	VkBufferView CreateBufferView(VkBuffer Buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize range);
	void DestroyBufferView(VkBufferView BufferView, const VkAllocationCallbacks* Allocator);

	/*
		Image
	*/
	void MapFormatSupport(RHIPixelFormat UEFormat, VkFormat InFormat, const VkComponentMapping& ComponentMapping);
	void SetupFormats();
	VkResult CreateImage(const VkImageCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkImage* Image);
	void BindImageMemory(VkImage Image, VkDeviceMemory Memory, VkDeviceSize MemoryOffset);
	void DestroyImage(VkImage Image, const VkAllocationCallbacks* Allocator);
	void GetImageSubresourceLayout(VkImage Image, const VkImageSubresource* Subresource, VkSubresourceLayout* Layout);

	/*
		Image View
	*/
	VkResult CreateImageView(const VkImageViewCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkImageView* View);
	void DestroyImageView(VkImageView ImageView, const VkAllocationCallbacks* Allocator);

	/*
		Descriptor Set Layout
	*/
	VkResult AllocateDescriptorSets(const VkDescriptorSetAllocateInfo* AllocateInfo, VkDescriptorSet* DescriptorSets);
	void UpdateDescriptorSets(uint32_t DescriptorWriteCount, const VkWriteDescriptorSet* DescriptorWrites, uint32_t DescriptorCopyCount, const VkCopyDescriptorSet* DescriptorCopies);
	VkResult CreateDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkDescriptorSetLayout* SetLayout);
	void DestroyDescriptorSetLayout(VkDescriptorSetLayout DescriptorSetLayout, const VkAllocationCallbacks* Allocator);

	/*
		Descriptor Pool
	*/
	VkResult CreateDescriptorPool(const VkDescriptorPoolCreateInfo* CreateInfo, const VkAllocationCallbacks* Allocator, VkDescriptorPool* DescriptorPool);
	void DestroyDescriptorPool(VkDescriptorPool DescriptorPool, const VkAllocationCallbacks* Allocator);

	/*
		Query Pool
	*/
	VkQueryPool CreateQueryPool(); 
	VkBool32 GetSupportedDepthFormat(VkFormat* depthFormat);
	VkBool32 GetSupportedBlitting(VkFormat* format);

	/*
		Ray Tracing Pipeline
	*/
	VkResult CreateRayTracingPipelinesKHR(VkDeferredOperationKHR DeferredOperation,
		VkPipelineCache                             PipelineCache,
		uint32_t                                    CreateInfoCount,
		const VkRayTracingPipelineCreateInfoKHR* CreateInfos,
		const VkAllocationCallbacks* Allocator,
		VkPipeline* Pipelines);

	/*
		Acceleration Structure
	*/
	VkResult CreateAccelerationStructure(VkAccelerationStructureCreateInfoKHR& accelerationStructureCreateInfo, VkAccelerationStructureKHR* pAccelerationStructure);
	void DestroyAccelerationStructureKHR(VkAccelerationStructureKHR  AccelerationStructure, const VkAllocationCallbacks* Allocator);

	VkDeviceAddress GetAccelerationStructureDeviceAddress(const VkAccelerationStructureDeviceAddressInfoKHR* pInfo);
	 
	void GetAccelerationStructureBuildSizes(const VkAccelerationStructureBuildGeometryInfoKHR* pBuildInfo, const uint32_t* pMaxPrimitiveCounts, VkAccelerationStructureBuildSizesInfoKHR* pSizeInfo);
	void GetRayTracingShaderGroupHandles(VkPipeline pipeline, uint32_t groupCount, size_t dataSize, void* pData);

	/*
		Swap Chain
	*/
	VkResult CreateSwapchainKHR(const VkSwapchainCreateInfoKHR* CreateInfo, const VkAllocationCallbacks* Allocator, VkSwapchainKHR* Swapchain);
	void DestroySwapchainKHR(VkSwapchainKHR Swapchain, const VkAllocationCallbacks* Allocator);
	VkResult GetSwapchainImagesKHR(VkSwapchainKHR Swapchain, uint32_t* SwapchainImageCount, VkImage* SwapchainImages);
	VkResult AcquireNextImageKHR(VkSwapchainKHR Swapchain, uint64_t Timeout, VkSemaphore Semaphore, VkFence Fence, uint32_t* ImageIndex);
	VkResult AcquireNextImage2KHR(const VkAcquireNextImageInfoKHR* AcquireInfo, uint32_t* ImageIndex);

	/*
		Get Device Group Present Capabilitie
	*/
	VkResult GetDeviceGroupPresentCapabilitiesKHR(VkDeviceGroupPresentCapabilitiesKHR* DeviceGroupPresentCapabilities);
	VkResult GetDeviceGroupSurfacePresentModesKHR(VkSurfaceKHR Surface, VkDeviceGroupPresentModeFlagsKHR* Modes);



#ifdef VK_ENABLE_BETA_EXTENSIONS
	VkResult CreateMicromap(VkMicromapEXT micromap, uint64_t micromapSize, VkBuffer micromapBuffer);
	void DestroyMicromap(VkMicromapEXT micromap);
	void GetMicromapBuildSizes(std::vector<VkMicromapUsageEXT> usages);
#endif
 
    /*
		Buffer Device Address
	*/
	VkDeviceAddress GetBufferDeviceAddressKHR(const VkBufferDeviceAddressInfo* Info);

	/*
		Wait
	*/
	VkResult DeviceWaitIdle();


public:
	
	/*
		支持Host Visible的Memory Type中, heap size最大的Memory Type.
	*/
	std::uint32_t HostVisibleMemoryTypeIndexWithMaxHeapSize;
	/*
		支持Device Local的Memory Type中, heap size最大的Memory Type.
	*/
	std::uint32_t DeviceLocalMemoryTypeIndexWithMaxHeapSize;

	VulkanPhysicalDevice* PhysicalDevice;
	VulkanInstance* Instance;


	VkCommandPool m_CommandPool; //暂放
	VkQueue m_graphicsQueue;//暂放
private:
	VkDevice Handle;
};