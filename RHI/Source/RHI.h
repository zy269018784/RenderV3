#pragma once
#include <RHIBuffer.h>
#include <RHITexture.h>
#include <RHIShader.h>
#include <RHIDefinitions.h>
#include <RHIPipeline.h>
#include <RHICommandBuffer.h>
#include <RHIUpdateTextureRegion2D.h>
#include <RHIUpdateTextureRegion3D.h>
#include <RHIRaytracing.h>

#include <cstdint>
#include <vector>
class RHISampler;
class RHITexture;
class RHIBuffer;
class RHIShader;

class QVulkanWindow;

enum class RHIAccess
{
	// Used when the previous state of a resource is not known,
	// which implies we have to flush all GPU caches etc.
	Unknown = 0,

	// Read states
	CPURead = 1 << 0,
	Present = 1 << 1,
	IndirectArgs = 1 << 2,
	VertexOrIndexBuffer = 1 << 3,
	SRVCompute = 1 << 4,
	SRVGraphics = 1 << 5,
	CopySrc = 1 << 6,
	ResolveSrc = 1 << 7,
	DSVRead = 1 << 8,

	// Read-write states
	UAVCompute = 1 << 9,
	UAVGraphics = 1 << 10,
	RTV = 1 << 11,
	CopyDest = 1 << 12,
	ResolveDst = 1 << 13,
	DSVWrite = 1 << 14,

	// Ray tracing acceleration structure states.
	// Buffer that contains an AS must always be in either of these states.
	// BVHRead -- required for AS inputs to build/update/copy/trace commands.
	// BVHWrite -- required for AS outputs of build/update/copy commands.
	BVHRead = 1 << 15,
	BVHWrite = 1 << 16,

	// Invalid released state (transient resources)
	Discard = 1 << 17,

	// Shading Rate Source
	ShadingRateSource = 1 << 18,

	Last = ShadingRateSource,
	None = Unknown,
	Mask = (Last << 1) - 1,

	// A mask of the two possible SRV states
	SRVMask = SRVCompute | SRVGraphics,

	// A mask of the two possible UAV states
	UAVMask = UAVCompute | UAVGraphics,

	// A mask of all bits representing read-only states which cannot be combined with other write states.
	ReadOnlyExclusiveMask = CPURead | Present | IndirectArgs | VertexOrIndexBuffer | SRVGraphics | SRVCompute | CopySrc | ResolveSrc | BVHRead,

	// A mask of all bits representing read-only states which may be combined with other write states.
	ReadOnlyMask = ReadOnlyExclusiveMask | DSVRead | ShadingRateSource,

	// A mask of all bits representing readable states which may also include writable states.
	ReadableMask = ReadOnlyMask | UAVMask,

	// A mask of all bits representing write-only states which cannot be combined with other read states.
	WriteOnlyExclusiveMask = RTV | CopyDest | ResolveDst,

	// A mask of all bits representing write-only states which may be combined with other read states.
	WriteOnlyMask = WriteOnlyExclusiveMask | DSVWrite,

	// A mask of all bits representing writable states which may also include readable states.
	WritableMask = WriteOnlyMask | UAVMask | BVHWrite
};
ENUM_CLASS_FLAGS(RHIAccess)


/** \defgroup RHI rhi
 *  @{
 */ 
class RHI
{
public:
	enum Implementation {
		Null,
		Vulkan,
		OpenGLES2,
		D3D11,
		Metal,
		D3D12
	};
	virtual ~RHI() {}
	static RHI* create(Implementation impl, QVulkanWindow* window);
	/*
		1. Resource Creation
	*/
	virtual RHIVertexShader* RHILoadVertexShader(const char* filename) = 0;
	virtual RHIPixelShader* RHILoadPixelShader(const char* filename) = 0;
	virtual RHIComputeShader* RHILoadComputeShader(const char* filename) = 0;
	virtual RHIRayGenShader* RHILoadRayGenShader(const char* filename) = 0;
	virtual RHIRayMissShader* RHILoadRayMissShader(const char* filename) = 0;

	// Closest Hit, always present,
	// Any Hit, optional
	// Intersection, optional
	virtual RHIRayHitGroupShader* RHILoadRayHitGroupShader(const char* filename, const char* anyhit, const char* intersect) = 0;

	// create vertex shader
	virtual RHIVertexShader* RHICreateVertexShader(std::vector<char> GlslCode) = 0;

	// create pixel shader
	virtual RHIPixelShader* RHICreatePixelShader(std::vector<char> GlslCode) = 0;

	// create compute shader
	virtual RHIComputeShader* RHICreateComputeShader(std::vector<char> GlslCode) = 0;

	// create 1D texture 
	virtual RHITexture* RHICreateTexture1D(std::uint32_t SizeX, std::uint8_t Format, std::uint32_t NumMips, TextureCreateFlags Flags, RHIAccess InResourceState) = 0;

	// create 2D texture 
	virtual RHITexture* RHICreateTexture2D(std::uint32_t SizeX, std::uint32_t SizeY, std::uint8_t Format, std::uint32_t NumMips, TextureCreateFlags Flags, RHIAccess InResourceState) = 0;

	// create 3D texture 
	virtual RHITexture* RHICreateTexture3D(std::uint32_t SizeX, std::uint32_t SizeY, std::uint32_t SizeZ, std::uint8_t Format, std::uint32_t NumMips, TextureCreateFlags Flags, RHIAccess InResourceState)  = 0;

	// create CubeMap texture 
	virtual RHITexture* RHICreateTextureCube(std::uint32_t Size, std::uint8_t Format, std::uint32_t NumMips, TextureCreateFlags Flags, RHIAccess InResourceState)  = 0;

	// create 2D texture array
	virtual RHITexture* RHICreateTexture2DArray(std::uint32_t SizeX, std::uint32_t SizeY, std::uint32_t SizeZ, std::uint8_t Format, std::uint32_t NumMips, std::uint32_t NumSamples, TextureCreateFlags Flags, RHIAccess InResourceState) = 0;

	// create CubeMap texture array
	virtual RHITexture* RHICreateTextureCubeArray(std::uint32_t SizeX, std::uint32_t SizeY, std::uint32_t SizeZ, std::uint8_t Format, std::uint32_t NumMips, std::uint32_t NumSamples, TextureCreateFlags Flags, RHIAccess InResourceState) = 0;

	// create buffer
	virtual RHIBuffer* RHICreateBuffer(std::uint32_t Size, BufferUsageFlags Usage, std::uint32_t Stride, const void* InData) = 0;




	virtual RHISampler* RHINewSampler() = 0;
	virtual RHIPipeline* RHINewGraphicsPipeline() = 0;
	virtual RHIPipeline* RHINewComputePipeline() = 0; 
	virtual RHIPipeline* RHINewRaytracingPipeline() = 0;
	virtual RHIShaderBindings* RHINewShaderBindings() = 0;
	virtual RHIRenderTarget* RHINewRenderTarget() = 0;
	virtual RHITextureRenderTarget* RHINewTextureRenderTarget(const RHIRenderTargetDesc& desc) = 0;
	virtual RHIRenderPass* RHINewRenderPass() = 0;

	/**
	* @brief 新建光追底层加速结构，针对几何数据层次 
    * @return 返回RHIRaytracingBLAS
	*/
	virtual RHIRaytracingBLAS* RHINewRaytracingBLAS() = 0; 
	virtual RHIRaytracingBLAS* RHINewRaytracingBLAS(RHIRaytracingGeometryData* geoData) = 0;
	virtual RHIRaytracingBLAS* RHINewRaytracingBLAS(RHIRaytracingGeometryDataSimple* geoData) = 0;
	virtual std::vector< RHIRaytracingBLAS* > RHICreateRaytracingBLASs(std::vector< RHIRaytracingGeometryData* >& geometries) = 0;
	virtual void RHIBuildBLASs(std::vector< RHIRaytracingBLAS* >& BLASs) = 0;
	/**
		@brief 新建顶层加速结构，针对场景层次
		@param  deviceAddress : uint64_t ，绑定BLAS设备地址
		@return 返回顶层加速结构指针RHIRaytracingTLAS
	*/
	virtual RHIRaytracingTLAS* RHINewRaytracingTLAS(uint64_t deviceAddress) = 0;
	virtual RHIRaytracingTLAS* RHINewRaytracingTLAS(std::vector< RHIRaytracingBLAS* >& BLASs) = 0;
	virtual RHIRaytracingTLAS* RHINewRaytracingTLAS(std::vector< RHIRaytracingBLAS* >& BLASs, std::vector<glm::mat4> transforms) = 0;
	/**
	* @brief 新建着色器绑定表，描述光线追踪管线流程
	*/
	virtual RHIShaderBindingTables* RHINewShaderBindingTables(RHIRaytracingPipeline* pipeline) = 0;

	/*
		2. Operations
	*/

	// update buffer
	virtual void RHIUpdateBuffer(RHIBuffer* Buffer, std::uint32_t Offset, std::uint32_t Size, const void* InData) = 0;

	// copy buffer
	virtual void RHICopyBuffer(RHIBuffer* SourceBufferRHI, RHIBuffer* DestBufferRHI) = 0;

	// dispatch compute shader
	virtual void RHIDispatchComputeShader(std::uint32_t ThreadGroupCountX, std::uint32_t ThreadGroupCountY, std::uint32_t ThreadGroupCountZ) = 0;

	// update 1D texture
	virtual void RHIUpdateTexture1D(RHITexture1D* TextureRHI, uint32_t MipIndex, const RHIUpdateTextureRegion1D& UpdateRegionIn, const uint8_t* SourceDataIn) = 0;

	// update 2D texture
	virtual void RHIUpdateTexture2D(RHITexture2D* TextureRHI, std::uint32_t MipIndex, const RHIUpdateTextureRegion2D& UpdateRegionIn, std::uint32_t SourcePitch, const std::uint8_t* SourceDataIn) = 0;

	// update 3D texture
	virtual void RHIUpdateTexture3D(RHITexture3D* TextureRHI, std::uint32_t MipIndex, const RHIUpdateTextureRegion3D& UpdateRegion, std::uint32_t SourceRowPitch, std::uint32_t SourceDepthPitch, const std::uint8_t* SourceData) = 0;

	// update cubemap texture
	virtual void RHIUpdateTextureCube(RHITextureCube* TextureRHI, CubeMapFace Face, std::uint32_t MipIndex, const RHIUpdateTextureRegion2D& UpdateRegionIn, std::uint32_t SourcePitch, const std::uint8_t* SourceDataIn) = 0;

	// update 2D texture array
	virtual void RHIUpdateTexture2DArray(RHITexture2DArray* TextureRHI, std::uint32_t MipIndex, std::uint32_t ArrayIndex, const RHIUpdateTextureRegion2D& UpdateRegionIn, std::uint32_t SourcePitch, const std::uint8_t* SourceDataIn) = 0;

	// update cube texture array
	virtual void RHIUpdateTextureCubeArray(RHITextureCubeArray* TextureRHI, CubeMapFace Face, std::uint32_t MipIndex, std::uint32_t ArrayIndex, const RHIUpdateTextureRegion2D& UpdateRegionIn, std::uint32_t SourcePitch, const std::uint8_t* SourceDataIn) = 0;

	/*
		6. Frame 
	*/
	virtual RHICommandBuffer* RHIBeginFrame() = 0;
	virtual RHICommandBuffer* RHIBeginOffscreenFrame() = 0;
	virtual void RHIEndFrame() = 0;

	virtual void RHICopyImage() = 0;

	//virtual RHICommandBuffer* RHIBeginOneTimeGraphicsCommand() = 0;
	//virtual RHICommandBuffer* RHIEndOneTimeGraphicsCommand() = 0;
};


/** @}*/