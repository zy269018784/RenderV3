#pragma once
#include <VulkanRHI1/VulkanRHI.h>
#include <VulkanRHI1/Vulkan.h>
#include <VulkanRHI1/VulkanLoader.h>
#include <VulkanRHI1/VulkanObjects/VulkanShader.h>
#include <VulkanRHI1/VulkanObjects/Texture/VulkanTexture.h>
#include <VulkanRHI1/VulkanObjects/VulkanPipeline.h>
#include <VulkanRHI1/VulkanObjects/VulkanShaderBindings.h>
#include <VulkanRHI1/VulkanObjects/VulkanRaytracing.h>
#include <VulkanRHI1/VulkanObjects/VulkanCommandBuffer.h>

#include <VulkanRHI1/VulkanCommon.h>
#include <iostream>
#include <algorithm>
#include <assert.h>
#include <string>
#include <cstring>
#include <fstream> 
#include <chrono>

#define DEFINE_VK_ENTRYPOINTS(Type,Func) VULKANRHI_API Type VKRT::Func = NULL;
ENUM_VK_ENTRYPOINTS_RAYTRACING(DEFINE_VK_ENTRYPOINTS)

std::vector<const char*> getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	for (int i = 0; i < extensions.size(); i++) {
		printf(extensions[i]);
	}
	//if (enableValidationLayers) 
	//{
	//	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	//}

	return extensions;
}
#define GETINSTANCE_VK_ENTRYPOINTS(Type, Func) VKRT::Func = (Type)Instance->GetInstanceProcAddr(#Func); 

VulkanRHI1::VulkanRHI1(int InGpuIndex) //无窗口模式
	: GpuIndex(InGpuIndex)
{ 
	Instance = new VulkanInstance();
	ENUM_VK_ENTRYPOINTS_RAYTRACING(GETINSTANCE_VK_ENTRYPOINTS); 	//添加Raytracing函数入口

	CreatePhysicalDevice(GpuIndex);  
	CreateDeviceWithoutUI();
	// command buffer
	CreateCommandBuffer();
	//同步对象
	CreateSyncObjects();
	//Compute
	ComputeShaderResource();
	Device->SetupFormats();//预设定像素格式映射
}

#ifdef USE_QT
VulkanRHI1::VulkanRHI(QVulkanWindow* window)
{	
	VkDevice dev = window->device();
	QVulkanInstance* vulkanInstance = window->vulkanInstance();

	Instance.Handle = vulkanInstance->vkInstance();
#define GETINSTANCE_VK_ENTRYPOINTS(Type, Func) VKRT::Func = (Type)vkGetInstanceProcAddr(Instance.Handle, #Func); 
	ENUM_VK_ENTRYPOINTS_RAYTRACING(GETINSTANCE_VK_ENTRYPOINTS); 	//添加Raytracing函数入口

	Surface = new VulkanSurface();
	Surface->Handle = vulkanInstance->surfaceForWindow(window);
	PhysicalDevice.Handle = window->physicalDevice();	//CreatePhysicalDevice();
	PhysicalDevice.queueFamilyIndices.graphicsFamily = window->graphicsQueueFamilyIndex(); 
	Device.Handle = dev;//CreateDevice(); 
	Device.PhysicalDevice = &PhysicalDevice;
	Device.m_graphicsQueue = window->graphicsQueue();// GraphicsQueue.Handle;
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice.Handle, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
		if (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
			Device.HostVisibleMemoryIndex = i;
		}
	}

	printf("VulkanRHI\n");
	QVulkanDeviceFunctions* devFuncs = vulkanInstance->deviceFunctions(dev);
	 
	CommandPool = new VulkanCommandPool(Device, PhysicalDevice.queueFamilyIndices.graphicsFamily.value()); 
	CommandPool->Handle = window->graphicsCommandPool();
	Device.m_CommandPool = CommandPool->Handle;

	pCommandBuffer = new VulkanCommandBuffer(&Device, *CommandPool); 
	pPreCommandBuffer = new VulkanCommandBuffer(&Device, *CommandPool);

	SwapChain = new VulkanSwapChain(Device, PhysicalDevice, *Surface); 
	size_t size = SwapChain->swapChainImageViews.size();
	this->ConcurrentFrameCount = size;

	pRenderPass = new VulkanRenderPass(&Device, SwapChain->swapChainImageFormat);
	FrameBuffers.reserve(size);
	for (size_t i = 0; i < size; i++) {
		FrameBuffers[i] = new VulkanFrameBuffer(Device, *pRenderPass, SwapChain, i);
	}
	printf("VulkanRHI 2\n");
	//同步对象
	for (size_t i = 0; i < ConcurrentFrameCount; i++) {
		VulkanSemaphore* pImageAvailableSemaphore = new VulkanSemaphore(Device);
		ImageAvailableSemaphores.push_back(pImageAvailableSemaphore);

		VulkanSemaphore* pRenderFinishedSemaphore = new VulkanSemaphore(Device);
		RenderFinishedSemaphores.push_back(pRenderFinishedSemaphore);

		VulkanFence* pInFlightFence = new VulkanFence(&Device);
		InFlightFences.push_back(pInFlightFence);
	}
	printf("VulkanRHI 3\n");
	//////////////Compute
	//ComputeCommandPool = new VulkanCommandPool(Device, PhysicalDevice.queueFamilyIndices.computeFamily.value());
	//ComputeCommandBuffer = new VulkanCommandBuffer(&Device, *ComputeCommandPool);
	//ComputeFence = new VulkanFence(&Device);
	//printf("VulkanRHI 4\n");
	//VkPipelineCacheCreateInfo PipelineCacheCreateInfo;
	//std::memset(&PipelineCacheCreateInfo, 0, sizeof(VkPipelineCacheCreateInfo));
	//PipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO; 
	//Device.CreatePipelineCache(&PipelineCacheCreateInfo, nullptr, &this->PipelineCache.Handle);

	printf("VulkanRHI 5\n");
	Device.SetupFormats(); //预设定像素格式映射
}
#endif
VulkanRHI1::VulkanRHI1(GLFWwindow* window)
{
	Instance = new VulkanInstance();
	ENUM_VK_ENTRYPOINTS_RAYTRACING(GETINSTANCE_VK_ENTRYPOINTS); 	//添加Raytracing函数入口


	Surface = new VulkanSurface(*Instance, window);
	CreatePhysicalDevice(); 
	CreateDevice();

	SwapChain = new VulkanSwapChain(Device, *PhysicalDevice, *Surface);
	 
	size_t size = SwapChain->swapChainImageViews.size(); 
	this->ConcurrentFrameCount = size;

	pRenderPass = new VulkanRenderPass(Device, SwapChain->swapChainImageFormat); 
	FrameBuffers.reserve(size); 
	for (size_t i = 0; i < size; i++) {	
		FrameBuffers[i] = new VulkanFrameBuffer(*Device, *pRenderPass, SwapChain, i);
	}

	// command buffer
	CreateCommandBuffer();

	//同步对象
	CreateSyncObjects();

	//Compute
	ComputeShaderResource();

	Device->SetupFormats();//预设定像素格式映射

}
 
VulkanRHI1::~VulkanRHI1()
{ 	
	//std::cout << "VulkanRHI1::~VulkanRHI()" << std::endl;
	for (int i = 0; i < ImageAvailableSemaphores.size(); i++)
	{
		delete ImageAvailableSemaphores[i];
	}

	for (int i = 0; i < RenderFinishedSemaphores.size(); i++)
	{
		delete RenderFinishedSemaphores[i];
	}

	for (int i = 0; i < InFlightFences.size(); i++)
	{
		delete InFlightFences[i];
	}

	delete Fence;
	delete ComputeFence;
	/*
		在CommandPool之前释放
	*/
	delete CommandBuffer;
	delete CommandPool;
	/*
		在ComputeCommandPool之前释放
	*/
	delete ComputeCommandBuffer;
	delete ComputeCommandPool;
	delete Device;
	delete PhysicalDevice;
	delete Instance;
}

VulkanDevice* VulkanRHI1::GetDevice()
{
	return Device;
}

VulkanSwapChain* VulkanRHI1::GetSwapChain()
{
	return SwapChain;
}

void VulkanRHI1::Release()
{
	if (asBuffer) {
		delete asBuffer;
		asBuffer = nullptr;
	}
	for (int i = 0; i < asBuffers.size(); i++) {
		if (asBuffers[i]) {
			delete asBuffers[i];
			asBuffers[i] = nullptr;
		}
	}
}

VkShaderModule VulkanRHI1::ReadShader(std::string filename)
{
	size_t shaderSize;
	char* shaderCode{ nullptr };
	std::ifstream is(filename, std::ios::binary | std::ios::in | std::ios::ate);
	if (is.is_open()) {
		shaderSize = is.tellg();
		is.seekg(0, std::ios::beg);
		shaderCode = new char[shaderSize];
		is.read(shaderCode, shaderSize);
		is.close();
		assert(shaderSize > 0);
	}
	if (shaderCode) { // Create a new shader module that will be used for pipeline creation
		VkShaderModuleCreateInfo shaderModuleCI{};
		shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCI.codeSize = shaderSize;
		shaderModuleCI.pCode = (uint32_t*)shaderCode; 
		VkShaderModule shaderModule;
		Device->CreateShaderModule(&shaderModuleCI, nullptr, &shaderModule);
		delete[] shaderCode;
		return shaderModule;
	}
	else {
		std::cerr << "Error: Could not open shader file \"" << filename << "\"" << std::endl;
		return VK_NULL_HANDLE;
	}
}

/*
	1. Resource Creation
*/
RHIVertexShader* VulkanRHI1::RHILoadVertexShader(const char* filename)
{
	VkShaderModule module = ReadShader(filename);
	if(module == VK_NULL_HANDLE)
		return nullptr;
	VulkanVertexShader* Shader = new VulkanVertexShader(Device, module);
	return Shader;
}

RHIPixelShader* VulkanRHI1::RHILoadPixelShader(const char* filename)
{
	VkShaderModule module = ReadShader(filename);
	if (module == VK_NULL_HANDLE)
		return nullptr;
	VulkanPixelShader* Shader = new VulkanPixelShader(Device, module);
	return Shader;
}

RHIComputeShader* VulkanRHI1::RHILoadComputeShader(const char* filename)
{
	VkShaderModule module = ReadShader(filename);
	if (module == VK_NULL_HANDLE)
		return nullptr;
	VulkanComputeShader* Shader = new VulkanComputeShader(Device, module);
	return Shader;
}

RHIRayGenShader* VulkanRHI1::RHILoadRayGenShader(const char* filename)
{
	VkShaderModule module = ReadShader(filename);
	if (module == VK_NULL_HANDLE)
		return nullptr;
	VulkanRayGenShader* Shader = new VulkanRayGenShader(Device, module);
	return Shader;
}

RHIRayMissShader* VulkanRHI1::RHILoadRayMissShader(const char* filename)
{
	VkShaderModule module = ReadShader(filename);
	if (module == VK_NULL_HANDLE)
		return nullptr;
	VulkanRayMissShader* Shader = new VulkanRayMissShader(Device, module);
	return Shader;
}

RHIRayHitGroupShader* VulkanRHI1::RHILoadRayHitGroupShader(const char* filename, const char* filenameAnyhit, const char* filenameIntersect)
{
	VkShaderModule module = VK_NULL_HANDLE;
	if(filename)
		module = ReadShader(filename);	 

	VkShaderModule moduleAnyhit = VK_NULL_HANDLE;
	if(filenameAnyhit)
		moduleAnyhit = ReadShader(filenameAnyhit);

	VkShaderModule moduleIntersection = VK_NULL_HANDLE;
	if (filenameIntersect)
		moduleIntersection = ReadShader(filenameIntersect);

	VulkanRayHitGroupShader* Shader = new VulkanRayHitGroupShader(Device, module, moduleAnyhit, moduleIntersection);
	return Shader;
}

// create vertex shader
RHIVertexShader* VulkanRHI1::RHICreateVertexShader(std::vector<char> GlslCode)
{
	VulkanVertexShader* Shader = new VulkanVertexShader(Device, (std::uint32_t *)GlslCode.data(), GlslCode.size());
	return Shader;
}

// create pixel shader
RHIPixelShader* VulkanRHI1::RHICreatePixelShader(std::vector<char> GlslCode)
{
	VulkanPixelShader* Shader = new VulkanPixelShader(Device, (std::uint32_t*)GlslCode.data(), GlslCode.size());
	return Shader;
}

// create compute shader
RHIComputeShader* VulkanRHI1::RHICreateComputeShader(std::vector<char> GlslCode)
{
	VulkanComputeShader* Shader = new VulkanComputeShader(Device, (std::uint32_t*)GlslCode.data(), GlslCode.size());
	return Shader;
}

// create 1D texture 
RHITexture* VulkanRHI1::RHICreateTexture1D(std::uint32_t SizeX, std::uint8_t Format, std::uint32_t NumMips, TextureCreateFlags Flags, RHIAccess InResourceState)
{
	VulkanTexture1D* Texture = new VulkanTexture1D(Device, SizeX, NumMips, Format, Flags);
	return static_cast<RHITexture*>(Texture);
}

// create 2D texture 
RHITexture* VulkanRHI1::RHICreateTexture2D(std::uint32_t SizeX, std::uint32_t SizeY, std::uint8_t Format, std::uint32_t NumMips, TextureCreateFlags Flags, RHIAccess InResourceState)
{
	VulkanTexture2D* Texture = new VulkanTexture2D(Device, SizeX, SizeY, NumMips, Format, Flags);
	//std::cout << "Image View " << Texture->GetImageView() << std::endl;
	return static_cast<RHITexture*>(Texture); 
}

// create 3D texture 
RHITexture* VulkanRHI1::RHICreateTexture3D(std::uint32_t SizeX, std::uint32_t SizeY, std::uint32_t SizeZ, std::uint8_t Format, std::uint32_t NumMips, TextureCreateFlags Flags, RHIAccess InResourceState)
{
	VulkanTexture3D* Texture = new VulkanTexture3D(Device, SizeX, SizeY, SizeZ, NumMips, Format, Flags);
	return static_cast<RHITexture*>(Texture);
}

// create CubeMap texture 
RHITexture* VulkanRHI1::RHICreateTextureCube(std::uint32_t Size, std::uint8_t Format, std::uint32_t NumMips, TextureCreateFlags Flags, RHIAccess InResourceState)
{
	VulkanTextureCube* Texture = new VulkanTextureCube(Device, Size, Size, 6, NumMips, Format, Flags);
	return static_cast<RHITexture*>(Texture);
}

// create 2D texture array
RHITexture* VulkanRHI1::RHICreateTexture2DArray(std::uint32_t SizeX, std::uint32_t SizeY, std::uint32_t SizeZ, std::uint8_t Format, std::uint32_t NumMips, std::uint32_t NumSamples, TextureCreateFlags Flags, RHIAccess InResourceState)
{
	VulkanTexture2DArray* Texture = new VulkanTexture2DArray(Device, SizeX, SizeY, SizeZ, NumMips, Format, Flags);
	return static_cast<RHITexture*>(Texture);
}

// create CubeMap texture array
RHITexture* VulkanRHI1::RHICreateTextureCubeArray(std::uint32_t SizeX, std::uint32_t SizeY, std::uint32_t SizeZ, std::uint8_t Format, std::uint32_t NumMips, std::uint32_t NumSamples, TextureCreateFlags Flags, RHIAccess InResourceState)
{
	VulkanTextureCubeArray* Texture = new VulkanTextureCubeArray(Device, SizeX, SizeY, SizeZ, NumMips, Format, Flags);
	return static_cast<RHITexture*>(Texture);
}

// create buffer
RHIBuffer* VulkanRHI1::RHICreateBuffer(std::uint32_t Size, BufferUsageFlags Usage, std::uint32_t Stride, const void* Data)
{
	VulkanBuffer* Buffer = new VulkanBuffer(Device, Stride, Size, Usage, Data);

	return Buffer;
}

RHISampler* VulkanRHI1::RHINewSampler()
{
	VulkanSampler* sampler = new VulkanSampler(Device);
	sampler->setAddressMode(EAddressMode::REPEAT);
	return static_cast<RHISampler*>(sampler);
}


RHIPipeline* VulkanRHI1::RHINewGraphicsPipeline() 
{
	VulkanGraphicsPipeline* Pipeline = new VulkanGraphicsPipeline(Device);
	return static_cast<RHIPipeline*>(Pipeline);
}

RHIPipeline* VulkanRHI1::RHINewComputePipeline()
{
	VulkanComputePipeline* Pipeline = new VulkanComputePipeline(Device);
	return static_cast<RHIPipeline*>(Pipeline);
} 

RHIPipeline* VulkanRHI1::RHINewRaytracingPipeline()
{
	VulkanRaytracingPipeline* Pipeline = new VulkanRaytracingPipeline(Device);
	return static_cast<RHIPipeline*>(Pipeline);
}

RHIShaderBindings* VulkanRHI1::RHINewShaderBindings()
{
	VulkanShaderBindings* pShaderBindings = new VulkanShaderBindings(Device);
	return static_cast<RHIShaderBindings*>(pShaderBindings);
}

RHIRenderTarget* VulkanRHI1::RHINewRenderTarget()
{
	VulkanRenderTarget* rt = nullptr;// new VulkanRenderTarget(Device); todo...
	return nullptr;// static_cast<RHIRenderTarget*>(rt);
}

RHITextureRenderTarget* VulkanRHI1::RHINewTextureRenderTarget(const RHIRenderTargetDesc& desc)
{
	VulkanRenderTarget* rt = new VulkanRenderTarget(Device, desc);
	return static_cast<RHITextureRenderTarget*>(rt);
}

RHIRenderPass* VulkanRHI1::RHINewRenderPass()
{
	VulkanRenderPass* rp = new VulkanRenderPass();
	return static_cast<RHIRenderPass*>(rp);
}

RHIRaytracingBLAS* VulkanRHI1::RHINewRaytracingBLAS()
{
#if 0
	RHIRaytracingGeometryData* geoData = = nullptr;
#else
	RHIRaytracingGeometryData* geoData = new RHIRaytracingGeometryData(); 
	{  
		struct Vertex {
			float pos[3];
		};

		std::vector<Vertex> vertices = {
			{ -1.0, -1.0, 0.0 },
			{  1.0, -1.0, 0.0 },
			{ -1.0,  1.0, 0.0 }
		};
		std::vector<uint32_t> indices = { 0, 1, 2 };

		VkTransformMatrixKHR transformMatrix = {
			1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f
		};

		RHIRaytracingASGeometry* pGeo = new RHIRaytracingASGeometry(); 
		VulkanBuffer* VBO = new VulkanBuffer(Device, sizeof(Vertex), vertices.size() * sizeof(Vertex), BUF_AccelerationStructure, vertices.data());
		VulkanBuffer* EBO = new VulkanBuffer(Device, 0, indices.size() * sizeof(uint32_t), BUF_AccelerationStructure, indices.data());
		VulkanBuffer* TransformBuffer= new VulkanBuffer(Device, 0, sizeof(VkTransformMatrixKHR), BUF_AccelerationStructure, &transformMatrix);
		pGeo->MaxVertices = vertices.size();
		pGeo->indexCount = indices.size();
		pGeo->VertexBufferStride = sizeof(Vertex);
		pGeo->vertexAddress = VBO->DeviceAddress;
		pGeo->indexAddress = EBO->DeviceAddress;
		pGeo->transformAddress = TransformBuffer->DeviceAddress;
		geoData->ASGeometries.push_back(pGeo); 
	}
	{
		struct Vertex {
			float pos[3];
		};

		std::vector<Vertex> vertices = {
			{ -1.0, -1.0, 0.0 },
			{  1.0, -1.0, 0.0 },
			{ -1.0,  1.0, 0.0 }
		};
		std::vector<uint32_t> indices = { 0, 1, 2 };

		VkTransformMatrixKHR transformMatrix = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f
		};

		RHIRaytracingASGeometry* pGeo = new RHIRaytracingASGeometry();
		VulkanBuffer* VBO = new VulkanBuffer(Device, sizeof(Vertex), vertices.size() * sizeof(Vertex), BUF_AccelerationStructure, vertices.data());
		VulkanBuffer* EBO = new VulkanBuffer(Device, 0, indices.size() * sizeof(uint32_t), BUF_AccelerationStructure, indices.data());
		VulkanBuffer* TransformBuffer = new VulkanBuffer(Device, 0, sizeof(VkTransformMatrixKHR), BUF_AccelerationStructure, &transformMatrix);
		pGeo->MaxVertices = vertices.size();
		pGeo->indexCount = indices.size();
		pGeo->VertexBufferStride = sizeof(Vertex);
		pGeo->vertexAddress = VBO->DeviceAddress;
		pGeo->indexAddress = EBO->DeviceAddress;
		pGeo->transformAddress = TransformBuffer->DeviceAddress;
		geoData->ASGeometries.push_back(pGeo);
	}
	{
		struct Vertex {
			float pos[3];
		};

		std::vector<Vertex> vertices = {
			{ -1.0, -1.0, 0.0 },
			{  1.0, -1.0, 0.0 },
			{ -1.0,  1.0, 0.0 }
		};
		std::vector<uint32_t> indices = { 0, 1, 2 };

		VkTransformMatrixKHR transformMatrix = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 0.0f
		};

		RHIRaytracingASGeometry* pGeo = new RHIRaytracingASGeometry();
		VulkanBuffer* VBO = new VulkanBuffer(Device, sizeof(Vertex), vertices.size() * sizeof(Vertex), BUF_AccelerationStructure, vertices.data());
		VulkanBuffer* EBO = new VulkanBuffer(Device, 0, indices.size() * sizeof(uint32_t), BUF_AccelerationStructure, indices.data());
		VulkanBuffer* TransformBuffer = new VulkanBuffer(Device, 0, sizeof(VkTransformMatrixKHR), BUF_AccelerationStructure, &transformMatrix);
		pGeo->MaxVertices = vertices.size();
		pGeo->indexCount = indices.size();
		pGeo->VertexBufferStride = sizeof(Vertex);
		pGeo->vertexAddress = VBO->DeviceAddress;
		pGeo->indexAddress = EBO->DeviceAddress;
		pGeo->transformAddress = TransformBuffer->DeviceAddress;
		geoData->ASGeometries.push_back(pGeo);
	}

#endif 
	VulkanRaytracingBLAS* blas = new VulkanRaytracingBLAS(Device, geoData); 
	return static_cast<RHIRaytracingBLAS*>(blas);
}

RHIRaytracingBLAS* VulkanRHI1::RHINewRaytracingBLAS(RHIRaytracingGeometryData* geoData)
{
	VulkanRaytracingBLAS* blas = new VulkanRaytracingBLAS(Device, geoData);
	return static_cast<RHIRaytracingBLAS*>(blas);
}

RHIRaytracingBLAS* VulkanRHI1::RHINewRaytracingBLAS(RHIRaytracingGeometryDataSimple* geoData)
{
	VulkanRaytracingBLAS* blas = new VulkanRaytracingBLAS(Device, geoData);
	return static_cast<RHIRaytracingBLAS*>(blas);
}

std::vector< RHIRaytracingBLAS* > VulkanRHI1::RHICreateRaytracingBLASs(std::vector< RHIRaytracingGeometryData* >& geometries)
{
	std::vector< RHIRaytracingBLAS* > BLASs;
	BLASs.reserve(geometries.size());
	for (int i = 0; i < geometries.size(); i++) {
		RHIRaytracingGeometryData* geoData = geometries[i];
		VulkanRaytracingBLAS* blas = new VulkanRaytracingBLAS(Device, geoData);
		BLASs.push_back(static_cast<RHIRaytracingBLAS*>(blas));
	}
	return BLASs;
}

struct ASPart {
	uint64_t accelerationStructureSize;
	uint64_t BuildScratchSize; 
	int partIndex;
};

VulkanCommandBuffer* VulkanRHI1::BeginCommand()
{
	VulkanCommandBuffer* pCmdBuffer = new VulkanCommandBuffer(Device, CommandPool);
	//pCmdBuffer->BeginOneTimeGraphicsCommand();
	pCmdBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	return pCmdBuffer;
}

void VulkanRHI1::EndCommand(VulkanCommandBuffer* CmdBuffer)
{
	CmdBuffer->End();

	VkCommandBuffer CommandBuffer = CmdBuffer->GetHandle();

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CommandBuffer;

	vkQueueSubmit(Device->m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(Device->m_graphicsQueue);
	
	//Device->FreeCommandBuffers(Device->m_CommandPool, 1, &CommandBuffer);
	if (CmdBuffer) 
	{
		delete CmdBuffer;
	}
}

void VulkanRHI1::InitMultiThreadCommandBuffers(int threads)
{
	cmdBuffers.reserve(threads);
	for (int i = 0; i < threads; i++) {
		cmdBuffers[i] = new VulkanCommandBuffer(Device, CommandPool);
	}
}

void VulkanRHI1::BeginCommands()
{
	for (int i = 0; i < cmdBuffers.size(); i++) {
		//cmdBuffers[i]->BeginOneTimeGraphicsCommand();
		cmdBuffers[i]->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	}
}

void VulkanRHI1::EndCommands()
{
	for (int i = 0; i < cmdBuffers.size(); i++) {
		cmdBuffers[i]->End();
	}
}

void VulkanRHI1::SubmitCommands()
{
	std::vector<VkCommandBuffer> vkCmdBuffers(cmdBuffers.size());
	for (int i = 0; i < cmdBuffers.size(); i++) {
		vkCmdBuffers[i] = cmdBuffers[i]->GetHandle();
	}
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = vkCmdBuffers.size();
	submitInfo.pCommandBuffers = vkCmdBuffers.data();
	vkQueueSubmit(Device->m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(Device->m_graphicsQueue);
	Device->FreeCommandBuffers(Device->m_CommandPool, vkCmdBuffers.size(), vkCmdBuffers.data());
}

void VulkanRHI1::DestroyMultiThreadCommandBuffers()
{
	for (int i = 0; i < cmdBuffers.size(); i++) {
		delete cmdBuffers[i];
	}
}

void threadFunc(int i,int threadIndex, VulkanRaytracingBLAS* blas, std::vector< VulkanCommandBuffer* > cmdBuffers,VulkanBuffer* asBuffer, VulkanBuffer* rtScratchBuffer, std::vector<uint64_t> ResultSizes, std::vector<uint64_t> BuildScratchSizes)
{
	blas->build2(cmdBuffers[threadIndex], asBuffer, ResultSizes[i], rtScratchBuffer, BuildScratchSizes[i]);
}

void threadBuildAS(int start, int end, std::vector < RHIRaytracingBLAS*> BLASs, VkCommandBuffer cmdBuffer, VulkanBuffer* asBuffer, std::vector<uint64_t> ResultSizes, VulkanBuffer* rtScratchBuffer, std::vector<uint64_t> BuildScratchSizes)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;	

	vkBeginCommandBuffer(cmdBuffer, &beginInfo);
	for (int i = start; i < end; i++) {
		VulkanRaytracingBLAS* blas = (VulkanRaytracingBLAS*)BLASs[i];
		blas->build3(cmdBuffer, asBuffer, ResultSizes[i], rtScratchBuffer, BuildScratchSizes[i]);

	}	
	vkEndCommandBuffer(cmdBuffer);
} 
 
void threadBuildAS2(int start, int end, std::vector < RHIRaytracingBLAS*> BLASs, std::vector<ASPart> asParts, VkCommandBuffer cmdBuffer, VulkanBuffer* asBuffer, std::vector<uint64_t> ResultSizes, VulkanBuffer* rtScratchBuffer, std::vector<uint64_t> BuildScratchSizes)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VkMemoryBarrier memoryBarrier = {};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
	memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

	vkBeginCommandBuffer(cmdBuffer, &beginInfo);
	for (int i = start; i < end; i++) { 
		VulkanRaytracingBLAS* blas = (VulkanRaytracingBLAS*)BLASs[i];   
		blas->build3(cmdBuffer, asBuffer, ResultSizes[i], rtScratchBuffer, BuildScratchSizes[i]);
		vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, 0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
	}
	vkEndCommandBuffer(cmdBuffer);
}

#define USE_MERGE_COMMANDBUFFER 1

#if USE_MERGE_COMMANDBUFFER
#define USE_MULTI_THREAD 1
void VulkanRHI1::RHIBuildBLASs(std::vector< RHIRaytracingBLAS* >& BLASs)
{
#if 1
	const uint64_t maxAllocateSize = 2294967295;
	VkAccelerationStructureBuildSizesInfoKHR total{}; 
	std::vector<uint64_t> ResultSizes;
	std::vector<uint64_t> BuildScratchSizes;
	uint64_t ASTotalSize = 0;
	uint64_t BSTotalSize = 0;
	 
	std::vector<ASPart> asParts;

	uint64_t maxBSSize = 0;

	int i = 0;
	for (const auto& blas : BLASs) {
		uint64_t ASSize = blas->GetSizeInfo().ResultSize;	
		uint64_t BSSize = blas->GetSizeInfo().BuildScratchSize;


		if (ASTotalSize + ASSize > maxAllocateSize) {
			ASPart p;
			p.accelerationStructureSize = ASTotalSize;
			p.BuildScratchSize = BSTotalSize;
			p.partIndex = i; 
			asParts.push_back(p);	
			if (BSTotalSize > maxBSSize)
				maxBSSize = BSTotalSize;
			ASTotalSize = BSTotalSize = 0; 
		}
		 
		total.accelerationStructureSize += ASSize;
		total.buildScratchSize += BSSize;
		//total.updateScratchSize += blas->GetSizeInfo().UpdateScratchSize; 
		ResultSizes.push_back(ASTotalSize);
		BuildScratchSizes.push_back(BSTotalSize);	
		
		ASTotalSize += ASSize;
		BSTotalSize += BSSize;
		i++;
	} 

	bool allInOne = total.accelerationStructureSize < maxAllocateSize; 
	if (!allInOne) {
		ASPart p;
		p.accelerationStructureSize = ASTotalSize;
		p.BuildScratchSize = BSTotalSize; 
		p.partIndex = i;
		asParts.push_back(p);
		if (BSTotalSize > maxBSSize)
			maxBSSize = BSTotalSize;
	}
	std::cout << "allInOne " << allInOne << std::endl;

	std::cout << "[SCENE] Acceleration Structure size = "<< total.accelerationStructureSize << "," << static_cast<double>(total.accelerationStructureSize) / 1024 / 1024 << " MB" << std::endl;
	std::cout << "[SCENE] Build Scratch size = " << total.buildScratchSize << "," << static_cast<double>(total.buildScratchSize) / 1024 / 1024 << " MB" << std::endl;
	//std::cout << "[SCENE] Update Scratch size = " << total.updateScratchSize << "," << static_cast<double>(total.updateScratchSize) / 1024 / 1024 << " MB" << std::endl;
	std::cout << "asParts size = " << asParts.size() << std::endl;

#if USE_MULTI_THREAD
	if(BLASs.size() > 10000){//加过10000才开多线程
		printf("use multi-thread\n");
		if (allInOne) {
			asBuffer = new VulkanBuffer(Device, 0, total.accelerationStructureSize, BUF_AccelerationStructure, nullptr);
			VulkanBuffer* rtScratchBuffer = new VulkanBuffer(Device, 0, total.buildScratchSize, BUF_RayTracingScratch, nullptr);
			int numThreads = 6;
			threadPool.setThreadCount(numThreads);
			int nBLASs = BLASs.size();
			int CommandBufferSize = numThreads;


			std::vector<VulkanCommandPool*> CommandPools;// (CommandBufferSize);
			//std::vector<VkCommandPool> cmdPools(CommandBufferSize);
			for (int i = 0; i < CommandBufferSize; i++) {
				VulkanCommandPool* CP = new VulkanCommandPool(Device, queueFamilyIndex);
				CommandPools.emplace_back(CP);
/*
				VkCommandPoolCreateInfo PoolInfo{};
				PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
				PoolInfo.queueFamilyIndex = queueFamilyIndex;
				if (Device->CreateCommandPool(&PoolInfo, nullptr, &cmdPools[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create command pool!");
				}
*/
			}
			std::vector<VkCommandBuffer> cmdBuffers(CommandBufferSize);
			for (int i = 0; i < CommandBufferSize; i++) {
				VkCommandBufferAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				//allocInfo.commandPool = cmdPools[i];
				allocInfo.commandPool = CommandPools[i]->GetHandle();
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandBufferCount = 1;// CommandBufferSize;
				Device->AllocateCommandBuffers(&allocInfo, &cmdBuffers[i]);
			}
			int n = BLASs.size();
			int tile = n / numThreads;
			printf("tile %d\n", tile);
			std::vector<int> starts, ends;
			for (int i = 0; i < numThreads; i++) {
				starts.push_back(tile * i);
				if (i == numThreads - 1) {
					ends.push_back(n);
				}
				else
					ends.push_back(tile * (i + 1));
			}
			std::cout << "debug 5 " << std::endl;
			for (int i = 0; i < numThreads; i++) {
				threadPool.threads[i]->addJob([=] { threadBuildAS(starts[i], ends[i], BLASs, cmdBuffers[i], asBuffer, ResultSizes, rtScratchBuffer, BuildScratchSizes); });
			}
			threadPool.wait();

			//std::cout << "aaa 1" << std::endl;
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = CommandBufferSize;
			submitInfo.pCommandBuffers = cmdBuffers.data();

			VkFence fence;
			VkFenceCreateInfo fenceCreateInfo = {};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = 0;
			Device->CreateFence(&fenceCreateInfo, nullptr, &fence);
			vkQueueSubmit(Device->m_graphicsQueue, 1, &submitInfo, fence);
			Device->WaitForFences(1, &fence, VK_TRUE, UINT64_MAX);
			Device->DestroyFence(fence, nullptr);

			vkQueueWaitIdle(Device->m_graphicsQueue);
			for (int i = 0; i < CommandBufferSize; i++) 
			{			
				Device->FreeCommandBuffers(CommandPools[i]->GetHandle(), 1, &cmdBuffers[i]);
			}

			if (rtScratchBuffer) {
				delete rtScratchBuffer;
			}
			for (const auto& blas : BLASs) {
				blas->GetASDeviceAddress();
			}
			for (int i = 0; i < CommandPools.size(); i++)
			{
				delete CommandPools[i];
;			}
		}
		else
		{
			asBuffers.reserve(asParts.size());
			for (int i = 0; i < asParts.size(); i++) {
				asBuffers[i] = new VulkanBuffer(Device, 0, asParts[i].accelerationStructureSize, BUF_AccelerationStructure, nullptr);
			}
			VulkanBuffer* rtScratchBuffer = new VulkanBuffer(Device, 0, maxBSSize, BUF_RayTracingScratch, nullptr);

			int numThreads = asParts.size();
			threadPool.setThreadCount(numThreads);

			int nBLASs = BLASs.size();
			int CommandBufferSize = numThreads;

			std::vector<VkCommandPool> cmdPools(CommandBufferSize);
			for (int i = 0; i < CommandBufferSize; i++) {
				VkCommandPoolCreateInfo PoolInfo{};
				PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
				PoolInfo.queueFamilyIndex = queueFamilyIndex;
				if (Device->CreateCommandPool(&PoolInfo, nullptr, &cmdPools[i]) != VK_SUCCESS) {
					throw std::runtime_error("failed to create command pool!");
				}
			}

			std::vector<VkCommandBuffer> cmdBuffers(CommandBufferSize);
			for (int i = 0; i < CommandBufferSize; i++) {
				VkCommandBufferAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.commandPool = cmdPools[i];// Device.m_CommandPool;
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandBufferCount = 1;// CommandBufferSize;
				Device->AllocateCommandBuffers(&allocInfo, &cmdBuffers[i]);
			}

			int n = BLASs.size();
			int tile = n / numThreads;
			printf("tile %d\n", tile);
			std::vector<int> starts, ends;
			int start = 0;
			for (int i = 0; i < numThreads; i++) {
				starts.push_back(start);
				ends.push_back(asParts[i].partIndex);
				start = asParts[i].partIndex;
				//printf("%d\n", asParts[i].partIndex);
			}
			//std::cout << "aaa 0" << std::endl;
			for (int i = 0; i < numThreads; i++) {
				threadPool.threads[i]->addJob([=] { threadBuildAS2(starts[i], ends[i], BLASs, asParts, cmdBuffers[i], asBuffers[i], ResultSizes, rtScratchBuffer, BuildScratchSizes); });
			}
			threadPool.wait();

			//std::cout << "aaa 1" << std::endl;
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = CommandBufferSize;
			submitInfo.pCommandBuffers = cmdBuffers.data();

			VkFence fence;
			VkFenceCreateInfo fenceCreateInfo = {};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = 0;
			Device->CreateFence(&fenceCreateInfo, nullptr, &fence);
			vkQueueSubmit(Device->m_graphicsQueue, 1, &submitInfo, fence);
			Device->WaitForFences(1, &fence, VK_TRUE, UINT64_MAX);
			Device->DestroyFence(fence, nullptr);

			vkQueueWaitIdle(Device->m_graphicsQueue);
			for (int i = 0; i < CommandBufferSize; i++) {
				Device->FreeCommandBuffers(cmdPools[i], 1, &cmdBuffers[i]);
			}

			if (rtScratchBuffer) {
				delete rtScratchBuffer;
			}
			for (const auto& blas : BLASs) {
				blas->GetASDeviceAddress();
			}

			for (int i = 0; i < cmdPools.size(); i++)
			{
				Device->DestroyCommandPool(cmdPools[i], nullptr);
			}
		}
	}
	else {
		if (allInOne) {
			asBuffer = new VulkanBuffer(Device, 0, total.accelerationStructureSize, BUF_AccelerationStructure, nullptr);
			VulkanBuffer* rtScratchBuffer = new VulkanBuffer(Device, 0, total.buildScratchSize, BUF_RayTracingScratch, nullptr); 
			VulkanCommandBuffer* pCmdBuffer = BeginCommand();
			for (int i = 0; i < BLASs.size(); i++) {
				BLASs[i]->build2(pCmdBuffer, asBuffer, ResultSizes[i], rtScratchBuffer, BuildScratchSizes[i]);
			}
			EndCommand(pCmdBuffer); 
			if (rtScratchBuffer) {
				delete rtScratchBuffer;
			}
			for (const auto& blas : BLASs) {
				blas->GetASDeviceAddress();
			}
		} 
		else {
			VulkanCommandBuffer* pCmdBuffer = BeginCommand(); 
			VkMemoryBarrier memoryBarrier = {};
			memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
			memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
			memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR; 
			asBuffers.reserve(asParts.size());
			for (int i = 0; i < asParts.size(); i++) {
				asBuffers[i] = new VulkanBuffer(Device, 0, asParts[i].accelerationStructureSize, BUF_AccelerationStructure, nullptr);
			}
			VulkanBuffer* rtScratchBuffer = new VulkanBuffer(Device, 0, maxBSSize, BUF_RayTracingScratch, nullptr); 
			for (int i = 0; i < BLASs.size(); i++) {
				RHIRaytracingBLAS* blas = BLASs[i];
				int bufferIndex = 0;
				for (int j = 0; j < asParts.size(); j++) {
					if (i >= asParts[j].partIndex) {
						bufferIndex++;
					}
				}
				blas->build2(pCmdBuffer, asBuffers[bufferIndex], ResultSizes[i], rtScratchBuffer, BuildScratchSizes[i]);
				vkCmdPipelineBarrier(pCmdBuffer->GetHandle(), VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, 0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
			}
			EndCommand(pCmdBuffer); 
			if (rtScratchBuffer) {
				delete rtScratchBuffer;
			}
			for (const auto& blas : BLASs) {
				blas->GetASDeviceAddress();
			}
		}
	}

#else
	if (allInOne) {  
 		asBuffer = new VulkanBuffer(&Device, 0, total.accelerationStructureSize, BUF_AccelerationStructure, nullptr); 
		VulkanBuffer* rtScratchBuffer = new VulkanBuffer(&Device, 0, total.buildScratchSize, BUF_RayTracingScratch, nullptr);

		VulkanCommandBuffer* pCmdBuffer = BeginCommand();	   
		for (int i = 0; i < BLASs.size(); i++) {
			BLASs[i]->build2(pCmdBuffer, asBuffer, ResultSizes[i], rtScratchBuffer, BuildScratchSizes[i]);
		} 
		EndCommand(pCmdBuffer);

		if (rtScratchBuffer) {
			delete rtScratchBuffer;
		} 
		for (const auto& blas : BLASs) {
			blas->GetASDeviceAddress();
		}
	}

	else
	{
		VulkanCommandBuffer* pCmdBuffer = BeginCommand();

		VkMemoryBarrier memoryBarrier = {};
		memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
		memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

		asBuffers.reserve(asParts.size());
		for (int i = 0; i < asParts.size(); i++) {
			asBuffers[i] = new VulkanBuffer(&Device, 0, asParts[i].accelerationStructureSize, BUF_AccelerationStructure, nullptr);
		}
		VulkanBuffer* rtScratchBuffer = new VulkanBuffer(&Device, 0, maxBSSize, BUF_RayTracingScratch, nullptr);

		for (int i = 0; i < BLASs.size(); i++) {
			RHIRaytracingBLAS* blas = BLASs[i];
			int bufferIndex = 0;
			for (int j = 0; j < asParts.size(); j++) {
				if (i >= asParts[j].partIndex) {
					bufferIndex++;
				}
			}
			blas->build2(pCmdBuffer, asBuffers[bufferIndex], ResultSizes[i], rtScratchBuffer, BuildScratchSizes[i]);
			vkCmdPipelineBarrier(pCmdBuffer->Handle, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, 0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
		}
		EndCommand(pCmdBuffer);

		if (rtScratchBuffer) {
			delete rtScratchBuffer;
		}
		for (const auto& blas : BLASs) {
			blas->GetASDeviceAddress();
		}
	} 
#endif	//multithread

#else
	VkAccelerationStructureBuildSizesInfoKHR total{};
	std::vector<uint64_t> ResultSizes;
	std::vector<uint64_t> BuildScratchSizes;
	uint64_t maxBSSize = 0;
	for (const auto& blas : BLASs) {
		uint64_t ASSize = blas->GetSizeInfo().ResultSize;
		uint64_t BSSize = blas->GetSizeInfo().BuildScratchSize;
		if (BSSize > maxBSSize) 
			maxBSSize = BSSize; 

		total.accelerationStructureSize += ASSize;
		total.buildScratchSize += BSSize;
		total.updateScratchSize += blas->GetSizeInfo().UpdateScratchSize;
		//std::cout << "resultOffset " << ASSize << " total.accelerationStructureSize " << total.accelerationStructureSize << std::endl; 
		ResultSizes.push_back(ASSize);
		BuildScratchSizes.push_back(blas->GetSizeInfo().BuildScratchSize);
	} 
	std::cout << maxBSSize << std::endl;
	std::cout << "[SCENE] Acceleration Structure size = " << total.accelerationStructureSize << "," << static_cast<double>(total.accelerationStructureSize) / 1024 / 1024 << " MB" << std::endl;
	std::cout << "[SCENE] Build Scratch size = " << total.buildScratchSize << "," << static_cast<double>(total.buildScratchSize) / 1024 / 1024 << " MB" << std::endl;
	
	VulkanBuffer* rtScratchBuffer = new VulkanBuffer(&Device, 0, maxBSSize, BUF_RayTracingScratch, nullptr);
std::cout << "[SCENE] Update Scratch size = " << total.updateScratchSize << "," << static_cast<double>(total.updateScratchSize) / 1024 / 1024 << " MB" << std::endl;
	asBuffers.reserve(BLASs.size()); 
	VulkanCommandBuffer* pCmdBuffer = BeginCommand();

	VkMemoryBarrier memoryBarrier = {};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
	memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

	int i = 0;
	for (const auto& blas : BLASs) {
		asBuffers[i] = new VulkanBuffer(&Device, 0, blas->GetSizeInfo().ResultSize, BUF_AccelerationStructure, nullptr); 
		//auto start1 = std::chrono::high_resolution_clock::now();
		blas->build2(pCmdBuffer, asBuffers[i], 0, rtScratchBuffer, 0); 
		// guard our scratch buffer
		vkCmdPipelineBarrier(pCmdBuffer->Handle,
			VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
			VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
			0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
		i++;
	}   
	EndCommand(pCmdBuffer);	
	
	if (rtScratchBuffer) {
		delete rtScratchBuffer;
	}
	for (const auto& blas : BLASs) {
		blas->GetDeviceAddress();
	}
#endif
}
#else
void VulkanRHI1::RHIBuildBLASs(std::vector< RHIRaytracingBLAS* >& BLASs)
{
#if 0
	const uint64_t maxAllocateSize = 2294967295; //4294967295;最大可分配   2294967295速度较快，且device_local支持
	VkAccelerationStructureBuildSizesInfoKHR total{};
	std::vector<uint64_t> ResultSizes;
	std::vector<uint64_t> BuildScratchSizes;
	uint64_t ASTotalSize = 0;
	uint64_t BSTotalSize = 0;

	std::vector<ASPart> asParts;

	int i = 0;
	for (const auto& blas : BLASs) {
		uint64_t ASSize = blas->GetSizeInfo().ResultSize;
		uint64_t BSSize = blas->GetSizeInfo().BuildScratchSize;

		if (ASTotalSize + ASSize > maxAllocateSize) {
			ASPart p;
			p.accelerationStructureSize = ASTotalSize;
			p.BuildScratchSize = BSTotalSize;
			p.partIndex = i;
			asParts.push_back(p);

			ASTotalSize = BSTotalSize = 0;
		}

		total.accelerationStructureSize += ASSize;
		total.buildScratchSize += BSSize;
		//total.updateScratchSize += blas->GetSizeInfo().UpdateScratchSize; 
		ResultSizes.push_back(ASTotalSize);
		BuildScratchSizes.push_back(BSTotalSize);

		ASTotalSize += ASSize;
		BSTotalSize += BSSize;
		i++;
	}

	bool allInOne = total.accelerationStructureSize < maxAllocateSize;
	if (!allInOne) {
		ASPart p;
		p.accelerationStructureSize = ASTotalSize;
		p.BuildScratchSize = BSTotalSize;
		p.partIndex = i;
		asParts.push_back(p);
	}

	std::cout << "[SCENE] Acceleration Structure size = " << total.accelerationStructureSize << "," << static_cast<double>(total.accelerationStructureSize) / 1024 / 1024 << " MB" << std::endl;
	std::cout << "[SCENE] Build Scratch size = " << total.buildScratchSize << "," << static_cast<double>(total.buildScratchSize) / 1024 / 1024 << " MB" << std::endl;
	//std::cout << "[SCENE] Update Scratch size = " << total.updateScratchSize << "," << static_cast<double>(total.updateScratchSize) / 1024 / 1024 << " MB" << std::endl;
	std::cout << "asParts size = " << asParts.size() << std::endl;
	if (allInOne) {
		asBuffer = new VulkanBuffer(&Device, 0, total.accelerationStructureSize, BUF_AccelerationStructure, nullptr);
		VulkanBuffer* rtScratchBuffer = new VulkanBuffer(&Device, 0, total.buildScratchSize, BUF_RayTracingScratch, nullptr);
		VkDeviceSize resultOffset = 0;
		VkDeviceSize scratchOffset = 0;
		for (const auto& blas : BLASs) {
			//auto start1 = std::chrono::high_resolution_clock::now();
			blas->build(asBuffer, resultOffset, rtScratchBuffer, scratchOffset);
			//auto end1 = std::chrono::high_resolution_clock::now();
			//auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
			//std::cout << duration1.count() / 1000.0 << "ms" << std::endl;
			//std::cout << "resultOffset " << resultOffset << ", scratchOffset " << scratchOffset << std::endl; 
			resultOffset += blas->GetSizeInfo().ResultSize; ;// .accelerationStructureSize;
			scratchOffset += blas->GetSizeInfo().BuildScratchSize;// .buildScratchSize; 
		}
		if (rtScratchBuffer) {
			delete rtScratchBuffer;
		}
	}
	else
	{
		asBuffers.reserve(asParts.size());
		std::vector<VulkanBuffer*> rtScratchBuffers(asParts.size());

		for (int i = 0; i < asParts.size(); i++) {
			asBuffers[i] = new VulkanBuffer(&Device, 0, asParts[i].accelerationStructureSize, BUF_AccelerationStructure, nullptr);
			rtScratchBuffers[i] = new VulkanBuffer(&Device, 0, asParts[i].BuildScratchSize, BUF_RayTracingScratch, nullptr);
		}

		for (int i = 0; i < BLASs.size(); i++) {
			RHIRaytracingBLAS* blas = BLASs[i];
			int bufferIndex = 0;
			for (int j = 0; j < asParts.size(); j++) {
				if (i >= asParts[j].partIndex) {
					bufferIndex++;
				}
			}
			blas->build(asBuffers[bufferIndex], ResultSizes[i], rtScratchBuffers[bufferIndex], BuildScratchSizes[i]);
		}

		for (int i = 0; i < asParts.size(); i++) {
			if (rtScratchBuffers[i]) {
				delete rtScratchBuffers[i];
			}
		}
		rtScratchBuffers.clear();
	}

#else
	VkAccelerationStructureBuildSizesInfoKHR total{};
	std::vector<uint64_t> ResultSizes;
	std::vector<uint64_t> BuildScratchSizes;
	for (const auto& blas : BLASs) {
		uint64_t ASSize = blas->GetSizeInfo().ResultSize;
		total.accelerationStructureSize += blas->GetSizeInfo().ResultSize;
		total.buildScratchSize += blas->GetSizeInfo().BuildScratchSize;
		total.updateScratchSize += blas->GetSizeInfo().UpdateScratchSize;
		//std::cout << "resultOffset " << ASSize << " total.accelerationStructureSize " << total.accelerationStructureSize << std::endl; 
		ResultSizes.push_back(ASSize);
		BuildScratchSizes.push_back(blas->GetSizeInfo().BuildScratchSize);
	}
	std::cout << "[SCENE] Acceleration Structure size = " << total.accelerationStructureSize << "," << static_cast<double>(total.accelerationStructureSize) / 1024 / 1024 << " MB" << std::endl;
	std::cout << "[SCENE] Build Scratch size = " << total.buildScratchSize << "," << static_cast<double>(total.buildScratchSize) / 1024 / 1024 << " MB" << std::endl;
	std::cout << "[SCENE] Update Scratch size = " << total.updateScratchSize << "," << static_cast<double>(total.updateScratchSize) / 1024 / 1024 << " MB" << std::endl;

	asBuffers.reserve(BLASs.size());

	int i = 0;
	for (const auto& blas : BLASs) {
		asBuffers[i] = new VulkanBuffer(&Device, 0, blas->GetSizeInfo().ResultSize, BUF_AccelerationStructure, nullptr);
		VulkanBuffer* rtScratchBuffer = new VulkanBuffer(&Device, 0, blas->GetSizeInfo().BuildScratchSize, BUF_RayTracingScratch, nullptr);
		//auto start1 = std::chrono::high_resolution_clock::now();
		blas->build(asBuffers[i], 0, rtScratchBuffer, 0);
		//auto end1 = std::chrono::high_resolution_clock::now();
		//auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
		//std::cout << duration1.count() / 1000.0 << "ms" << std::endl;
		//std::cout << "resultOffset " << resultOffset << ", scratchOffset " << scratchOffset << std::endl;

		//resultOffset += ResultSizes[i];// .accelerationStructureSize;
		//scratchOffset += BuildScratchSizes[i];// .buildScratchSize;


		if (rtScratchBuffer) {
			delete rtScratchBuffer;
		}
		i++;
	}

#endif
}
#endif

RHIRaytracingTLAS* VulkanRHI1::RHINewRaytracingTLAS(uint64_t deviceAddress)
{
	VulkanRaytracingTLAS* tlas = new VulkanRaytracingTLAS(Device, deviceAddress);
	return static_cast<RHIRaytracingTLAS*>(tlas);
}

RHIRaytracingTLAS* VulkanRHI1::RHINewRaytracingTLAS(std::vector< RHIRaytracingBLAS* >& BLASs)
{
	VulkanRaytracingTLAS* tlas = new VulkanRaytracingTLAS(Device, BLASs);
	return static_cast<RHIRaytracingTLAS*>(tlas);
}

RHIRaytracingTLAS* VulkanRHI1::RHINewRaytracingTLAS(std::vector< RHIRaytracingBLAS* >& BLASs, std::vector<glm::mat4> transforms)
{
	VulkanRaytracingTLAS* tlas = new VulkanRaytracingTLAS(Device, BLASs, transforms);
	return static_cast<RHIRaytracingTLAS*>(tlas);
}
 
RHIShaderBindingTables* VulkanRHI1::RHINewShaderBindingTables(RHIRaytracingPipeline* pipeline)
{
	VulkanShaderBindingTables* sbt = new VulkanShaderBindingTables(Device, dynamic_cast<VulkanRaytracingPipeline*>(pipeline));
	return static_cast<RHIShaderBindingTables*>(sbt);
}

/*
	2. Operations
*/
// update buffer
void VulkanRHI1::RHIUpdateBuffer(RHIBuffer* Buffer, std::uint32_t Offset, std::uint32_t Size, const void* InData)
{
	VulkanBuffer* VKBuffer = dynamic_cast<VulkanBuffer*>(Buffer);

	std::uint8_t* p;
	Device->MapMemory(VKBuffer->GetMemoryHandle(), Offset, Size, 0, reinterpret_cast<void**>(&p));
	std::memcpy(p, InData, Size);
	Device->UnmapMemory(VKBuffer->GetMemoryHandle());
}

// copy buffer
void VulkanRHI1::RHICopyBuffer(RHIBuffer* SourceBufferRHI, RHIBuffer* DestBufferRHI)
{
	VulkanBuffer* SrcBuffer = dynamic_cast<VulkanBuffer*>(SourceBufferRHI);
	VulkanBuffer* DstBuffer = dynamic_cast<VulkanBuffer*>(DestBufferRHI);

	//VkCommandBuffer copyCmd = vulkanDevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
	VkBufferCopy copyRegion = { 0, 0, SrcBuffer->GetSize()};
	//vkCmdCopyBuffer(copyCmd, SrcBuffer->Handle, DstBuffer->Handle, 1, &copyRegion);
	//ComputeQueue;
	//vulkanDevice->flushCommandBuffer(copyCmd, queue, true);
}

void VulkanRHI1::RHIDispatchComputeShader(std::uint32_t ThreadGroupCountX, std::uint32_t ThreadGroupCountY, std::uint32_t ThreadGroupCountZ)
{
	std::cout << "RHIDispatchComputeShader" << std::endl;
	//pCommandBuffer->CmdBindPipeline(ComputePipeline);

	//pCommandBuffer->CmdBindDescriptorSets(ComputePipeline.BindPoint, ComputePipeline.PipelineLayout, 0, 1,
	//	&DescriptorSet[CurrentFrame].Handle, 0, nullptr);

	CommandBuffer->CmdDispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
	std::cout << "RHIDispatchComputeShader end" << std::endl;
}


VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}
#include <fstream>

static std::vector<char> readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

/*
	5. Update Resource
*/
// update 1D texture
void VulkanRHI1::RHIUpdateTexture1D(RHITexture1D* TextureRHI, uint32_t MipIndex, const RHIUpdateTextureRegion1D& UpdateRegionIn, const uint8_t* SourceDataIn)
{
	const void* InData = SourceDataIn;
	VulkanTexture1D* Texture = dynamic_cast<VulkanTexture1D*>(TextureRHI);
	Texture->UpdateImage(0, InData);
}


// update 2D texture
void VulkanRHI1::RHIUpdateTexture2D(RHITexture2D* TextureRHI, std::uint32_t MipIndex, const RHIUpdateTextureRegion2D& UpdateRegionIn, std::uint32_t SourcePitch, const std::uint8_t* SourceDataIn)
{
	const void* InData = SourceDataIn;
	VulkanTexture2D* Texture = dynamic_cast<VulkanTexture2D*>(TextureRHI); 
	Texture->UpdateImage(0, InData);
}

// update 3D texture
void  VulkanRHI1::RHIUpdateTexture3D(RHITexture3D* TextureRHI, std::uint32_t MipIndex, const RHIUpdateTextureRegion3D& UpdateRegion, std::uint32_t SourceRowPitch, std::uint32_t SourceDepthPitch, const std::uint8_t* SourceDataIn)
{
	const void* InData = SourceDataIn;
	VulkanTexture3D* Texture = dynamic_cast<VulkanTexture3D*>(TextureRHI); 
	Texture->UpdateImage(0, InData);
}

// update cubemap texture
void VulkanRHI1::RHIUpdateTextureCube(RHITextureCube* TextureRHI, CubeMapFace Face, std::uint32_t MipIndex, const RHIUpdateTextureRegion2D& UpdateRegionIn, std::uint32_t SourcePitch, const std::uint8_t* SourceDataIn)
{
	const void* InData = SourceDataIn;
	VulkanTextureCube* Texture = dynamic_cast<VulkanTextureCube*>(TextureRHI);
	Texture->UpdateImage(static_cast<uint32_t>(Face), InData);
}

// update 2D texture array
void  VulkanRHI1::RHIUpdateTexture2DArray(RHITexture2DArray* TextureRHI, std::uint32_t MipIndex, std::uint32_t ArrayIndex, const RHIUpdateTextureRegion2D& UpdateRegionIn, std::uint32_t SourcePitch, const std::uint8_t* SourceDataIn)
{
	const void* InData = SourceDataIn;
	VulkanTexture2DArray* Texture = dynamic_cast<VulkanTexture2DArray*>(TextureRHI);
	Texture->UpdateImage(ArrayIndex, InData);
}

// update cube texture array
void  VulkanRHI1::RHIUpdateTextureCubeArray(RHITextureCubeArray* TextureRHI, CubeMapFace Face, std::uint32_t MipIndex, std::uint32_t ArrayIndex, const RHIUpdateTextureRegion2D& UpdateRegionIn, std::uint32_t SourcePitch, const std::uint8_t* SourceDataIn)
{
	const void* InData = SourceDataIn;
	VulkanTextureCubeArray* Texture = dynamic_cast<VulkanTextureCubeArray*>(TextureRHI);
	Texture->UpdateImage(static_cast<uint32_t>(Face), InData);
}


bool VulkanRHI1::RecreateSwapChain(void)
{
	Device->DeviceWaitIdle();
	//todo...
	//std::unique_ptr<SwapChain>&& tempSwapChain = std::move(_swapChain);
	//_swapChain = std::make_unique<SwapChain>(std::move(tempSwapChain));
	return true;
}

/*
	初始状态:
		Fence[0] unsignaled
		Fence[1] signaled
		Current Frame = 1
	第1帧:
		
*/
RHICommandBuffer* VulkanRHI1::RHIBeginFrame()
{   
	VkResult Result = VK_SUCCESS;
#if 0
	std::vector<VkFence> fences;
	fences.resize(InFlightFences.size());
	for (int i = 0; i < InFlightFences.size(); ++i) {
		fences[i] = InFlightFences[i]->Handle;
	}
	std::cout << "RHIBeginFrame 1" << std::endl;
	Device->WaitForFences(InFlightFences.size(), fences.data(), VK_TRUE, std::numeric_limits<uint64_t>::max());
	std::cout << "RHIBeginFrame 2" << std::endl;
	for (int i = 0; i < InFlightFences.size(); ++i) {
		InFlightFences[i]->Reset();
	}
	std::cout << "RHIBeginFrame 3" << std::endl;
#else
	Result = Device->WaitForFences(1, &Fence->Handle, VK_TRUE, std::numeric_limits<uint64_t>::max());
	if (VK_SUCCESS != Result)
	{
		std::cout << "failed to Wait For Fences " << Result << std::endl;
		throw std::runtime_error("failed to Wait For Fences");
	}
#endif

	if (SwapChain) { 
		
		VkResult result = Device->AcquireNextImageKHR(SwapChain->Handle, UINT64_MAX, ImageAvailableSemaphores[CurrentFrame]->Handle, VK_NULL_HANDLE, &CurrentImageIndex_);
		//VkResult result = _swapChain->acquireNextImage(&_currentImageIndex); 
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			Device->DeviceWaitIdle();
			printf("vkAcquireNextImageKHR! VK_ERROR_OUT_OF_DATE_KHR\n");
			RecreateSwapChain();
			return nullptr;
		}
	}
	if (VK_SUCCESS != CommandBuffer->Reset(0))
	{
		throw std::runtime_error("failed to begin reset command buffer!");
	}
	if (CommandBuffer->Begin(0) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}  
	return CommandBuffer;
}

RHICommandBuffer* VulkanRHI1::RHIBeginOffscreenFrame()
{
	std::vector<VkFence> fences;
	fences.resize(InFlightFences.size());
	for (int i = 0; i < InFlightFences.size(); ++i) {
		fences[i] = InFlightFences[i]->Handle;
	}
	Device->WaitForFences(InFlightFences.size(), fences.data(), VK_TRUE, std::numeric_limits<uint64_t>::max());

	VkResult result = Device->AcquireNextImageKHR(SwapChain->Handle, UINT64_MAX, ImageAvailableSemaphores[CurrentFrame]->Handle, VK_NULL_HANDLE, &CurrentImageIndex_);
	//VkResult result = _swapChain->acquireNextImage(&_currentImageIndex); 
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		Device->DeviceWaitIdle();
		printf("vkAcquireNextImageKHR! VK_ERROR_OUT_OF_DATE_KHR\n");
		RecreateSwapChain();
		return nullptr;
	}

	if (VK_SUCCESS != CommandBuffer->Reset(0))
	{
		throw std::runtime_error("failed to begin reset command buffer!");
	}

	if (CommandBuffer->Begin(0) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}
	return CommandBuffer;
}

void VulkanRHI1::RHIEndFrame() 
{
	VkResult Result = VK_SUCCESS;
	Result = CommandBuffer->End();
	if (Result != VK_SUCCESS)
	{
		std::cout << "error " << Result << std::endl;
		throw std::runtime_error("failed to end command buffer");
	}
#if 0
	InFlightFences[CurrentFrame]->Reset();
#else
	Result = Fence->Reset();
	if (Result != VK_SUCCESS)
	{
		std::cout << "error " << Result << std::endl;
		throw std::runtime_error("failed to reset fence");
	}
#endif
	
#if 1
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkCommandBuffer CommandBufferHandle = CommandBuffer->GetHandle();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO; 
	submitInfo.pWaitDstStageMask = waitStages; 
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CommandBufferHandle;	
	if (SwapChain)
	{	
		//std::cout 
		//	<< "ImageAvailableSemaphores[CurrentFrame]->Handle " << ImageAvailableSemaphores[CurrentFrame]->Handle << " "
		//	<< "RenderFinishedSemaphores[CurrentFrame]->Handle " << RenderFinishedSemaphores[CurrentFrame]->Handle << " "
		//	<< std::endl;
		VkSemaphore waitSemaphores   =  ImageAvailableSemaphores[CurrentFrame]->Handle;
		VkSemaphore signalSemaphores =  RenderFinishedSemaphores[CurrentFrame]->Handle;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &waitSemaphores;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &RenderFinishedSemaphores[CurrentFrame]->Handle;
	}
#if 0
	Result = vkQueueSubmit(GraphicsQueue.Handle, 1, &submitInfo, InFlightFences[CurrentFrame]->Handle);
#else
	//std::cout << "vkQueueSubmit 11111111 "
	//	<< " CurrentFrame " << CurrentFrame << " "
	//	<< submitInfo.pWaitSemaphores[0]    << " "
	//	<< submitInfo.pSignalSemaphores[0]	<< " "
	//	<< std::endl;
	Result = vkQueueSubmit(GraphicsQueue.Handle, 1, &submitInfo, Fence->Handle);
	//std::cout << "vkQueueSubmit 22222222" << std::endl;
#endif
	if(Result != VK_SUCCESS)
	{
		std::cout << "error " << Result << std::endl;
		throw std::runtime_error("failed to submit draw command buffer");
	}
#else
	GraphicsQueue.SubmitSync(pCurCommandBuffer, ImageAvailableSemaphores[CurrentFrame], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, RenderFinishedSemaphores[CurrentFrame], InFlightFences[CurrentFrame]);
#endif

	if (SwapChain) {
		Result = PresentQueue.Present(SwapChain, RenderFinishedSemaphores[CurrentFrame], CurrentImageIndex_);
		if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR /*|| framebufferResized*/) {
			//framebufferResized = false;
			RecreateSwapChain();
			printf("vkQueuePresentKHR VK_ERROR_OUT_OF_DATE_KHR\n");
		}
		else if (Result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}
	} 

	CurrentFrame = (CurrentFrame + 1) % ConcurrentFrameCount;// MAX_FRAMES_IN_FLIGHT;
}

void VulkanRHI1::RHICopyImage()
{

}