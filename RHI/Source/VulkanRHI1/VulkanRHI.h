#pragma once
#include "VulkanRHI/VulkanRHI.h"
#include <VulkanRHI1/Vulkan.h>
#include <VulkanRHI1/VulkanObjects/VulkanInstance.h>
#include <VulkanRHI1/VulkanObjects/VulkanSurface.h>
#include <VulkanRHI1/VulkanObjects/VulkanDevice.h>
#include <VulkanRHI1/VulkanObjects/VulkanPhysicalDevice.h>
#include <VulkanRHI1/VulkanObjects/VulkanQueue.h>
#include <VulkanRHI1/VulkanObjects/VulkanSwapChain.h>
#include <VulkanRHI1/VulkanObjects/VulkanCommandPool.h>
#include <VulkanRHI1/VulkanObjects/VulkanCommandBuffer.h>
#include <VulkanRHI1/VulkanObjects/VulkanRenderPass.h>
#include <VulkanRHI1/VulkanObjects/VulkanFrameBuffer.h>
#include <VulkanRHI1/VulkanObjects/VulkanPipeline.h>
#include <VulkanRHI1/VulkanObjects/VulkanPipelineLayout.h>
#include <VulkanRHI1/VulkanObjects/VulkanPipelineCache.h>
#include <VulkanRHI1/VulkanObjects/VulkanDescriptorPool.h>
#include <VulkanRHI1/VulkanObjects/VulkanDescriptorSet.h>
#include <VulkanRHI1/VulkanObjects/VulkanDescriptorSetLayout.h>
#include <VulkanRHI1/VulkanObjects/VulkanBuffer.h>
#include <VulkanRHI1/VulkanObjects/VulkanSemaphore.h>
#include <VulkanRHI1/VulkanObjects/VulkanFence.h>
#ifdef USE_QT
#include <QVulkanWindow>
#endif
#include <RHI.h>
#include <RHIDefinitions.h>
#include <RHIShader.h>
#include <RHIGraphicsPipelineState.h>
#include <RHIPipeline.h>
#include <RHIRenderPass.h>
#include <RHIRenderTarget.h>

#include <glfw3.h>
#include <vector>

#define VK_DYNAMIC_STATE_BEGIN_RANGE (VK_DYNAMIC_STATE_VIEWPORT)
#define VK_DYNAMIC_STATE_END_RANGE (VK_DYNAMIC_STATE_STENCIL_REFERENCE)
#define VK_DYNAMIC_STATE_RANGE_SIZE (VK_DYNAMIC_STATE_STENCIL_REFERENCE - VK_DYNAMIC_STATE_VIEWPORT + 1)

std::vector<const char*> getRequiredExtensions();
 
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

// make_unique is not available in C++11
// Taken from Herb Sutter's blog (https://herbsutter.com/gotw/_102/)
template<typename T, typename ...Args>
std::unique_ptr<T> make_unique(Args&& ...args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
} 
namespace VK {


	class Thread
	{
	private:
		bool destroying = false;
		std::thread worker;
		std::queue<std::function<void()>> jobQueue;
		std::mutex queueMutex;
		std::condition_variable condition;

		// Loop through all remaining jobs
		void queueLoop()
		{
			while (true) {
				std::function<void()> job;
				{
					std::unique_lock<std::mutex> lock(queueMutex);
					condition.wait(lock, [this] { return !jobQueue.empty() || destroying; });
					if (destroying) {
						break;
					}
					job = jobQueue.front();
				}

				job();

				{
					std::lock_guard<std::mutex> lock(queueMutex);
					jobQueue.pop();
					condition.notify_one();
				}
			}
		}

	public:
		Thread()
		{
			worker = std::thread(&Thread::queueLoop, this);
		}

		~Thread()
		{
			if (worker.joinable()) {
				wait();
				queueMutex.lock();
				destroying = true;
				condition.notify_one();
				queueMutex.unlock();
				worker.join();
			}
		}

		// Add a new job to the thread's queue
		void addJob(std::function<void()> function)
		{
			std::lock_guard<std::mutex> lock(queueMutex);
			jobQueue.push(std::move(function));
			condition.notify_one();
		}

		// Wait until all work items have been finished
		void wait()
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			condition.wait(lock, [this]() { return jobQueue.empty(); });
		}
	};

	class ThreadPool
	{
	public:
		std::vector< Thread* > threads;

		// Sets the number of threads to be allocated in this pool
		void setThreadCount(uint32_t count)
		{
			threads.clear();
			for (uint32_t i = 0; i < count; i++)
			{
				threads.push_back( new Thread() );
			}
		}

		// Wait until all threads have finished their work items
		void wait()
		{
			for (auto& thread : threads) {
				thread->wait();
				delete thread;
			}
		}
	};

}

class VulkanRHI1 : public RHI
{
public:   
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};


	std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
public:
	VulkanRHI1(int GpuIndex = 0);
#ifdef USE_QT
	VulkanRHI1(QVulkanWindow *window);
#endif
	VulkanRHI1(GLFWwindow* window);
	~VulkanRHI1();

	VulkanDevice* GetDevice();
	VulkanSwapChain* GetSwapChain();

	void Release();
public:
	/*
		1. Resource Creation
	*/
	virtual RHIVertexShader* RHILoadVertexShader(const char* filename) final override;
	virtual RHIPixelShader* RHILoadPixelShader(const char* filename) final override;
	virtual RHIComputeShader* RHILoadComputeShader(const char* filename) final override;
	virtual RHIRayGenShader* RHILoadRayGenShader(const char* filename) final override;
	virtual RHIRayMissShader* RHILoadRayMissShader(const char* filename) final override; 
	virtual RHIRayHitGroupShader* RHILoadRayHitGroupShader(const char* filename, const char* anyhit, const char* intersect) final override;
	
	// create vertex shader
	virtual RHIVertexShader* RHICreateVertexShader(std::vector<char> GlslCode) final override;

	// create pixel shader
	virtual RHIPixelShader* RHICreatePixelShader(std::vector<char> GlslCode) final override;

	// create compute shader
	virtual RHIComputeShader* RHICreateComputeShader(std::vector<char> GlslCode) final override;

	// create 1D texture 
	virtual RHITexture* RHICreateTexture1D(std::uint32_t SizeX, std::uint8_t Format, std::uint32_t NumMips, TextureCreateFlags Flags, RHIAccess InResourceState) final override;

	// create 2D texture 
	virtual RHITexture* RHICreateTexture2D(std::uint32_t SizeX, std::uint32_t SizeY, std::uint8_t Format, std::uint32_t NumMips, TextureCreateFlags Flags, RHIAccess InResourceState) final override;

	// create 3D texture 
	virtual RHITexture* RHICreateTexture3D(std::uint32_t SizeX, std::uint32_t SizeY, std::uint32_t SizeZ, std::uint8_t Format, std::uint32_t NumMips, TextureCreateFlags Flags, RHIAccess InResourceState)  final override;

	// create CubeMap texture 
	virtual RHITexture* RHICreateTextureCube(std::uint32_t Size, std::uint8_t Format, std::uint32_t NumMips, TextureCreateFlags Flags, RHIAccess InResourceState)  final override;

	// create 2D texture array
	virtual RHITexture* RHICreateTexture2DArray(std::uint32_t SizeX, std::uint32_t SizeY, std::uint32_t SizeZ, std::uint8_t Format, std::uint32_t NumMips, std::uint32_t NumSamples, TextureCreateFlags Flags, RHIAccess InResourceState)  final override;
	//
	// create CubeMap texture array
	virtual RHITexture* RHICreateTextureCubeArray(std::uint32_t SizeX, std::uint32_t SizeY, std::uint32_t SizeZ, std::uint8_t Format, std::uint32_t NumMips, std::uint32_t NumSamples, TextureCreateFlags Flags, RHIAccess InResourceState) final override;

	// create buffer
	virtual RHIBuffer* RHICreateBuffer(std::uint32_t Size, BufferUsageFlags Usage, std::uint32_t Stride, const void* InData) final override;

	VulkanCommandBuffer* BeginCommand();
	void EndCommand(VulkanCommandBuffer* cmdBuffer);

	void InitMultiThreadCommandBuffers(int threads);
	void DestroyMultiThreadCommandBuffers();
	void BeginCommands();
	void EndCommands();
	void SubmitCommands();
	std::vector< VulkanCommandBuffer* > cmdBuffers;
	VK::ThreadPool threadPool;
	virtual RHISampler* RHINewSampler();
	virtual RHIPipeline* RHINewGraphicsPipeline();
	virtual RHIPipeline* RHINewComputePipeline(); 
	virtual RHIPipeline* RHINewRaytracingPipeline();
	virtual RHIShaderBindings* RHINewShaderBindings(); 
	virtual RHIRenderTarget* RHINewRenderTarget();
	virtual RHITextureRenderTarget* RHINewTextureRenderTarget(const RHIRenderTargetDesc& desc);
	virtual RHIRenderPass* RHINewRenderPass();
	
	virtual RHIRaytracingBLAS* RHINewRaytracingBLAS();
	virtual RHIRaytracingBLAS* RHINewRaytracingBLAS(RHIRaytracingGeometryData* geoData); 
	virtual RHIRaytracingBLAS* RHINewRaytracingBLAS(RHIRaytracingGeometryDataSimple* geoData);
	virtual std::vector< RHIRaytracingBLAS* > RHICreateRaytracingBLASs(std::vector< RHIRaytracingGeometryData* >& geometries);
	virtual void RHIBuildBLASs(std::vector< RHIRaytracingBLAS* >& BLASs);

	virtual RHIRaytracingTLAS* RHINewRaytracingTLAS(uint64_t deviceAddress);
	virtual RHIRaytracingTLAS* RHINewRaytracingTLAS(std::vector< RHIRaytracingBLAS* >& BLASs);
	virtual RHIRaytracingTLAS* RHINewRaytracingTLAS(std::vector< RHIRaytracingBLAS* >& BLASs,std::vector<glm::mat4> transforms);
	virtual RHIShaderBindingTables* RHINewShaderBindingTables(RHIRaytracingPipeline* pipeline);
	/*
		2. Operations
	*/

	// update buffer
	virtual void RHIUpdateBuffer(RHIBuffer* Buffer, std::uint32_t Offset, std::uint32_t Size, const void* InData) final override;

	// copy buffer
	virtual void RHICopyBuffer(RHIBuffer* SourceBufferRHI, RHIBuffer* DestBufferRHI) final override;

	// dispatch compute shader
	virtual void RHIDispatchComputeShader(std::uint32_t ThreadGroupCountX, std::uint32_t ThreadGroupCountY, std::uint32_t ThreadGroupCountZ) final override;

	/*
		5. Update Resource
	*/
	// update 1D texture
	virtual void RHIUpdateTexture1D(RHITexture1D* TextureRHI, uint32_t MipIndex, const RHIUpdateTextureRegion1D& UpdateRegionIn, const uint8_t* SourceDataIn) final override;

	// update 2D texture
	virtual void RHIUpdateTexture2D(RHITexture2D* TextureRHI, std::uint32_t MipIndex, const RHIUpdateTextureRegion2D& UpdateRegionIn, std::uint32_t SourcePitch, const std::uint8_t* SourceDataIn) final override;

	// update 3D texture
	virtual void RHIUpdateTexture3D(RHITexture3D* TextureRHI, std::uint32_t MipIndex, const RHIUpdateTextureRegion3D& UpdateRegion, std::uint32_t SourceRowPitch, std::uint32_t SourceDepthPitch, const std::uint8_t* SourceData) final override;

	// update cubemap texture
	virtual void RHIUpdateTextureCube(RHITextureCube* TextureRHI, CubeMapFace Face, std::uint32_t MipIndex, const RHIUpdateTextureRegion2D& UpdateRegionIn, std::uint32_t SourcePitch, const std::uint8_t* SourceDataIn) final override;

	// update 2D texture array
	virtual void RHIUpdateTexture2DArray(RHITexture2DArray* TextureRHI, std::uint32_t MipIndex, std::uint32_t ArrayIndex, const RHIUpdateTextureRegion2D& UpdateRegionIn, std::uint32_t SourcePitch, const std::uint8_t* SourceDataIn) final override;

	// update cube texture array
	virtual void RHIUpdateTextureCubeArray(RHITextureCubeArray* TextureRHI, CubeMapFace Face, std::uint32_t MipIndex, std::uint32_t ArrayIndex, const RHIUpdateTextureRegion2D& UpdateRegionIn, std::uint32_t SourcePitch, const std::uint8_t* SourceDataIn) final override;

	/*
		6. Frame
	*/
	virtual RHICommandBuffer* RHIBeginFrame() final override;
	virtual RHICommandBuffer* RHIBeginOffscreenFrame() final override;
	virtual void RHIEndFrame() final override;
	virtual void RHICopyImage();

private:
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
	void QueryAdvanceFeathers();
	void QueryAdvanceProperties();

	void AddRaytracingExtensions();
	void GetInstanceLayersAndExtensions();

	void CreatePhysicalDevice();
	void CreatePhysicalDevice(int Index);
	
	void CreateDevice();
	void CreateDeviceWithoutUI();

	void CreateSyncObjects();

	void ComputeShaderResource();

	void CreateCommandBuffer();
private:
	bool RecreateSwapChain(void);

	VkShaderModule ReadShader(std::string filename);

private:	
public:
	/*
		指定GPU
	*/
	int GpuIndex = 0;
	VulkanInstance				*Instance;
	VulkanDevice				*Device;
	VulkanPhysicalDevice		*PhysicalDevice = nullptr;
	VulkanCommandPool			*CommandPool;	
	VulkanCommandBuffer			*CommandBuffer;	
	/*
		同步对象
		ImageAvailableSemaphores.size() = ConcurrentFrameCount
		RenderFinishedSemaphores.size() = ConcurrentFrameCount
		InFlightFences.size() = ConcurrentFrameCount
	*/
    std::vector< VulkanSemaphore* > ImageAvailableSemaphores;
	std::vector< VulkanSemaphore* > RenderFinishedSemaphores;
	std::vector< VulkanFence* > InFlightFences;
	VulkanFence* Fence;
	/*
		Compure Shader需要
	*/
	VulkanCommandPool			*ComputeCommandPool = nullptr;
	VulkanCommandBuffer			*ComputeCommandBuffer = nullptr;
	VulkanFence					*ComputeFence = nullptr;

	/*
		屏幕渲染需要
	*/
	VulkanRenderPass			*pRenderPass;
	VulkanSurface				*Surface = nullptr;
	VulkanSwapChain				*SwapChain = nullptr;
	std::vector<VulkanFrameBuffer*>	FrameBuffers;
public:
	VulkanQueue					GraphicsQueue;
	VulkanQueue					PresentQueue;
	VulkanQueue					ComputeQueue;
	
	uint32_t queueFamilyIndex = 0;
	uint32_t					CurrentImageIndex_ = 0; 
	uint32_t					CurrentFrame = 0;
	uint32_t					ConcurrentFrameCount = 2;


	VulkanBuffer* asBuffer = nullptr;
	std::vector<VulkanBuffer*> asBuffers;
};

